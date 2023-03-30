//
// driver for qemu's virtio disk device.
// uses qemu's mmio interface to virtio.
//
// qemu ... -drive file=fs.img,if=none,format=raw,id=x0 -device virtio-blk-device,drive=x0,bus=virtio-mmio-bus.0
//

#include "riscv.h"
#include "kernel/proc.h"
#include "common.h"
#include "platform/qemu.h"
#include "atomic/spinlock.h"
#include "atomic/sleeplock.h"
#include "fs/fs.h"
#include "fs/blk_device.h"
#include "virtio.h"
#include "driver/plic.h"
#include "mm/io.h"
#include "mm/page.h"
#include "mm/alloc.h"
#include "bio.h"

#define __MODULE_NAME__ VIRTIO_DISK
#include "debug.h"

// the address of virtio mmio register r.
static uint64_t virtio_base_address;
#define R(r) ((volatile uint32_t *)(virtio_base_address + (r)))

static struct disk {
    // the first is a set (not a ring) of DMA descriptors, with which the
    // driver tells the device where to read and write individual
    // disk operations. there are NUM descriptors.
    // most commands consist of a "chain" (a linked list) of a couple of
    // these descriptors.
    struct virtq_desc *desc;

    // next is a ring in which the driver writes descriptor numbers
    // that the driver would like the device to process.  it only
    // includes the head descriptor of each chain. the ring has
    // NUM elements.
    struct virtq_avail *avail;

    // finally a ring in which the device writes descriptor numbers that
    // the device has finished processing (just the head of each chain).
    // there are NUM used ring entries.
    struct virtq_used *used;

    // our own book-keeping.
    char free[NUM];  // is a descriptor free?
    uint16_t used_idx; // we've looked this far in used[2..NUM].

    // track info about in-flight operations,
    // for use when completion interrupt arrives.
    // indexed by first descriptor index of chain.
    struct {
        bio_vec_t *bio_vec;
        char status;
    } info[NUM];

    // disk command headers.
    // one-for-one with descriptors, for convenience.
    struct virtio_blk_req ops[NUM];

    // struct spinlock vdisk_lock;
    wq_t vdisk_queue;

} disk;

void virtio_disk_intr(void);

