global isr_stub_table
extern interrupt_handler

; [ESP+12] SS (if privilege change)
; [ESP+8]  ESP (if privilege change)
; [ESP+4]  EFLAGS
; [ESP]    CS:EIP
; [ESP-4]  Error code ← pushed by CPU for certain exceptions

; iret expects stack: CS:EIP | EFLAGS | SS:ESP. Error code still there → stack corruption.

; Macro for exceptions that push error code
%macro isr_err_stub 1
isr_stub_%+%1:
	push %1
	call interrupt_handler
	add esp, 8	; removing error code
	iret 
%endmacro

; Macro for exceptions that don't push error code
%macro isr_no_err_stub 1

isr_stub_%+%1:
	push %1
	call interrupt_handler
	add esp, 4
	iret
%endmacro

; CPU Exception handlers (0-31)
; ref : Table 6-1. Protected-Mode Exceptions and Interrupts  6-2, Vol. 3A
; Intel® 64 and IA-32 Architectures Software Developer’s Manual, Combined Volumes:
isr_no_err_stub 0   ; Divide Error
isr_no_err_stub 1   ; Debug
isr_no_err_stub 2   ; NMI
isr_no_err_stub 3   ; Breakpoint
isr_no_err_stub 4   ; Overflow
isr_no_err_stub 5   ; BOUND Range Exceeded
isr_no_err_stub 6   ; Invalid Opcode
isr_no_err_stub 7   ; Device Not Available
isr_err_stub    8   ; Double Fault (has error code)
isr_no_err_stub 9   ; Coprocessor Segment Overrun
isr_err_stub    10  ; Invalid TSS (has error code)
isr_err_stub    11  ; Segment Not Present (has error code)
isr_err_stub    12  ; Stack-Segment Fault (has error code)
isr_err_stub    13  ; General Protection (has error code)
isr_err_stub    14  ; Page Fault (has error code)
isr_no_err_stub 15  ; Reserved
isr_no_err_stub 16  ; x87 FPU Error
isr_err_stub    17  ; Alignment Check (has error code)
isr_no_err_stub 18  ; Machine Check
isr_no_err_stub 19  ; SIMD Floating-Point Exception
isr_no_err_stub 20  ; Virtualization Exception
isr_no_err_stub 21  ; Control Protection Exception
isr_no_err_stub 22  ; Reserved
isr_no_err_stub 23  ; Reserved
isr_no_err_stub 24  ; Reserved
isr_no_err_stub 25  ; Reserved
isr_no_err_stub 26  ; Reserved
isr_no_err_stub 27  ; Reserved
isr_no_err_stub 28  ; Hypervisor Injection Exception
isr_no_err_stub 29  ; VMM Communication Exception
isr_err_stub    30  ; Security Exception (has error code)
isr_no_err_stub 31  ; Reserved

; IRQ Hnadlers (32 - 47)
%assign i 32
%rep 16
	isr_no_err_stub i
%assign i i+1
%endrep

; "stub table". (This is used to prevent excessive code reuse, and not related to actual function.)
isr_stub_table:
%assign i 0
%rep 48
	dd isr_stub_%+i ; dd = define doubleword (4-bytes = 32-bit pointer)
%assign i i+1
%endrep


