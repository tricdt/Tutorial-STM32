#include "stm32f10x.h"
#include "LED.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "Led7Control.h"
/************************************************
www.stm32vn.com
************************************************/


///////////////////////////////////////////////////////////////////////

 
 int main(void)
 {	
	 //u8 c=0;
	 u16 delaytime=1800;
	 char buffer[100];
	 //LED_init();
	 delay_init();
	 //pinMode(PB5,INPUT_PULLUP);
//	 pinMode(PB6,INPUT);
//	 
	 USARTx_Init(USART2, Pins_PA2PA3, 115200);
//	 do { answer = sendAT("AT","OK",1000);} while(answer==0);
//	
//	 init_SIM808();
////////////////////////////////////////////////////////////////////////////////////////////////////////////////	 
	 	Led7Control(PB15,PB13,PB12,1);
	/*
   The MAX72XX is in power-saving mode on startup,
   we have to do a wakeup call
   */
  LedControl_shutdown(0,false);
  /* Set the brightness to a medium values */
  LedControl_setIntensity(0,15);
  /* and clear the display */
  LedControl_clearDisplay(0);
	
	delay_ms(delaytime);
	LedControl_setChar(0,0,'a',false);
  delay_ms(delaytime);
  LedControl_setRow(0,0,0x05);
  delay_ms(delaytime);
  LedControl_setChar(0,0,'d',false);
  delay_ms(delaytime);
  LedControl_setRow(0,0,0x1c);
  delay_ms(delaytime);
  LedControl_setRow(0,0,0x10);
  delay_ms(delaytime);
  LedControl_setRow(0,0,0x15);
  delay_ms(delaytime);
  LedControl_setRow(0,0,0x1D);
  delay_ms(delaytime);
  LedControl_clearDisplay(0);
  delay_ms(delaytime);
	
	for( int i=0;i<13;i++) {
    LedControl_setDigit(0,7,i,false);
    LedControl_setDigit(0,6,i+1,false);
    LedControl_setDigit(0,5,i+2,false);
    LedControl_setDigit(0,4,i+3,false);
    delay_ms(delaytime);
  }
  LedControl_clearDisplay(0);
  delay_ms(delaytime);
	////////////////////////////////////////////////////////////////////////////////////////////////////////

	 
  while(1)
	{
//		c=USART_Getc(USART1);
//		if(c) USART_Putc(USART1,c);
		
		//if (USART_Gets(USART1, buffer, sizeof(buffer))) 	printf("%s\r\n",buffer);//USART_Puts(USART1,buffer);
		
//		if(ReadPin(PBin(5))==0) printf("abc\r\n");
//		delay_ms(10);
		
//	digitalWrite( PB5, HIGH );delay_ms(300);
//	digitalWrite( PB5, LOW );delay_ms(300);
	}
 }
