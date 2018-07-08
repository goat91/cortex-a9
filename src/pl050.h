#ifndef __PL050_H__
#define __PL050_H__

#include "platform.h"

#define KMITXINTR	(1 << 1)
#define KMIRXINTR	(1 << 0)

#define KMICR_KMIEN	(1 << 2)
#define KMICR_TXEN	(1 << 3)
#define KMICR_RXEN	(1 << 4)

#define KMIREFCLK	0x24000000
#define KMISTAT_RXFULL	(1 << 4)

typedef struct {
	uint32_t cr;
	uint32_t status;
	uint32_t data;
	uint32_t clk;
	uint32_t ir;
	
} kmi_t;

void kb_handler(void);
void kb_init(void);

#endif
