#include "atkbd.h"
#include <ctype.h>

char static ascii_map[128] = {

//	0 1 2 3 4 5 6 7 8 9 a b c d e f	
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 0x00 - 0x0f 
	0,0,0,0,0,'q','1',0,0,0,'z','s','a','w','2',0, // 0x10 - 0x1f 
	0,'c','x','d','e','4','3',0,0,' ','v','f','t','r','5',0, // 0x20 - 0x2f 
	0,'n','b','h','g','y','6',0,0,0,'m','j','u','7','8',0, // 0x30 - 0x3f 
	0,',','k','i','o','0','9',0,0,'.','/','l',';','p','-',0, // 0x40 - 0x4f 
	0,0,'\'',0,'[','=',0,0,0,0,'\n',']',0,'\\',0,0, // 0x50 - 0x5f
	0,0,0,0,0,0,8,0,0,'1',0,'4','7',0,0,0, // 0x60 - 0x6f
	'0','.','2','5','6','8',27,0,0,'+','3','-','*','9',0,0 // 0x70 - 0x7f
};

#define MAP_OFFSET	30
char static shift_map[]={
	0, 0, 0, 0, 0, 0, 0, 0, 0, '"',								//30 - 39
	0, 0, 0, 0, '<', '_', '>', '?', ')', '!', 					//40 - 49
	'@', '#', '$', '%', '^', '&', '*', '(', 0, ':',				//50 - 59
	0, '+', 0, 0, 0, 0, 0, 0, 0, 0, 							//60 - 69
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,								//70 - 79
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,								//80 - 89
	0, '{', '|', '}', 0, 0, 0, 0, 0, 0, 						//90 - 99
};

static int shift = 0, capslock = 0, numlock = 0, released = 0;
static char scancode_to_ascii(uint8_t keycode, int capslock, int shift, int numlock)
{
	char ascii = 0;
	
	ascii = ascii_map[keycode];
	if (!ascii)
		return 0;

	if (isalpha(ascii)) {
		if (capslock ^ shift)
			ascii = toupper(ascii);
	} else if (shift) {
		ascii = shift_map[ascii - MAP_OFFSET];
		if (!ascii)
			return 0;
	}

	if (keycode >= 0x69) {
		if (!numlock)
			return 0;
	}

	return ascii;
}

#define ATKBD_RET_RELEASE 0xf0

void handle_kbd_event(uint8_t data)
{
	uint8_t code = data, keycode;

	switch (code) {
	case ATKBD_RET_RELEASE:
		released = 1;
		return;
	case CODE_CAPSLOCK:
		capslock = !capslock;
		break;
	case CODE_NUMLOCK:
		numlock = !numlock;
		break;
	case CODE_SHIFT_L:
	case CODE_SHIFT_R:
		shift = !released;
		break;
	case CODE_BACKSPACE:
		if (!released)
			UG_InputUpdate(KEY_RAW, code);
		break;
	default:
		keycode = scancode_to_ascii(code,
				capslock, shift, numlock);
		if (keycode) {
			//printf("key '%c' %s\n", keycode, released?"released":"pressed");

			//input_report_key(KEY_ASCII, keycode);
			if (released)
				UG_InputUpdate(KEY_ASCII, keycode);
		}
	}

	if (released)
		released = 0;
}
