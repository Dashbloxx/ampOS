#pragma once

void pic_initialize();
void pic_sendeoi(int irq);
void pic_setmask(int irq);
void pic_clearmask(int irq);