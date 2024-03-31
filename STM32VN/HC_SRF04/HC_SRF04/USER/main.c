#include "stm32f10x.h"
#include "LED.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "adc.h"

#include <HC-SR04.h>

/************************************************
www.stm32vn.com
************************************************/

extern u16 ADC_ConvertedValue;
 
 int main(void)
 {	
	 u8 c=0;
	 u16 adcx=0;
	 float  temp=0;
	 char buffer[100];
	 //LED_init();
	 delay_init();
	 pinMode(PB5,OUTPUT);
	 pinMode(PB6,INPUT);
	 
	 USARTx_Init(USART1, Pins_PA9PA10, 115200);
	 //Adc_Init(ADC1,ADC_Channel_0);
		//adcMode(uint32_t ulPin, uint8_t Mode);
	 
	//pinMode(PB0,OUTPUT); 
	HCSR04_Init();
	
	 
  while(1)
	{
//		c=USART_Getc(USART1);
//		if(c) USART_Putc(USART1,c);
//		
//		if (USART_Gets(USART1, buffer, sizeof(buffer))) 	printf("%s\r\n",buffer);//USART_Puts(USART1,buffer);
//		
//		
		//adcx=Get_Adc_Average(ADC1,ADC_Channel_0,10);
		
		//temp=(float)ADC_ConvertedValue*(3.3/4096);
		//adcx=analogRead(PA0);
		//xuat LCD-UART
		
//		digitalWrite( PB0, LOW );delay_us(2);
//			digitalWrite( PB0, HIGH );delay_us(10);
//		digitalWrite( PB0, LOW );
			
		u16 dist = HCSR04_GetDistance();
		printf("%dmm\n",dist);
		
		delay_ms(500);
		
//	digitalWrite( PB5, HIGH );delay_ms(300);
//	digitalWrite( PB5, LOW );delay_ms(300);
	}
 }
