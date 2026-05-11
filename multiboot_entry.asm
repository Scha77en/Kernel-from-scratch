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
    ; Null descriptor (required)
    dq 0x0000000000000000
    
    ; Code descriptor (kernel mode)
    ; Base: 0x0, Limit: 0xFFFFF
    ; Present=1, DPL=0, Type=1 (code)
    ; Readable=1, Conforming=0, Accessed=0
    ; Granularity=1 (4KB), DB=1 (32-bit)
    dw 0xFFFF           ; Limit 0-15
    dw 0x0000           ; Base 0-15
    db 0x00             ; Base 16-23
    db 0b10011010       ; Access byte (P=1, DPL=0, S=1, Type=1010)
    db 0b11001111       ; Granularity (G=1, DB=1, L=0, AVL=0), Limit 16-19
    db 0x00             ; Base 24-31
    
    ; Data descriptor (kernel mode)
    ; Base: 0x0, Limit: 0xFFFFF
    ; Present=1, DPL=0, Type=0 (data)
    ; Writable=1, Expand=0, Accessed=0
    ; Granularity=1 (4KB), DB=1 (32-bit)
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 0b10010010       ; Access byte (P=1, DPL=0, S=1, Type=0010)
    db 0b11001111       ; Granularity
    db 0x00

gdt_end:

gdt_descriptor:
    dw (gdt_end - gdt_start - 1)  ; Size - 1
    dd gdt_start                   ; Base address

; GDT segment selectors
CODE_SEG equ 0x08
DATA_SEG equ 0x10

section .text
global _start
_start:
    cli                 ; Disable interrupts
    
    ; Load our GDT (replace GRUB's GDT)
    lgdt [gdt_descriptor]
    
    ; Far jump to set CS to our code segment
    jmp CODE_SEG:setup_segments
    
setup_segments:
	; Set data segment selectors
	mov ax, DATA_SEG
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

	; Set up stack
	mov esp, stack_top
	mov ebp, stack_top

	; Now call kernel main (interrupts still disabled)
	extern main
	call main

	; Hang if kernel returns
	jmp $

section .bss
align 16
stack_bottom:
    resb 16384
stack_top:
