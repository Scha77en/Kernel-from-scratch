global isr_stub_table
global isr_stub_128

extern interrupt_handler
extern schedule_signal
extern syscall_handler

%macro isr_err_stub 1
isr_stub_%+%1:
	pushad
	push %1
	call interrupt_handler
	add esp, 4
	popad
	add esp, 4
	iretd
%endmacro

%macro isr_no_err_stub 1
isr_stub_%+%1:
	pushad
	push %1
	call interrupt_handler
	add esp, 4
	popad
	iretd
%endmacro

; --- Syscall Interrupt Stub (Vector 128 / 0x80) ---
isr_stub_128:
	pushad              ; Push EDI, ESI, EBP, ESP, EBX, EDX, ECX, EAX
	push ds

	mov ax, 0x10        ; Load Kernel Data Segment
	mov ds, ax
	mov es, ax

	push esp            ; Pass registers_t pointer to C handler
	call syscall_handler
	add esp, 4

	pop ds
	popad               ; Restores updated registers (including EAX return value)
	iretd

isr_no_err_stub 0
isr_no_err_stub 1
isr_no_err_stub 2
isr_no_err_stub 3
isr_no_err_stub 4
isr_no_err_stub 5
isr_no_err_stub 6
isr_no_err_stub 7
isr_err_stub    8
isr_no_err_stub 9
isr_err_stub    10
isr_err_stub    11
isr_err_stub    12
isr_err_stub    13
isr_err_stub    14
isr_no_err_stub 15
isr_no_err_stub 16
isr_err_stub    17
isr_no_err_stub 18
isr_no_err_stub 19
isr_no_err_stub 20
isr_no_err_stub 21
isr_no_err_stub 22
isr_no_err_stub 23
isr_no_err_stub 24
isr_no_err_stub 25
isr_no_err_stub 26
isr_no_err_stub 27
isr_no_err_stub 28
isr_no_err_stub 29
isr_err_stub    30
isr_no_err_stub 31

%assign i 32
%rep 16
	isr_no_err_stub i
%assign i i+1
%endrep

isr_stub_table:
	%assign i 0
	%rep 48
		dd isr_stub_%+i
	%assign i i+1
	%endrep


