#include <low_level.h>
#include <screen.h>

void	port_byte_out(unsigned short port, unsigned char data) {
	__asm__("out %%al, %%dx" : :"a" (data), "d" (port));
}

void	move_cursor(u16 pos) {
	port_byte_out(REG_SCREEN_CTRL, 14);
	port_byte_out(REG_SCREEN_DATA, ((pos >> 8 & 0x00FF)));
	port_byte_out(REG_SCREEN_CTRL, 15);
	port_byte_out(REG_SCREEN_DATA, pos & 0x00FF);
	
	return ;
}

