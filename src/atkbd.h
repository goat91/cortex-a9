#ifndef ATKBD_INPUT_H
#define ATKBD_INPUT_H

#include <stdint.h>

#define CODE_CAPSLOCK	0x58
#define CODE_ESC	0x76
#define CODE_SHIFT_L	0x12
#define CODE_SHIFT_R	0x59
#define CODE_CTRL	0x14
#define CODE_ALT	0x11
#define CODE_ENTER	0x5a
#define CODE_EXTRA	0xe0
#define CODE_BACKSPACE	0x66
#define CODE_NUMLOCK	0x77

#define KEY_RAW		0
#define KEY_ASCII	1

void handle_kbd_event(uint8_t data);

#endif //ATKBD_INPUT_H
