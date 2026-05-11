#ifndef IDT_H
#define IDT_H

#include "types.h"
#include "pic.h"
#include "keyboard.h"

#define IDT_CPU_EXCEPTIONS 36

typedef struct {
	u16    isr_low;      // The lower 16 bits of the ISR's address
	u16    kernel_cs;    // The GDT segment selector that the CPU will load into CS before calling the ISR
	u8     reserved;     // Set to zero
	u8     attributes;   // Type and attributes; see the IDT page
	u16    isr_high;     // The higher 16 bits of the ISR's address
} __attribute__((packed)) idt_entry_t;

typedef struct {
	u16	limit; // Size of IDT - 1
	u32	base; // Base address of IDT
} __attribute__((packed)) idtr_t;

#define IDT_ENTRIES 256
#define IDT_MAX_DESCRIPTORS 256

__attribute__((aligned(0x10)))
static idt_entry_t idt[IDT_ENTRIES]; // Array of IDT entries, aligned for performance.

static idtr_t idtr;

void	interrupt_handler(u32 irq_num);

void	idt_set_descriptor(u8 vector, void* isr, u8 flags);
void	idt_init(void);

void	timer_handler(void);

// idt_set_descriptor() Parameters
// vector - Interrupt number (0-255) - index into IDT array
// isr - Function pointer to interrupt handler (e.g., isr_stub_0)
// flags - IDT gate descriptor attributes byte:
// Bit 7: Present (1 = valid)
// Bits 5-6: DPL (privilege level 0-3)
// Bits 0-4: Gate type (0xE = 32-bit interrupt gate)


#endif

