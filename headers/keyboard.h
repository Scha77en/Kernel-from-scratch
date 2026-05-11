#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "types.h"
#include "screen.h"
#include "pic.h"

#define MAX_KEYB_BUFFER_SIZE	255

typedef struct {
	u8	code;
} key_event;

void	keyboard_handler(void);

#endif
