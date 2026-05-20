#include "../headers/screen.h"
#include "../headers/idt.h"

void	timer_handler(void) {
	// print_string("TICK", (BLACK << 4) + CYAN);
	PIC_sendEOI(0); // IRQ0 ackowleging interrupt compeletion for PIC. So PIC can send next interrupt.
	return ;
}

static _bool vectors[IDT_MAX_DESCRIPTORS];

extern void* isr_stub_table[];

// general exception handler
void	interrupt_handler(u32 irq_num) {
	//print_int(irq_num);
	if (irq_num == 32) {
		// print_string("Inside 32!!", (BLACK << 4) + YELLOW);
		timer_handler();
		return ;
	}
	else if (irq_num == 33) {
		//print_string("Inside 33!!", (BLACK << 4) + YELLOW);
		keyboard_handler();
		return ;
	}
	else
		// print_string("Halting The CPU!!", (BLACK << 4) + YELLOW);
		// __asm__ volatile ("cli; hlt"); // completely hangs the computer
		return ;
}

static u16	get_current_code_segment(void) {
	u16	cs;
	__asm__ volatile("mov %%cs, %0" : "=r"(cs));
	return cs;
}

void	idt_set_descriptor(u8 vector, void* isr, u8 flags) {
	idt_entry_t* descriptor = &idt[vector];

	descriptor->isr_low = (u32)isr & 0xFFFF;
	descriptor->kernel_cs = get_current_code_segment();
	descriptor->reserved = 0;
	descriptor->attributes = flags;
	descriptor->isr_high = (u32)isr >> 16;
}

void idt_init(void) {
    idtr.base = (u32)&idt[0];
    idtr.limit = (u16)sizeof(idt_entry_t) * IDT_MAX_DESCRIPTORS - 1;

    for (u8 vector = 0; vector < 48; vector++) {
        idt_set_descriptor(vector, isr_stub_table[vector], 0x8E);
        vectors[vector] = true;
    }

    IRQ_clear_mask(0);
    IRQ_clear_mask(1);

    __asm__ volatile ("lidt %0" : : "m"(idtr));
    
    // **DEBUG: Verify IDT was loaded**
    idtr_t verify_idtr;
    __asm__ volatile ("sidt %0" : "=m"(verify_idtr));
    
    //print_string((u8 *)"IDT Base loaded: ", WHITE_ON_BLACK);
    //print_hex(verify_idtr.base);
    //print_char('\n', WHITE_ON_BLACK);
    
    //print_string((u8 *)"IDT Limit loaded: ", WHITE_ON_BLACK);
    //print_hex(verify_idtr.limit);
    //print_char('\n', WHITE_ON_BLACK);
    
    //print_string((u8 *)"ISR stub 33 addr: ", WHITE_ON_BLACK);
    //print_hex((u32)isr_stub_table[33]);
    //print_char('\n', WHITE_ON_BLACK);
    
    __asm__ volatile ("sti");
}

/*void	idt_init(void) {
	idtr.base = (u32)&idt[0];
	idtr.limit = (u16)sizeof(idt_entry_t) * IDT_MAX_DESCRIPTORS - 1;

	for (u8 vector = 0; vector < 48; vector++) {
		idt_set_descriptor(vector, isr_stub_table[vector], 0x8E);
		vectors[vector] = true;
	}

	IRQ_clear_mask(0);
	IRQ_clear_mask(1);

	__asm__ volatile ("lidt %0" : : "m"(idtr)); // loading the new IDT;
	__asm__ volatile ("sti"); // enabling interrupts by setting the interrupt flag

	__asm__ volatile ("nop; nop; nop");  // Wait a bit
	print_string("Interrupts enabled\n", WHITE_ON_BLACK);
	//print_hex(idtr.base);   // Print IDT base address
	//print_hex(idtr.limit);  // Print IDT limit
}

void idt_init(void) {
    idtr.base = (u32)&idt[0];
    idtr.limit = (u16)sizeof(idt_entry_t) * IDT_MAX_DESCRIPTORS - 1;

    for (u8 vector = 0; vector < 48; vector++) {
        idt_set_descriptor(vector, isr_stub_table[vector], 0x8E);
        vectors[vector] = true;
    }

    IRQ_clear_mask(0);
    IRQ_clear_mask(1);

    __asm__ volatile ("lidt %0" : : "m"(idtr));
    
    // **DEBUG: Verify IDT was loaded**
    idtr_t verify_idtr;
    __asm__ volatile ("sidt %0" : "=m"(verify_idtr));
    
    print_string((u8 *)"IDT Base loaded: ", WHITE_ON_BLACK);
    print_hex(verify_idtr.base);
    print_char('\n', WHITE_ON_BLACK);
    
    print_string((u8 *)"IDT Limit loaded: ", WHITE_ON_BLACK);
    print_hex(verify_idtr.limit);
    print_char('\n', WHITE_ON_BLACK);
    
    print_string((u8 *)"ISR stub 33 addr: ", WHITE_ON_BLACK);
    print_hex((u32)isr_stub_table[33]);
    print_char('\n', WHITE_ON_BLACK);
    
    __asm__ volatile ("sti");
}

IDT Base loaded: 0x00108820
IDT Limit loaded: 0x000007ff
ISR stub 33 addr: 0x0010116b
32[3] IDT initialized

*/
