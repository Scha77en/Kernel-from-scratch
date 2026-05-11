#include "../headers/low_level.h"
#include "../headers/screen.h"

unsigned char	port_byte_in(unsigned short port) {
	// A handy C wrapper function that reads a byte from the specified port
	// "=a" ( result ) means : put AL register in variable RESULT when finished
	// "d" ( port ) means : load EDX with port
	
	unsigned char result;
	__asm__("in %%dx, %%al" : "=a" (result) : "d" (port));

	return result;
}

void	port_byte_out(unsigned short port, unsigned char data) {
	// "a" ( data ) means : load EAX with data
	// "d" ( port ) means : load EDX with port 

	__asm__("out %%al, %%dx" : :"a" (data), "d" (port));
}
unsigned short	port_word_in(unsigned short port) {
	unsigned short result;
	__asm__("in %%dx, %%ax" : "=a" (result) : "d" (port));
	return result;
}
void	port_word_out(unsigned short port, unsigned short data) {
	__asm__("out %%ax , %%dx" : :"a" (data), "d" (port));
}

void	move_cursor(u16 pos) {
	/*
	out 0x3D4, 14      ; 14 tells the framebuffer to expect the highest 8 bits of the position
	out 0x3D5, 0x00    ; sending the highest 8 bits of 0x0050
	out 0x3D4, 15      ; 15 tells the framebuffer to expect the lowest 8 bits of the position
	out 0x3D5, 0x50    ; sending the lowest 8 bits of 0x0050
	*/

	port_byte_out(REG_SCREEN_CTRL, 14);
	port_byte_out(REG_SCREEN_DATA, ((pos >> 8 & 0x00FF)));
	port_byte_out(REG_SCREEN_CTRL, 15);
	port_byte_out(REG_SCREEN_DATA, pos & 0x00FF);
	
	return ;
}

