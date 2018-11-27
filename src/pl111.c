#include "pl111.h"
#include <string.h>
#include <stdio.h>

UG_GUI gui;

#define BPP	0x4

void pdraw(void *framebuffer, UG_S16 x, UG_S16 y, UG_COLOR c, int bpp){
	

	/*
	int i = 0;
	uint8 *pixel = (uint8*)&c;

	uint16_t color = ((pixel[0] >>3) & 0x1f) | (((pixel[1] >> 2) & 0x3f) <<5) | (((pixel[2] >> 3) & 0x1f) <<11);
	fb[(y*800+x) * BPP] = color & 0xff;
	fb[(y*800+x) * BPP + 1] = color >> 8;
	*/

	//printf("draw (%p, %p)\n", framebuffer, (void*)fb);
	volatile uint32 *fb = framebuffer;
	fb[y*800+x] = c;
}

void clcd_init(void){
	PL111MMIO *plio;
	int bpp = 0x4;
	int x;
	uint32 *fb;

	plio = (PL111MMIO*)PL111_IOBASE;
	plio->tim0 = 0x3f1f3cc4;
	plio->tim1 = 0x080b6257;
	plio->upbase = PL111_RAMBASE;
	plio->control = 0x192b;			// 16bpp, 5:6:5
	fb = (uint32*)plio->upbase;

	for (x = 0; x < (800 * 600); ++x)
		fb[x] = 0;

	UG_Init (&gui, pdraw, fb, 800, 600, bpp);
}
