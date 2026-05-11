/* Entry point: where execution starts */
ENTRY(_start)

SECTIONS
{
    /* Kernel loaded at 0x100000 (1MB) - standard for Multiboot */
    . = 0x100000;

    /* Multiboot header MUST be first (within first 8KB) */
    .multiboot ALIGN(4K) : {
        *(.multiboot)
    }

    /* Executable code section */
    .text ALIGN(4K) : {
        *(.text)
    }

    /* Read-only data (strings, constants) */
    .rodata ALIGN(4K) : {
        *(.rodata)
    }

    /* Initialized data (global variables with values) */
    .data ALIGN(4K) : {
        *(.data)
    }

    /* Uninitialized data (stack, BSS) */
    .bss ALIGN(4K) : {
        *(COMMON)
        *(.bss)
    }
}
