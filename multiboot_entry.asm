MBALIGN  equ  1 << 0
MEMINFO  equ  1 << 1
FLAGS    equ  MBALIGN | MEMINFO
MAGIC    equ  0x1BADB002
CHECKSUM equ -(MAGIC + FLAGS)


section .multiboot
align 4
    dd MAGIC
    dd FLAGS
    dd CHECKSUM

; ========== GDT for Kernel ==========
section .data
align 16
gdt_start:
    dq 0x0000000000000000

; --- Kernel Code Segment ---
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 0b10011010
    db 0b11001111
    db 0x00

; --- Kernel Data Segment ---
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 0b10010010
    db 0b11001111
    db 0x00

; --- Kernel Stack Segment ---
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 0b10010010
    db 0b11001111
    db 0x00

; --- User Code Segment ---
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 0b11111010
    db 0b11001111
    db 0x00

; --- User Data Segment ---
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 0b11110010
    db 0b11001111
    db 0x00

; --- User Stack Segment ---
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 0b11110010
    db 0b11001111
    db 0x00

gdt_end:

gdt_descriptor:
    dw (gdt_end - gdt_start - 1)
    dd 0x00000800

CODE_SEG equ 0x08
DATA_SEG equ 0x10
STACK_SEG equ 0x18

section .text
global _start

_start:
    cli

    mov esp, stack_top
    mov ebp, stack_top

    mov esi, gdt_start
    mov edi, 0x00000800
    mov ecx, (gdt_end - gdt_start)
    cld
    rep movsb

    lgdt [gdt_descriptor]

    jmp CODE_SEG:setup_segments

setup_segments:

	mov ax, DATA_SEG
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

    move ax, STACK_SEG
	mov ss, ax

	extern main
	call main

	jmp $

section .bss
align 16
stack_bottom:
    resb 16384
stack_top:
