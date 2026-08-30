#ifndef LOW_LEVEL_H
#define LOW_LEVEL_H

#include <types.h>

void port_byte_out(unsigned short port, unsigned char data);

void	move_cursor(u16 pos);

#endif

