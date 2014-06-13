#include <l4.h>

ENTRY(_start)

MEMORY
{
    ram(RWX) :   ORIGIN = VIRT_ADDR_RAM, LENGTH = 0x10000000
}

#define rom     ram

SECTIONS
{
    /* Readonly sections starts here. */

    .text :
    {
        _start_rom = .;

        . = ALIGN(64K);
        *(.init.head)
        *(.init.start)
        *(.base)
        *(.text.startup)
        *(.text)
    } > rom

    .rodata . :
    {
        *(.rodata)
        *(.rodata.*)
    } > rom
    _end_rom = .;                       /* ROM ends here. */
    _start_init = .;
    .init . :
    {
        *(.init)
        *(.init.*)
    } > rom
    .roinit . :
    {
        *(.roinit)
    } > rom
    .elfweaver_info :
    {
        *(.elfweaver_info)
    } > rom
    _end_init = .;

    /* Readwrite sections starts here. */
    .data :
    {

        _start_ram = .;                 /* RAM starts here. */
        _start_data = .;
        *(.data.cpulocal)
        *(.data.cpulocal.stack)
        *(.data.cpulocal.utcb)
        *(.data.cpulocal.tcb)
        *(.sdata)
        *(.data)
        /**(.data.*)*/
    } > ram
    .bss . :
    {
        . = ALIGN(16);
        _start_setlist = .;
        *(.setlist)
        _end_setlist = .;
        . = ALIGN(16);
        _start_sets = .;
        *(SORT(set_*))
        _end_sets = .;
	. = ALIGN(32);
        *(.bss)
        *(.kdebug-bss)
    } > ram

    . = ALIGN(16K);
    .kspace . :
    {
        *(.data.kspace)
    } > ram
    . = ALIGN(4K);
    .traps . :
    {
        *(.data.traps)
    } > ram

    .utcb_page . :
    {
        *(.data.utcb)
    } > ram

    . = ALIGN(4K);
    _end_ram = .; 
    
    /* Required for user code! */
    . = ALIGN(4K);
    _end = .;

    /DISCARD/ :
    {
        /*              *(*)            <-- THIS CAUSES SEGFAULT!!! */
        *(.eh_frame)
        *(.note)
        *(.comment)
        *(.delete)
    }
}

ASSERT(SIZEOF(.traps) <= 0x1000, "ERROR: trap section is larger than 4K!!")
