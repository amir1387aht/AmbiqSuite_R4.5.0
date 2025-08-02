/****************************************************************************** 
 * linker_script.ld - Linker script for applications using startup_gnu.c 
 * 
 *****************************************************************************/
ENTRY(Reset_Handler)

MEMORY
{
    MCU_MRAM     (rx)  : ORIGIN = 0x00018000, LENGTH = 1998848
    MCU_TCM      (rwx) : ORIGIN = 0x10000000, LENGTH = 393216
    SHARED_SRAM  (rwx) : ORIGIN = 0x10060000, LENGTH = 1048576
}

SECTIONS
{
    .text :
    {
        . = ALIGN(4);
        KEEP(*(.isr_vector))
        KEEP(*(.patch))
        *(.text)
        *(.text*)
        *(.rodata)
        *(.rodata*)
        . = ALIGN(4);
        _etext = .;
    } > MCU_MRAM

    /* User stack section, moved to SHARED_SRAM to avoid MCU_TCM overflow */
    .stack (NOLOAD):
    {
        . = ALIGN(8);
        *(.stack)
        *(.stack*)
        . = ALIGN(8);
        __stack_start__ = ORIGIN(SHARED_SRAM);
        __stack_end__ = __stack_start__ + LENGTH(SHARED_SRAM) / 3; /* 1/3 of SHARED_SRAM for stack */
    } > SHARED_SRAM

    .heap (NOLOAD):
    {
        __heap_start__ = .;
        KEEP(*(.heap))
        . = ALIGN(8);
        __heap_end__ = __heap_start__ + LENGTH(SHARED_SRAM) / 3; /* Another 1/3 for heap */
    } > SHARED_SRAM

    .data :
    {
        . = ALIGN(4);
        _sdata = .;
        *(.data)
        *(.data*)
        *(.ramfunc) 
        . = ALIGN(4);
        _edata = .;
    } > MCU_TCM AT>MCU_MRAM

    /* used by startup to initialize data */
    _init_data = LOADADDR(.data);

    .bss :
    {
        . = ALIGN(4);
        _sbss = .;
        *(.bss)
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
    } > SHARED_SRAM  /* Moved to SHARED_SRAM */

    .shared (NOLOAD):
    {
        . = ALIGN(4);
        KEEP(*(.resource_table))
        KEEP(*(.shared))
        . = ALIGN(4);
    } > SHARED_SRAM AT>MCU_MRAM

    .buffers (NOLOAD) :
    {
        *(.display_buffer)
    } > SHARED_SRAM

    .ARM.attributes 0 : { *(.ARM.attributes) }
}
