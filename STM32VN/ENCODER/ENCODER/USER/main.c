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

void ENCODER_Init() {
	//Enable Clock();
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//////////////////////////////////////////////////////
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	TIM_DeInit(TIM1);
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period = 0xFFFF;
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStruct);


	TIM_ICInitTypeDef TIM_ICInitStruct;
	TIM_ICInitStruct.TIM_Channel = TIM_Channel_1;
	TIM_ICInitStruct.TIM_ICFilter = 0;
	TIM_ICInit(TIM1, &TIM_ICInitStruct);//TIM_PWMIConfig
	TIM_ICInitStruct.TIM_Channel = TIM_Channel_2;
	TIM_ICInitStruct.TIM_ICFilter = 0;
	TIM_ICInit(TIM1, &TIM_ICInitStruct);//TIM_PWMIConfig
	
	TIM_EncoderInterfaceConfig(TIM1, TIM_EncoderMode_TI12, TIM_ICPolarity_BothEdge, TIM_ICPolarity_BothEdge);
	
	TIM_Cmd(TIM1, ENABLE);
	
	TIM1->CNT=0;
}


 
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
	 
	 USARTx_Init(USART2, Pins_PA2PA3, 115200);
	 //Adc_Init(ADC1,ADC_Channel_0);
		//adcMode(uint32_t ulPin, uint8_t Mode);
	 
	//pinMode(PB0,OUTPUT); 
	//HCSR04_Init();
	ENCODER_Init();
	 
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
			
//		u16 dist = HCSR04_GetDistance();
		
		int dir = ((TIM1->CR1&TIM_CR1_DIR)? -1 : 1);
		printf("%d..+..%d\n",TIM1->CNT,dir);
		
		delay_ms(100);
		
//	digitalWrite( PB5, HIGH );delay_ms(300);
//	digitalWrite( PB5, LOW );delay_ms(300);
	}
 }
