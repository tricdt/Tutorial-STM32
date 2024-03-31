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

u32 UART6_Rx_data0=0 ;
u32	UART6_Rx_data1=0 ;
float UART6_Rx_data2=0;
float UART6_Rx_data3=0;


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
	//strstr()
	//strtok()
	//sscanf()
	// sprintf(buffer,"abc=%d\r\n",123);//USART_Puts(USART1,buffer);
	 // printf("abc\r\n");//printf("abc: %c %s %s %d\r\n",'1',"bcd","def",12);//printf("abc=%3.2f\r\n",1.22);//printf("0x%X\r\n",255);
	////////////////////////////////////////////////////////////////////////////////////////////////////////

	 
  while(1)
	{
//		c=USART_Getc(USART1);
//		if(c) USART_Putc(USART1,c);
		
		//if (USART_Gets(USART1, buffer, sizeof(buffer))) 	printf("%s\r\n",buffer);//USART_Puts(USART1,buffer);
		
//		if (USART_Gets(USART2, buffer, sizeof(buffer))) {//'\n'
//			//if(strstr(buffer,"Input")!=NULL){//"1,2,3,4,5\n"
//			u8 a=	atoi(strtok(buffer,","));
//			if(a>0)	printf("%s\r\n",buffer);
//			//}
//		}
		
//		if (USART_Gets(USART2, buffer, sizeof(buffer))) {//'\n'
//			//if(strstr(buffer,"Input")!=NULL){//"1,2,3,4,5\n"
//			u8 a=	atoi(strtok(buffer,","));
//			strtok(NULL,",");strtok(NULL,",");strtok(NULL,",");
//			u8 b = atoi(strtok(NULL,"\n"));
//			if(b>0)	printf("%d\r\n",b);
//			//}
//		}
		
		
		if (USART_Gets(USART2, buffer, sizeof(buffer))) {//'\n'
			//if(strstr(buffer,"Input")!=NULL){//"1,2,3,4,5\n"
			u8 ready = (uint8_t)(sscanf((const char*)buffer,"Input %d %d %f %f\n" ,		//
      &UART6_Rx_data0 ,&UART6_Rx_data1 ,&UART6_Rx_data2 ,
				&UART6_Rx_data3 ) == 4);
			
			if(ready) printf("%d %d %2.2f %2.2f\r\n",UART6_Rx_data0 ,UART6_Rx_data1 ,UART6_Rx_data2 ,UART6_Rx_data3);
			//}
			
		}
		
			
		
//		if(ReadPin(PBin(5))==0) printf("abc\r\n");
//		delay_ms(10);
		
//	digitalWrite( PB5, HIGH );delay_ms(300);
//	digitalWrite( PB5, LOW );delay_ms(300);
	}
 }
