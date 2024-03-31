#include "stm32f10x.h"
#include "LED.h"
#include "delay.h"
#include "sys.h"


/************************************************
www.stm32vn.com
************************************************/


 
 int main(void)
 {	
	 //LED_init();
	 delay_init();
	 pinMode(PB5,OUTPUT);
	 pinMode(PB6,INPUT);
 
  while(1)
	{
	digitalWrite( PB5, HIGH );delay_ms(300);
	digitalWrite( PB5, LOW );delay_ms(300);
	}
 }