void virtio_disk_init(void) {
    uint32_t status = 0;

    // initlock(&disk.vdisk_lock, "virtio_disk");
    disk.vdisk_queue = (wq_t)INIT_WAIT_QUEUE(disk.vdisk_queue);
    // we need to remap io to virt addr
    virtio_base_address = ioremap(VIRTIO0, PGSIZE);

    plic_register_handler(VIRTIO0_IRQ, (plic_irq_callback_t)virtio_disk_intr, NULL);
 
    if (*R(VIRTIO_MMIO_MAGIC_VALUE) != 0x74726976 ||
        *R(VIRTIO_MMIO_VERSION) != 2 ||
        *R(VIRTIO_MMIO_DEVICE_ID) != 2 ||
        *R(VIRTIO_MMIO_VENDOR_ID) != 0x554d4551) {
        panic("could not find virtio disk");
    }

    // reset device
    *R(VIRTIO_MMIO_STATUS) = status;

    // set ACKNOWLEDGE status bit
    status |= VIRTIO_CONFIG_S_ACKNOWLEDGE;
    *R(VIRTIO_MMIO_STATUS) = status;

    // set DRIVER status bit
    status |= VIRTIO_CONFIG_S_DRIVER;
    *R(VIRTIO_MMIO_STATUS) = status;

    // negotiate features
    uint64_t features = *R(VIRTIO_MMIO_DEVICE_FEATURES);
    features &= ~(1 << VIRTIO_BLK_F_RO);
    features &= ~(1 << VIRTIO_BLK_F_SCSI);
    features &= ~(1 << VIRTIO_BLK_F_CONFIG_WCE);
    features &= ~(1 << VIRTIO_BLK_F_MQ);
    features &= ~(1 << VIRTIO_F_ANY_LAYOUT);
    features &= ~(1 << VIRTIO_RING_F_EVENT_IDX);
    features &= ~(1 << VIRTIO_RING_F_INDIRECT_DESC);
    *R(VIRTIO_MMIO_DRIVER_FEATURES) = features;

    // tell device that feature negotiation is complete.
    status |= VIRTIO_CONFIG_S_FEATURES_OK;
    *R(VIRTIO_MMIO_STATUS) = status;

    // re-read status to ensure FEATURES_OK is set.
    status = *R(VIRTIO_MMIO_STATUS);
    if(!(status & VIRTIO_CONFIG_S_FEATURES_OK))
        panic("virtio disk FEATURES_OK unset");

    // initialize queue 0.
    *R(VIRTIO_MMIO_QUEUE_SEL) = 0;

    // ensure queue 0 is not in use.
    if(*R(VIRTIO_MMIO_QUEUE_READY))
        panic("virtio disk should not be ready");

    // check maximum queue size.
    uint32_t max = *R(VIRTIO_MMIO_QUEUE_NUM_MAX);
    if (max == 0)
        panic("virtio disk has no queue 0");
    if (max < NUM)
        panic("virtio disk max queue too short");
    
    // allocate and zero queue memory.
    disk.desc = kzalloc(PGSIZE);
    disk.avail = kzalloc(PGSIZE);
    disk.used = kzalloc(PGSIZE);
    if(!disk.desc || !disk.avail || !disk.used)
        panic("virtio disk kalloc");

    // set queue size.
    *R(VIRTIO_MMIO_QUEUE_NUM) = NUM;

    // write physical addresses.
    *R(VIRTIO_MMIO_QUEUE_DESC_LOW) = (uint64_t)disk.desc;
    *R(VIRTIO_MMIO_QUEUE_DESC_HIGH) = (uint64_t)disk.desc >> 32;
    *R(VIRTIO_MMIO_DRIVER_DESC_LOW) = (uint64_t)disk.avail;
    *R(VIRTIO_MMIO_DRIVER_DESC_HIGH) = (uint64_t)disk.avail >> 32;
    *R(VIRTIO_MMIO_DEVICE_DESC_LOW) = (uint64_t)disk.used;
    *R(VIRTIO_MMIO_DEVICE_DESC_HIGH) = (uint64_t)disk.used >> 32;

    // queue is ready.
    *R(VIRTIO_MMIO_QUEUE_READY) = 0x1;

    // all NUM descriptors start out unused.
    for (int i = 0; i < NUM; i++)
        disk.free[i] = 1;

    // tell device we're completely ready.
    status |= VIRTIO_CONFIG_S_DRIVER_OK;
    *R(VIRTIO_MMIO_STATUS) = status;

    // plic.c and trap.c arrange for interrupts from VIRTIO0_IRQ.
}

// find a free descriptor, mark it non-free, return its index.
static int alloc_desc() {
    for (int i = 0; i < NUM; i++) {
        if (disk.free[i]) {
            disk.free[i] = 0;
            return i;
        }
    }
    return -1;
}

// mark a descriptor as free.
static void free_desc(int i) {
    if (i >= NUM)
        panic("free_desc 1");
    if (disk.free[i])
        panic("free_desc 2");
    disk.desc[i].addr = 0;
    disk.desc[i].len = 0;
    disk.desc[i].flags = 0;
    disk.desc[i].next = 0;
    disk.free[i] = 1;
    // wakeup(&disk.free[0]);
    wq_wakeup_locked(&disk.vdisk_queue);
}

// free a chain of descriptors.
static void free_chain(int i) {
    while (1) {
        int flag = disk.desc[i].flags;
        int nxt = disk.desc[i].next;
        free_desc(i);
        if (flag & VRING_DESC_F_NEXT)
            i = nxt;
        else
            break;
    }
}

// allocate three descriptors (they need not be contiguous).
// disk transfers always use three descriptors.
static int alloc3_desc(int *idx) {
    for (int i = 0; i < 3; i++) {
        idx[i] = alloc_desc();
        if (idx[i] < 0) {
            for (int j = 0; j < i; j++)
                free_desc(idx[j]);
            return -1;
        }
    }
    return 0;
}

