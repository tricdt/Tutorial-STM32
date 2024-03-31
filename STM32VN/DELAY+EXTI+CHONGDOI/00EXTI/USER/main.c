#include "sys.h"
#include "LED.h"
#include "exti.h"
#include "delay.h"
#include "usart.h"


/************************************************
www.stm32vn.com
************************************************/

////////////////////////////////////////////////////////////

        
        uint32_t    		tick_time=0;              // The time in ms when the button Tick was set
        uint16_t    		b_pin;                  // The pin number connected to the button
        uint16_t    		over_press;				// Maximum time in ms the button can be pressed
        volatile uint8_t    modex;					// The button mode: 0 - not pressed, 1 - pressed, 2 - long pressed
        volatile uint32_t    pt = 0;						// Time in ms when the button was pressed (press time)
        const uint16_t      tick_timeout = 200;		// Period of button tick, while the button is pressed
        const uint16_t      short_press  = 900;		// If the button was pressed less that this timeout, we assume the short button press
        const uint8_t       bounce      = 50;		// Bouncing timeout (ms)
				        
        uint8_t		  ButtonStatus(void);
        void        button_EXTI(u8 ButtonPIN, uint16_t timeout_ms);//////dat ham xet nay trong thai trong ngat or while
				
/////////////////////////////////////////////////////////////////////
				

uint8_t	ButtonStatus(void) {
	if (pt > 0) {									//  pressed
		if ((millis() - pt) > short_press) {
			modex = 2;								// Long press
			pt = 0;
		}
	}
	uint8_t m = modex; modex = 0;
	return m;
}

void button_EXTI(u8 ButtonPIN, uint16_t timeout_ms) {							// Interrupt function, status changed
	uint32_t now_t = millis();
	over_press	= timeout_ms;
	b_pin=ButtonPIN;
	
	u8 keyUp = (digitalRead(ButtonPIN));//==1
	if (!keyUp) {                                  	//  pressed
		if ((pt == 0) || (now_t - pt > over_press)) pt = now_t;
	} else {										// released
		if (pt > 0) {
			if ((now_t - pt) < short_press)
				modex = 1; 							// short press
			else if ((now_t - pt) < over_press)
				modex = 2;                     		// long press
			else
				modex = 0;							// over press
			pt = 0;
		}
	}
}
				


void EXTI9_5_IRQHandler(void)
{
	 
	if(EXTI_GetITStatus(EXTI_Line5) != RESET) button_EXTI(PB5,2000);////dat ham xet nay trong thai trong ngat or while
		
	if(EXTI_GetITStatus(EXTI_Line6) != RESET) button_EXTI(PB6,2000);//printf("%d \n",modex);	
		
	 EXTI_ClearITPendingBit(EXTI_Line5|EXTI_Line6|EXTI_Line7|EXTI_Line8|EXTI_Line9);  
}

 
 int main(void)
 {	
	 
	 
	 NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 
	 
		USARTx_Init(USART2,Pins_PA2PA3,115200);	 
		LED_init();
	 
	 pinMode(PB5,INPUT_PULLUP);////////////////////////////ko dung EXTI	 
	 EXTIx_Init(GPIOB, GPIO_Pin_6, Trigger_Falling);
	 EXTIx_Init(GPIOB, GPIO_Pin_5, Trigger_Falling); 

		USART_Puts(	USART2,"LVD_IC \n" );
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
		
		int x =ButtonStatus();//lay trang thai
		
		if(PB5==b_pin&&x==1) {printf("abc1 \n");}
		else if(PB6==b_pin&&x>0) {printf("abc2 \n");} 
		
		//printf("%d\n",micros());
		delay_ms(200);
		//printf("abc \n");
		
		

		
	}
 }
