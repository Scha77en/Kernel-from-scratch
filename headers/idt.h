#ifndef IDT_H
#define IDT_H

#include <types.h>
#include <pic.h>
#include <keyboard.h>
#include <panic.h>

typedef struct {
	u16    isr_low;
	u16    kernel_cs;
	u8     reserved;
	u8     attributes;
	u16    isr_high;
} __attribute__((packed)) idt_entry_t;

typedef struct {
	u16	limit;
	u32	base;
} __attribute__((packed)) idtr_t;

#define IDT_ENTRIES 256
#define IDT_MAX_DESCRIPTORS 256

void	interrupt_handler(u32 irq_num);

void	idt_set_descriptor(u8 vector, void* isr, u8 flags);
void	idt_init(void);

void	timer_handler(void);

#endif

