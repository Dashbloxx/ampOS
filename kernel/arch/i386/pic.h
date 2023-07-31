#pragma once

#define IRQ_TO_INTVEC(irq) ((irq) + 0x20)

void pic_initialize();
void pic_sendeoi(int irq);
void pic_setmask(int irq);
void pic_clearmask(int irq);