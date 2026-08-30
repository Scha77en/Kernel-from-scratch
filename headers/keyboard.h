#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <types.h>
#include <screen.h>
#include <pic.h>
#include <signal.h>


void	keyboard_handler(void);
_bool	getascii(u8 code);
char	get_key(void);

typedef enum {
    QWERTY = 0,
    AZERTY = 1
} kbd_layout_t;


extern	kbd_layout_t	current_layout;
extern	_bool cmd;

#endif
