#ifndef _H_RISCV_
#define _H_RISCV_

#include "types.h"
#include "platform.h"

// Supervisor Status Register, sstatus

#define SSTATUS_SPP (1L << 8)  // Previous mode, 1=Supervisor, 0=User
#define SSTATUS_SPIE (1L << 5) // Supervisor Previous Interrupt Enable
#define SSTATUS_UPIE (1L << 4) // User Previous Interrupt Enable
#define SSTATUS_SIE (1L << 1)  // Supervisor Interrupt Enable
#define SSTATUS_FS (3L << 13)
#define SSTATUS_UIE (1L << 0)  // User Interrupt Enable
#define SSTATUS_SUM (1L << 18) // Supervisor User Memory access


#define read_csr(reg) ({ unsigned long __tmp; \
  asm volatile ("csrr %0, " #reg : "=r"(__tmp)); \
  __tmp; })

#define write_csr(reg, val) ({ \
  if (__builtin_constant_p(val) && (unsigned long)(val) < 32) \
    asm volatile ("csrw " #reg ", %0" :: "i"(val)); \
  else \
    asm volatile ("csrw " #reg ", %0" :: "r"(val)); })

#define swap_csr(reg, val) ({ unsigned long __tmp; \
  if (__builtin_constant_p(val) && (unsigned long)(val) < 32) \
    asm volatile ("csrrw %0, " #reg ", %1" : "=r"(__tmp) : "i"(val)); \
  else \
    asm volatile ("csrrw %0, " #reg ", %1" : "=r"(__tmp) : "r"(val)); \
  __tmp; })

#define set_csr(reg, bit) ({ unsigned long __tmp; \
  if (__builtin_constant_p(bit) && (unsigned long)(bit) < 32) \
    asm volatile ("csrrs %0, " #reg ", %1" : "=r"(__tmp) : "i"(bit)); \
  else \
    asm volatile ("csrrs %0, " #reg ", %1" : "=r"(__tmp) : "r"(bit)); \
  __tmp; })

#define clear_csr(reg, bit) ({ unsigned long __tmp; \
  if (__builtin_constant_p(bit) && (unsigned long)(bit) < 32) \
    asm volatile ("csrrc %0, " #reg ", %1" : "=r"(__tmp) : "i"(bit)); \
  else \
    asm volatile ("csrrc %0, " #reg ", %1" : "=r"(__tmp) : "r"(bit)); \
  __tmp; })


extern void push_off(void);
extern void pop_off(void);
/* 在特权级1.9版本中，SUM位为PUM为，其功能位与SUM作用相反 */
static inline void enable_sum() {
    push_off();
#if PRIVILEGE_VERSION == PRIVILEGE_VERSION_1_12
    set_csr(sstatus, SSTATUS_SUM);
#elif PRIVILEGE_VERSION == PRIVILEGE_VERSION_1_9
    // clear_csr(sstatus, SSTATUS_SUM);
#else 
#error "enable_sum"
#endif
}

static inline void disable_sum() {
#if PRIVILEGE_VERSION == PRIVILEGE_VERSION_1_12
    clear_csr(sstatus, SSTATUS_SUM);
#elif PRIVILEGE_VERSION == PRIVILEGE_VERSION_1_9
    // set_csr(sstatus, SSTATUS_SUM);
#else 
#error "disable_sum"
#endif
    pop_off();
}

// Supervisor Interrupt Enable
#define SIE_SEIE (1L << 9) // external
#define SIE_STIE (1L << 5) // timer
#define SIE_SSIE (1L << 1) // software


// SIP
#define SIP_SSIP (1L << 1)
#define SIP_STIP (1L << 5)
#define SIP_SEIP (1L << 9)


// use riscv's sv39 page table scheme.
#define SATP_SV39 (8L << 60)

#define set_pgtbl(pagetable) write_csr(satp, (SATP_SV39 | (((uint64_t)(pagetable)) >> 12)))
#define get_pgtbl() ((pagetable_t)(read_csr(satp) << 12))



// enable device interrupts
static inline void intr_on() {
    set_csr(sstatus, SSTATUS_SIE);
}

// disable device interrupts
static inline void intr_off() {
    clear_csr(sstatus, SSTATUS_SIE);
}

// are device interrupts enabled?
static inline int intr_get() {
    return (read_csr(sstatus) & SSTATUS_SIE);
}

// read and write tp, the thread pointer, which holds
// this core's hartid (core number), the index into cpus[].
static inline uint64_t r_tp() {
    uint64_t x;
    asm volatile("mv %0, tp" : "=r" (x));
    return x;
}

static inline uint64_t r_sp() {
    uint64_t x;
    asm volatile("mv %0, sp" : "=r" (x));
    return x;
}

static inline uint64_t r_fp() {
    uint64_t x;
    asm volatile("mv %0, fp" : "=r" (x));
    return x;
}

static inline void w_tp(uint64_t x) {
    asm volatile("mv tp, %0" : : "r" (x));
}

static inline uint64_t r_ra() {
    uint64_t x;
    asm volatile("mv %0, ra" : "=r" (x));
    return x;
}

// flush the TLB.
static inline void sfence_vma(void) {
    __asm__ __volatile__("fence\nfence.i\nsfence.vma" : : : "memory");
}

static inline void sfence_vma_addr(uint64_t addr) {
    __asm__ __volatile__("sfence.vma %0" : : "r" (addr) : "memory");
}


static inline struct proc *get_current() {
    return (struct proc *)read_csr(sscratch);
}

#define current (get_current())

static void inline set_current(struct proc *p) {
    write_csr(sscratch, p);
}

char *riscv_cause2str(uint64_t scause);

/**
 * @brief Convert system prot to riscv prot
 *
 * @param linux_prot
 * @return uint32_t
 */
uint32_t riscv_map_prot(uint32_t linux_prot);


/* TRAP */
#define INTERRUPT 0x8000000000000000UL
#define EXCEPTION 0x0000000000000000UL

#define INTR_SOFT   (INTERRUPT  + 1)
#define INTR_TIMER  (INTERRUPT  + 5)
#define INTR_EXT    (INTERRUPT  + 9) // NOT SUPPORT ON K210

#define EXCP_LOAD_MISALIGNED (EXCEPTION + 4)
#define EXCP_LOAD_FAULT (EXCEPTION + 5)
#define EXCP_STORE_MISALIGNED (EXCEPTION + 6)
#define EXCP_STORE_FAULT (EXCEPTION + 7)
#define EXCP_SYSCALL (EXCEPTION  + 8)
#define EXCP_INSTR_PAGE_FAULT (EXCEPTION  + 12)
#define EXCP_LOAD_PAGE_FAULT (EXCEPTION  + 13)
#define EXCP_STORE_PAGE_FAULT (EXCEPTION  + 15)

#define IS_INTR(scause) ((scause) & INTERRUPT)

#define when_pagefault \
    case EXCP_LOAD_FAULT: \
    case EXCP_LOAD_PAGE_FAULT: \
    case EXCP_STORE_FAULT: \
    case EXCP_STORE_PAGE_FAULT:

#define when_softirq \
    case INTR_SOFT:

#define when_hardirq \
    case INTR_EXT: 

#define when_clock \
    case INTR_TIMER:

#define when_irq \
    when_hardirq \
    when_softirq \
    when_clock

#define when_syscall \
    case EXCP_SYSCALL:

#define when_misalign \
    case EXCP_LOAD_MISALIGNED: \
    case EXCP_STORE_MISALIGNED: 

#endif
