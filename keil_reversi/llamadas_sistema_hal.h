#ifndef SWI_H
#define SWI_H

#include <inttypes.h>

uint16_t __swi(1) read_IRQ_bit(void);
void __swi(0xFF) enable_irq (void);
void __swi(0xFE) disable_irq(void);
void __swi(0xFD) disable_fiq(void);

#endif // SWI_H

