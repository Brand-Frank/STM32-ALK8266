#ifndef __LED_H
#define __LED_H	 
#include "sys.h"

#define LED0 PCout(1)// PC1
#define LED1 PCout(3)// PC3	

void LED_Init(void);
void LED_set(u8 led_no, u8 on);

#endif