void virtio_disk_rw(bio_vec_t *bio_vec, int write) {

    uint64_t sector = bio_vec->bv_start_num;

    acquire(&disk.vdisk_queue.wq_lock);

    // the spec's Section 5.2 says that legacy block operations use
    // three descriptors: one for type/reserved/sector, one for the
    // data, one for a 1-byte status result.

    // allocate the three descriptors.
    int idx[3];

    wait_event_locked(&disk.vdisk_queue, alloc3_desc(idx) == 0);


    // format the three descriptors.
    // qemu's virtio-blk.c reads them.

    struct virtio_blk_req *buf0 = &disk.ops[idx[0]];

    if (write)
        buf0->type = VIRTIO_BLK_T_OUT; // write the disk
    else
        buf0->type = VIRTIO_BLK_T_IN; // read the disk
    buf0->reserved = 0;
    buf0->sector = sector;

    disk.desc[idx[0]].addr = (uint64_t)buf0;
    disk.desc[idx[0]].len = sizeof(struct virtio_blk_req);
    disk.desc[idx[0]].flags = VRING_DESC_F_NEXT;
    disk.desc[idx[0]].next = idx[1];

    disk.desc[idx[1]].addr = (uint64_t)bio_vec->bv_buff;
    disk.desc[idx[1]].len = BSIZE * bio_vec->bv_count;
    if (write)
        disk.desc[idx[1]].flags = 0; // device reads b->data
    else
        disk.desc[idx[1]].flags = VRING_DESC_F_WRITE; // device writes b->data
    disk.desc[idx[1]].flags |= VRING_DESC_F_NEXT;
    disk.desc[idx[1]].next = idx[2];

    disk.info[idx[0]].status = 0xff; // device writes 0 on success
    disk.desc[idx[2]].addr = (uint64_t)&disk.info[idx[0]].status;
    disk.desc[idx[2]].len = 1; // char为1个字节
    disk.desc[idx[2]].flags = VRING_DESC_F_WRITE; // device writes the status
    disk.desc[idx[2]].next = 0;

    // record blk_buf_t for virtio_disk_intr().
    bio_vec->disk = 1;
    disk.info[idx[0]].bio_vec = bio_vec;

    // tell the device the first index in our chain of descriptors.
    disk.avail->ring[disk.avail->idx % NUM] = idx[0];

    __sync_synchronize();

    // tell the device another avail ring entry is available.
    disk.avail->idx += 1; // not % NUM ...

    __sync_synchronize();

    *R(VIRTIO_MMIO_QUEUE_NOTIFY) = 0; // value is queue number

    // Wait for virtio_disk_intr() to say request has finished.
    wait_event_locked(&disk.vdisk_queue, bio_vec->disk == 0);

    disk.info[idx[0]].bio_vec = 0;
    free_chain(idx[0]);

    // release(&disk.vdisk_lock);
    release(&disk.vdisk_queue.wq_lock);
}

void virtio_disk_intr() {
    acquire(&disk.vdisk_queue.wq_lock);

    // the device won't raise another interrupt until we tell it
    // we've seen this interrupt, which the following line does.
    // this may race with the device writing new entries to
    // the "used" ring, in which case we may process the new
    // completion entries in this interrupt, and have nothing to do
    // in the next interrupt, which is harmless.
    *R(VIRTIO_MMIO_INTERRUPT_ACK) = *R(VIRTIO_MMIO_INTERRUPT_STATUS) & 0x3;

    __sync_synchronize();

    // the device increments disk.used->idx when it
    // adds an entry to the used ring.

    while (disk.used_idx != disk.used->idx) {
        __sync_synchronize();
        int id = disk.used->ring[disk.used_idx % NUM].id;

        /* 如果为0，说明操作成功，否则失败，见前面的赋值为ff */
        if (disk.info[id].status != 0)
            panic("virtio_disk_intr status");

        bio_vec_t *bio_vec = disk.info[id].bio_vec;
        bio_vec->disk = 0;   // disk is done with buf
        wq_wakeup_locked(&disk.vdisk_queue);

        disk.used_idx += 1;
    }
    release(&disk.vdisk_queue.wq_lock);
}

void disk_io(bio_vec_t *bio_vec, int write) {
    virtio_disk_rw(bio_vec, write);
}
