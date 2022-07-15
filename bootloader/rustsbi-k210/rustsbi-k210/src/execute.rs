use core::{
    arch::asm,
    ops::{Generator, GeneratorState},
    pin::Pin,
};

use riscv::register::{
    scause::{self, Exception, Trap, Interrupt},
    mepc,mtval,
    mtvec::{self, TrapMode},
};
use rustsbi::println;
use crate::feature;
use crate::runtime::{MachineTrap, Runtime, SupervisorContext};

pub fn execute_supervisor(supervisor_mepc: usize, a0: usize, a1: usize) -> ! {
    let mut rt = Runtime::new_sbi_supervisor(supervisor_mepc, a0, a1);
    loop {
        match Pin::new(&mut rt).resume(()) {
            GeneratorState::Yielded(MachineTrap::SbiCall()) => {
                let ctx = rt.context_mut();
                if emulate_sbi_call(ctx) {
                    continue;
                }
                // feature::preprocess_supervisor_external(ctx); // specific for 1.9.1; see document for details
                let param = [ctx.a0, ctx.a1, ctx.a2, ctx.a3, ctx.a4, ctx.a5];
                let ans = rustsbi::ecall(ctx.a7, ctx.a6, param);
                ctx.a0 = ans.error;
                ctx.a1 = ans.value;
                ctx.mepc = ctx.mepc.wrapping_add(4);
            }
            GeneratorState::Yielded(MachineTrap::IllegalInstruction()) => {
                let ctx = rt.context_mut();
                // FIXME: get_vaddr_u32这个过程可能出错。
                let ins = unsafe { get_vaddr_u32(ctx.mepc) } as usize;
                if !emulate_illegal_instruction(ctx, ins) {
                    unsafe {
                        if feature::should_transfer_trap(ctx) {
                            feature::do_transfer_trap(
                                ctx,
                                Trap::Exception(Exception::IllegalInstruction),
                            )
                        } else {
                            fail_illegal_instruction(ctx, ins)
                        }
                    }
                }
            }
            GeneratorState::Yielded(MachineTrap::ExternalInterrupt()) => unsafe {
                // println!("external...");
                let ctx = rt.context_mut();
                feature::call_supervisor_interrupt(ctx)
            },
            GeneratorState::Yielded(MachineTrap::MachineTimer()) => {
                feature::forward_supervisor_timer()
            }
            GeneratorState::Yielded(MachineTrap::MachineSoft()) => {
                feature::forward_supervisor_soft()
            }
            // todo：编写样例，验证store page fault和instruction page fault
            GeneratorState::Yielded(MachineTrap::InstructionFault(addr)) => {
                let ctx = rt.context_mut();
                if feature::is_page_fault(addr) {
                    unsafe {
                        feature::do_transfer_trap(
                            ctx,
                            Trap::Exception(Exception::InstructionPageFault),
                        )
                    }
                } else {
                    unsafe {
                        feature::do_transfer_trap(ctx, Trap::Exception(Exception::InstructionFault))
                    }
                }
            }
            GeneratorState::Yielded(MachineTrap::LoadFault(addr)) => {
                let ctx = rt.context_mut();
                if feature::is_page_fault(addr) {
                    unsafe {
                        feature::do_transfer_trap(ctx, Trap::Exception(Exception::LoadPageFault))
                    }
                } else {
                    unsafe { feature::do_transfer_trap(ctx, Trap::Exception(Exception::LoadFault)) }
                }
            }
            GeneratorState::Yielded(MachineTrap::StoreFault(addr)) => {
                let ctx = rt.context_mut();
                if feature::is_page_fault(addr) {
                    unsafe {
                        feature::do_transfer_trap(ctx, Trap::Exception(Exception::StorePageFault))
                    }
                } else {
                    unsafe {
                        feature::do_transfer_trap(ctx, Trap::Exception(Exception::StoreFault))
                    }
                }
            }
            GeneratorState::Yielded(MachineTrap::LoadMisaligned()) => {
                let ctx = rt.context_mut();
                unsafe {
                    feature::do_transfer_trap(ctx, Trap::Exception(Exception::LoadMisaligned))
                }
            }
            GeneratorState::Yielded(MachineTrap::StoreMisaligned()) => {
                let ctx = rt.context_mut();
                unsafe {
                    feature::do_transfer_trap(ctx, Trap::Exception(Exception::StoreMisaligned))
                }
            }
            GeneratorState::Complete(()) => unreachable!(),
        }
    }
}

#[inline]
fn set_register_xi(ctx: &mut SupervisorContext, i: u8, data: usize) {
    let registers = unsafe { &mut *(ctx as *mut _ as *mut [usize; 31]) };
    assert!(i <= 31, "i should be valid register target");
    if i == 0 {
        // x0, don't modify
        return;
    }
    registers[(i - 1) as usize] = data;
}

#[inline]
fn get_register_xi(ctx: &mut SupervisorContext, i: u8) -> usize {
    let registers = unsafe { &mut *(ctx as *mut _ as *mut [usize; 31]) };
    assert!(i <= 31, "i should be valid register target");
    if i == 0 {
        // x0, don't modify
        return 0;
    }
    registers[(i - 1) as usize]
}

#[inline]
fn get_register_xic(ctx: &SupervisorContext, i: u8) -> usize {
    match i {
        0b010 => ctx.a0,
        0b011 => ctx.a1,
        0b100 => ctx.a2,
        0b101 => ctx.a3,
        0b110 => ctx.a4,
        0b111 => ctx.a5,
        0b000 => ctx.s0,
        0b001 => ctx.s1,
        _ => panic!("invalid get compressed target {}", i),
    }
}

#[inline]
unsafe fn get_vaddr_u32(vaddr: usize) -> u32 {
    get_vaddr_u16(vaddr) as u32 | ((get_vaddr_u16(vaddr.wrapping_add(2)) as u32) << 16)
}

#[inline]
unsafe fn get_vaddr_u16(vaddr: usize) -> u16 {
    let mut ans: u16;
    asm!("
        li      {2}, (1 << 17)
        csrrs   {2}, mstatus, {2}
        lhu     {0}, 0({1})
        csrw    mstatus, {2}
    ", out(reg) ans, in(reg) vaddr, out(reg) _);
    ans
}

fn emulate_sbi_call(ctx: &mut SupervisorContext) -> bool {
    if feature::sbi_set_mext(ctx) {
        return true;
    }
    false
}

fn emulate_illegal_instruction(ctx: &mut SupervisorContext, ins: usize) -> bool {
    if feature::emulate_rdtime(ctx, ins) {
        return true;
    }
    if feature::emulate_sfence_vma(ctx, ins) {
        return true;
    }
    false
}

// 真·非法指令异常，是M层出现的
fn fail_illegal_instruction(ctx: &mut SupervisorContext, ins: usize) -> ! {
    panic!("invalid instruction from machine level, mepc: {:016x?}, instruction: {:016x?}, context: {:016x?}", ctx.mepc, ins, ctx);
}
