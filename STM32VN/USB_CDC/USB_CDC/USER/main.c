#include "sys.h"
#include "LED.h"
#include "exti.h"
#include "delay.h"
#include "usart.h"

#include "CDC.h"

uint8_t packet_buffer[64], packet_size;
uint8_t flag_RX_USB = 0;
uint8_t flag_TX_USB = 0;
/************************************************
www.stm32vn.com
************************************************/

////////////////////////////////////////////////////////////

   


 
 int main(void)
 {	
	 
	 
	 NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 
	 
		USARTx_Init(USART2,Pins_PA2PA3,115200);	 
		LED_init();
	 
	 pinMode(PB5,INPUT_PULLUP);////////////////////////////ko dung EXTI	 


		USART_Puts(	USART2,"LVD_IC \n" );
	  delay_init();
	 
	 Serial_begin(115200);
	
  while(1)
	{
//	    LED0 =0;//GPIO_ResetBits(GPIOB,GPIO_Pin_5);
//	    GPIO_SetBits(GPIOE,GPIO_Pin_5);
//		delay_ms(300);
//			LED0 =1;GPIO_SetBits(GPIOB,GPIO_Pin_5);
//			GPIO_ResetBits(GPIOE,GPIO_Pin_5);
//		delay_ms(300);
		
//		if(strstr((char*)packet_buffer,"\n")!=NULL){
//			//Serial_write(Serial_read());
//			CDC_Send_DATA (&packet_buffer[10], 1);
//			memcpy(packet_buffer,"\0",sizeof(packet_buffer));
//		}
		if(Serial_available()){
			Serial_write(Serial_read());delay_ms(1);
			//Serial_Puts("abc", 3);//CDC_Send_DATA ("abc", 3);
		}

		//button_EXTI(PB5,2000);
		
//		int x =ButtonStatus();//lay trang thai
//		
//		if(PB5==b_pin&&x==1) {printf("abc1 \n");}
//		else if(PB6==b_pin&&x>0) {printf("abc2 \n");} 
		
		//printf("%d\n",micros());
		
		//printf("abc \n");
		
		

		
	}
 }
