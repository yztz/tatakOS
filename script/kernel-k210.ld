OUTPUT_ARCH( "riscv" )
ENTRY( _entry )

SECTIONS
{
  /*
   * 与qemu地址有出入，注意！
   */
  . = 0x80020000; 
  INCLUDE kernel.ld
}
