#include "sys.h"
#include "LED.h"
#include "exti.h"
#include "delay.h"
#include "usart.h"


#include "RCSwitch.h"

/************************************************
www.stm32vn.com
************************************************/

////////////////////////////////////////////////////////////


 
 int main(void)
 {	
	 
	 
	 NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 
	 
		USARTx_Init(USART1,Pins_PA9PA10,115200);	 
		LED_init();
	 
	 RCSwitch();
	 RCSwitch_enableReceive(PB14);

		USART_Puts(	USART1,"LVD_IC \n" );
	  delay_init();
	 
	
  while(1)
	{
//	    LED0 =0;//GPIO_ResetBits(GPIOB,GPIO_Pin_5);
//	    GPIO_SetBits(GPIOE,GPIO_Pin_5);
//		delay_ms(300);
//			LED0 =1;GPIO_SetBits(GPIOB,GPIO_Pin_5);
//			GPIO_ResetBits(GPIOE,GPIO_Pin_5);
//		delay_ms(300);
		
		//button_EXTI(PB5,2000);
		

		if (RCSwitch_available()) {
    
				printf("Received :%ld  ", RCSwitch_getReceivedValue() );
				printf(" / ");
				printf("%d ", RCSwitch_getReceivedBitlength() );
				printf("bits ");
				printf("Protocol: ");
				printf("%d  \n",RCSwitch_getReceivedProtocol() );

     RCSwitch_resetAvailable();
			}
		

		
	}
 }
