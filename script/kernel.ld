.text : {
    *(.text.entry)
    *(.text .text.*)
    PROVIDE(cp_start = .);
    *(.copy_*)
    PROVIDE(cp_end = .);
    . = ALIGN(0x1000);
    PROVIDE(etext = .);
}

.rodata : {
    . = ALIGN(16);
    *(.srodata .srodata.*) /* do not need to distinguish this from .rodata */
    . = ALIGN(16);
    *(.rodata .rodata.*)
}

.data : {
    . = ALIGN(16);
    *(.sdata .sdata.*) /* do not need to distinguish this from .data */
    . = ALIGN(16);
    *(.data .data.*)
}

.bss : {
    . = ALIGN(16);
    *(.kstack0)
    . = ALIGN(16);
    PROVIDE(bss_start = .);
    *(.sbss .sbss.*) /* do not need to distinguish this from .bss */
    . = ALIGN(16);
    *(.bss .bss.*)
    PROVIDE(bss_end = .);
}
. = ALIGN(0x1000); /* 保证end为页对齐 */
PROVIDE(end = .);