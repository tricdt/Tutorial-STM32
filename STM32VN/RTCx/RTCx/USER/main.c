#include "stm32f10x.h"
#include "LED.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "adc.h"
#include "rtc.h"
/************************************************
www.stm32vn.com
************************************************/

extern u16 ADC_ConvertedValue;
 
 int main(void)
 {	

	 //LED_init();
	 delay_init();
	 pinMode(PB5,OUTPUT);
	 pinMode(PB6,INPUT);
	 RTC_Init();
	 
	RTC_Set(25,4,2020,23,42,55);   
	 delay_ms(500);
	RTC_Alarm_Set(25,4,2020,23,43,5);  
	 USARTx_Init(USART1, Pins_PA9PA10, 115200);
	 //Adc_Init(ADC1,ADC_Channel_0);
		//adcMode(uint32_t ulPin, uint8_t Mode);
  while(1)
	{
//		c=USART_Getc(USART1);
//		if(c) USART_Putc(USART1,c);
//		
//		if (USART_Gets(USART1, buffer, sizeof(buffer))) 	printf("%s\r\n",buffer);//USART_Puts(USART1,buffer);
//		
//		
	//printf("Alarm Time: %d %d-%d-%d %d:%d:%d\n",calendar.week,calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);
	//delay_ms(500);
		
//	digitalWrite( PB5, HIGH );delay_ms(300);
//	digitalWrite( PB5, LOW );delay_ms(300);
	}
 }
