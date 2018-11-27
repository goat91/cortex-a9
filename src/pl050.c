#include "pl011.h"
#include "pl050.h"
#include "interrupt.h"
#include <stdio.h>
#include "cursor.h"
#include "ugui.h"
#include <stdbool.h>
#include "atkbd.h"

volatile kmi_t* const kmi_kb = (kmi_t*)(KMI_KB_BASE);
volatile kmi_t* const kmi_mouse = (kmi_t*)(KMI_MOUSE_BASE);


static void kb_intr_handler(void){
	uint8_t scan_keycode;

	if (kmi_kb->ir & KMIRXINTR) {
		scan_keycode = kmi_kb->data;
		handle_kbd_event(scan_keycode);
	}
}

static void mouse_intr_handler(void)
{
	int count = 0;
	uint8_t data_buf[3];
	int32_t x, y;
	static bool mouse_pressed = false;

	while (kmi_mouse->ir & KMIRXINTR) {
		uint8_t data = kmi_mouse->data;
		if (data != 0xfa && count < 3)
			data_buf[count++] = data;	
	}

	if (count != 3)
		return;

	if (data_buf[0] & 0x80)
		printf("y overflow\n");

	if (data_buf[0] & 0x40)
		printf("x overflow\n");

	if (data_buf[0] & 0x10)
		x = (int8_t)data_buf[1];
	else
		x = (uint8_t)data_buf[1];

	if (data_buf[0] & 0x20)
		y = (int8_t)data_buf[2];
	else
		y = (uint8_t)data_buf[2];

	if (data_buf[0] & 0x01) {
		if (!mouse_pressed) {
			mouse_pressed = true;
			UG_CursorPressed();
		}
	}
	else {
		if (mouse_pressed) {
			mouse_pressed = false;
			UG_CursorReleased();
		}
	}

	UG_CursorMove(x, 0 - y);
}

void kmi0_init(void){
	kmi_kb->cr = KMICR_RXEN | KMICR_KMIEN;
	kmi_kb->clk = KMIREFCLK/8000000 - 1;
	install_isr(KMI0_INTR_IRQn, kb_intr_handler);
	enable_irq(KMI0_INTR_IRQn);
}

void kmi1_init(void) {
	kmi_mouse->cr = KMICR_RXEN | KMICR_KMIEN;
	kmi_mouse->clk = KMIREFCLK/8000000 - 1;
	install_isr(KMI1_INTR_IRQn, mouse_intr_handler);

	// GET ID
	printf("Get Mouse ID\n");
	kmi_mouse->data = 0xf2;
	while (kmi_mouse->ir & KMIRXINTR) {
		printf("data: 0x%lx\n", kmi_mouse->data);
	}

	// Reset
	printf("Reset mouse\n");
	kmi_mouse->data = 0xff;
	while (kmi_mouse->ir & KMIRXINTR) {
		printf("data: 0x%lx\n", kmi_mouse->data);
	}


	// Set Rate
	printf("Set Rate\n");
	kmi_mouse->data = 0xf3;
	while (!(kmi_mouse->ir & KMIRXINTR));
	while (kmi_mouse->ir & KMIRXINTR) {
		printf("data: 0x%lx\n", kmi_mouse->data);
	}

	kmi_mouse->data = 200;
	while (!(kmi_mouse->ir & KMIRXINTR));
	while (kmi_mouse->ir & KMIRXINTR) {
		printf("data: 0x%lx\n", kmi_mouse->data);
	}

	// Set Resolution
	printf("Set Resolution\n");
	kmi_mouse->data = 0xe8;
	while (!(kmi_mouse->ir & KMIRXINTR));
	while (kmi_mouse->ir & KMIRXINTR) {
		printf("data: 0x%lx\n", kmi_mouse->data);
	}

	kmi_mouse->data = 2;
	while (!(kmi_mouse->ir & KMIRXINTR));
	while (kmi_mouse->ir & KMIRXINTR) {
		printf("data: 0x%lx\n", kmi_mouse->data);
	}


	// Set Scale
	printf("Set Scale\n");
	kmi_mouse->data = 0xe6;
	while (!(kmi_mouse->ir & KMIRXINTR));
	while (kmi_mouse->ir & KMIRXINTR) {
		printf("data: 0x%lx\n", kmi_mouse->data);
	}


	// Enable Reporting
	printf("Enable Reporting\n");
	kmi_mouse->data = 0xf4;
	while (kmi_mouse->ir & KMIRXINTR) {
		printf("data: 0x%lx\n", kmi_mouse->data);
	}

	enable_irq(KMI1_INTR_IRQn);
}
