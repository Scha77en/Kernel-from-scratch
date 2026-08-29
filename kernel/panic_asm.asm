global panic_clear_registers_and_halt

section .text

panic_clear_registers_and_halt:
    cli

    xor eax, eax
    xor ebx, ebx
    xor ecx, ecx
    xor edx, edx
    xor esi, esi
    xor edi, edi
    xor ebp, ebp

.halt_loop:
    hlt
    jmp .halt_loop  ; Loop in case an NMI wakes the CPU
