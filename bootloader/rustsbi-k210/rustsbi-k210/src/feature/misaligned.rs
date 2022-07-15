use core::arch::asm;

#[derive(Copy, Clone, Debug, PartialEq, Eq)]
pub enum MemoryUnit {
    Byte,
    HalfWord,
    Word,
    DoubleWord,
    Unknown,
}

impl MemoryUnit {
    pub fn to_bytenum(&self) -> usize {
        match self {
            MemoryUnit::Unknown => 0,
            MemoryUnit::Byte => 1,
            MemoryUnit::HalfWord => 2,
            MemoryUnit::Word => 4,
            MemoryUnit::DoubleWord => 8,
        }
    }

    pub fn to_bitwidth(&self) -> usize {
        let ret = self.to_bytenum() * 8;
        ret
    }

    pub fn from(code: usize) -> Self {
        match code {
            0 => MemoryUnit::Byte,
            1 => MemoryUnit::HalfWord,
            2 => MemoryUnit::Word,
            3 => MemoryUnit::DoubleWord,
            _ => MemoryUnit::Unknown,
        }
    }
}

#[inline]
pub unsafe fn load_vaddr(vaddr: usize, mem_unit: MemoryUnit, signed: bool) -> usize {
    let mut ans: usize;
    let align_mask = mem_unit.to_bytenum() - 1;
    let addr_offset = vaddr & align_mask;
    let vaddr = vaddr - addr_offset;
    let bit_offset = addr_offset * 8;
    let neg_offset = mem_unit.to_bitwidth() - bit_offset;
    match mem_unit {
        MemoryUnit::HalfWord => {
            asm!("
            li      {mprv}, (1 << 17)
            csrrs   {mprv}, mstatus, {mprv}
            fence   iorw, iorw
            lhu     {ans}, 0({vaddr})
            beqz    {bit_off}, 1f                   # vaddr naturally aligned
            lhu     {vaddr}, 2({vaddr})             # use vaddr to store high bits
            srl     {ans}, {ans}, {bit_off}         # low bits
            sll     {vaddr}, {vaddr}, {neg_off}     # high bits
            or      {ans}, {ans}, {vaddr}           # concat
        1:  csrw    mstatus, {mprv}
            ",
            vaddr = in(reg) vaddr,
            bit_off = in(reg) bit_offset,
            neg_off = in(reg) neg_offset,
            mprv = out(reg) _,
            ans = out(reg) ans
            );
        }
        MemoryUnit::Word => {
            asm!("
            li      {mprv}, (1 << 17)
            csrrs   {mprv}, mstatus, {mprv}
            fence   iorw, iorw
            lwu     {ans}, 0({vaddr})
            beqz    {bit_off}, 1f                   # vaddr naturally aligned
            lwu     {vaddr}, 4({vaddr})             # use vaddr to store high bits
            srl     {ans}, {ans}, {bit_off}         # low bits
            sll     {vaddr}, {vaddr}, {neg_off}     # high bits
            or      {ans}, {ans}, {vaddr}           # concat
        1:  csrw    mstatus, {mprv}
            ",
            vaddr = in(reg) vaddr,
            bit_off = in(reg) bit_offset,
            neg_off = in(reg) neg_offset,
            mprv = out(reg) _,
            ans = out(reg) ans
            );
        }
        MemoryUnit::DoubleWord => {
            asm!("
            li      {mprv}, (1 << 17)
            csrrs   {mprv}, mstatus, {mprv}
            fence   iorw, iorw
            ld      {ans}, 0({vaddr})
            beqz    {bit_off}, 1f                   # vaddr naturally aligned
            ld      {vaddr}, 8({vaddr})             # use vaddr to store high bits
            srl     {ans}, {ans}, {bit_off}         # low bits
            sll     {vaddr}, {vaddr}, {neg_off}     # high bits
            or      {ans}, {ans}, {vaddr}           # concat
        1:  csrw    mstatus, {mprv}
            ",
            vaddr = in(reg) vaddr,
            bit_off = in(reg) bit_offset,
            neg_off = in(reg) neg_offset,
            mprv = out(reg) _,
            ans = out(reg) ans
            );
        }
        _ => {
            panic!("[rustsbi misaligned] Unsupported memory load unit!");
            // ans = 0;
        }
    }
    if signed {
        let bitshift = 64 - mem_unit.to_bitwidth();
        let mut signed_ans = ans as isize;
        signed_ans = signed_ans << bitshift;
        signed_ans = signed_ans >> bitshift;
        ans = signed_ans as usize;
    } else if mem_unit != MemoryUnit::DoubleWord {
        let mask = (1 << mem_unit.to_bitwidth()) - 1;
        ans = ans & mask;
    }
    ans
}

