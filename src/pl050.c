#include "pl011.h"
#include "pl050.h"
#include "interrupt.h"
#include <ctype.h>
volatile kmi_t* const kmi_kb = (kmi_t*)(KMI_KB_BASE);

char static kbdus[128] = {

//	0 1 2 3 4 5 6 7 8 9 a b c d e f	
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 0x00 - 0x0f 
	0,0,0,0,0,'q','1',0,0,0,'z','s','a','w','2',0, // 0x10 - 0x1f 
	0,'c','x','d','e','4','3',0,0,' ','v','f','t','r','5',0, // 0x20 - 0x2f 
	0,'n','b','h','g','y','6',0,0,0,'m','j','u','7','8',0, // 0x30 - 0x3f 
	0,',','k','i','o','0','9',0,0,'.','/','l',';','p','-',0, // 0x40 - 0x4f 
	0,0,'\'',0,'[','=',0,0,0,0,'\n',']',0,'\\',0,0, // 0x50 - 0x5f
	0,0,0,0,0,0,0,0,0,'1',0,'4','7',0,0,0, // 0x60 - 0x6f
	'0','.','2','5','6','8',0,0,0,'+','3','-','*','9',0,0 // 0x70 - 0x7f
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

void inline kb_handler(void){
	static int released = 0;
	uint8_t scan_keycode, keycode;
	static int shift = 0, capslock = 0, numlock = 0;

	if (kmi_kb->ir & KMIRXINTR) {
		scan_keycode = kmi_kb->data;

		if(scan_keycode & 0x80){
			if (scan_keycode == 0xf0)	// key released
				released = 1;
		}else{
			keycode = 0;
			if (released) {
				if (scan_keycode == CODE_SHIFT_L || scan_keycode == CODE_SHIFT_R)
					shift = 0;
				released = 0;
				return;
			}
			switch (scan_keycode) {
				case CODE_CAPSLOCK:
					capslock = !capslock;
					break;
				case CODE_NUMLOCK:
					numlock = !numlock;
					break;
				case CODE_SHIFT_L:
				case CODE_SHIFT_R:
					shift = 1;
					break;
				case CODE_BACKSPACE:
					break;
				case CODE_ENTER:
					break;
				default:
					keycode = kbdus[scan_keycode];
					if (!keycode)
						return;

					if (isalpha(keycode) && (capslock || shift))
						keycode = toupper(keycode);
					else if (shift) {
						char mapped = shift_map[keycode - MAP_OFFSET];
						if (mapped)
							keycode = mapped;
					}

					if (scan_keycode >= 0x69) {
						//number from numeric keyboard, need to check numlock state
						if (!numlock) {
							return;
						}
					}

					uart_putc(keycode);
					uart_putc('\n');
					break;
			}
		}
	}
}

void kb_init(void){
	kmi_kb->cr = KMICR_RXEN | KMICR_KMIEN;
	kmi_kb->clk = KMIREFCLK/8000000 - 1;
	install_isr(KMI0_INTR_IRQn, kb_handler);
	enable_irq(KMI0_INTR_IRQn);
}
