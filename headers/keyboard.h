#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "types.h"
#include "screen.h"
#include "pic.h"
#include "signal.h"


void	keyboard_handler(void);
_bool	getascii(u8 code);

extern	_bool cmd;

#endif