#[inline]
#[allow(unused)]
pub unsafe fn store_vaddr(vaddr: usize, mem_unit: MemoryUnit, store_value: usize) {
    let shift = 64 - mem_unit.to_bitwidth();
    let store_value = store_value << shift;
    let store_value = store_value >> shift;
    let align_mask = mem_unit.to_bytenum() - 1;
    let addr_offset = vaddr & align_mask;
    let vaddr = vaddr - addr_offset;
    let bit_offset = addr_offset * 8;
    let neg_offset = mem_unit.to_bitwidth() - bit_offset;
    match mem_unit {
        MemoryUnit::HalfWord => {
            asm!("
            li      {mprv}, (1 << 17)
            csrrs   {mprv}, mstatus, {mprv}
            beqz    {bit_off}, 1f                       # vaddr naturally aligned
            fence   iorw, iorw
            lhu     {store_hi}, 2({vaddr})
            srl     {store_hi}, {store_hi}, {bit_off}
            sll     {store_hi}, {store_hi}, {bit_off}   # clear low bits in vaddr[2]
            srl     {store_lo}, {value}, {neg_off}      # clear high bits in value
            or      {store_hi}, {store_hi}, {store_lo}  # concat vaddr[2]
            sh      {store_hi}, 2({vaddr})
            lhu     {store_lo}, 0({vaddr})
            li      {store_hi}, 64
            sub     {neg_off}, {store_hi}, {bit_off}
            sll     {store_lo}, {store_lo}, {neg_off}
            srl     {store_lo}, {store_lo}, {neg_off}   # clear high bits in vaddr[0]
            sll     {value}, {value}, {bit_off}         # clear low bits in value
            or      {value}, {store_lo}, {value}        # concat vaddr[0]
        1:  sh      {value}, 0({vaddr})
            fence   iorw, iorw
            csrw    mstatus, {mprv}
            ",
            vaddr = in(reg) vaddr,
            bit_off = in(reg) bit_offset,
            neg_off = in(reg) neg_offset,
            value = in(reg) store_value,
            mprv = out(reg) _,
            store_lo = out(reg) _,
            store_hi = out(reg) _
            );
        }
        MemoryUnit::Word => {
            asm!("
            li      {mprv}, (1 << 17)
            csrrs   {mprv}, mstatus, {mprv}
            beqz    {bit_off}, 1f                       # vaddr naturally aligned
            fence   iorw, iorw
            lwu     {store_hi}, 4({vaddr})
            srl     {store_hi}, {store_hi}, {bit_off}
            sll     {store_hi}, {store_hi}, {bit_off}   # clear low bits in vaddr[4]
            srl     {store_lo}, {value}, {neg_off}      # clear high bits in value
            or      {store_hi}, {store_hi}, {store_lo}  # concat vaddr[4]
            sw      {store_hi}, 4({vaddr})
            lwu     {store_lo}, 0({vaddr})
            li      {store_hi}, 64
            sub     {neg_off}, {store_hi}, {bit_off}
            sll     {store_lo}, {store_lo}, {neg_off}
            srl     {store_lo}, {store_lo}, {neg_off}   # clear high bits in vaddr[0]
            sll     {value}, {value}, {bit_off}         # clear low bits in value
            or      {value}, {store_lo}, {value}        # concat vaddr[0]
        1:  sw      {value}, 0({vaddr})
            fence   iorw, iorw
            csrw    mstatus, {mprv}
            ",
            vaddr = in(reg) vaddr,
            bit_off = in(reg) bit_offset,
            neg_off = in(reg) neg_offset,
            value = in(reg) store_value,
            mprv = out(reg) _,
            store_lo = out(reg) _,
            store_hi = out(reg) _
            );
        }
        MemoryUnit::DoubleWord => {
            asm!("
            li      {mprv}, (1 << 17)
            csrrs   {mprv}, mstatus, {mprv}
            beqz    {bit_off}, 1f                       # vaddr naturally aligned
            fence   iorw, iorw
            ld      {store_hi}, 8({vaddr})
            srl     {store_hi}, {store_hi}, {bit_off}
            sll     {store_hi}, {store_hi}, {bit_off}   # clear low bits in vaddr[8]
            srl     {store_lo}, {value}, {neg_off}      # clear low bits in value
            or      {store_hi}, {store_hi}, {store_lo}  # concat vaddr[8]
            sd      {store_hi}, 8({vaddr})
            ld      {store_lo}, 0({vaddr})
            sll     {store_lo}, {store_lo}, {neg_off}
            srl     {store_lo}, {store_lo}, {neg_off}   # clear high bits in vaddr[0]
            sll     {value}, {value}, {bit_off}         # clear low bits in value
            or      {value}, {store_lo}, {value}        # concat vaddr[0]
        1:  sd      {value}, 0({vaddr})
            fence   iorw, iorw
            csrw    mstatus, {mprv}
            ",
            vaddr = in(reg) vaddr,
            bit_off = in(reg) bit_offset,
            neg_off = in(reg) neg_offset,
            value = in(reg) store_value,
            mprv = out(reg) _,
            store_lo = out(reg) _,
            store_hi = out(reg) _
            );
        }
        _ => {
            panic!("[rustsbi misaligned] Unsupported memory store unit!");
        }
    }
}