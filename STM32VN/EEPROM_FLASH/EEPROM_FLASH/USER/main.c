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


uint16_t ReadCnt = 0;
uint64_t Log = 0;
uint8_t ButtonValue = 0;  // Gi¨¢ tr? n¨²t nh?n sau khi l?c b?ng ph?n m?m

u8 ReadPin(u8 value)//////////chong doi
 {	 
	 
	 int i = 0;
     u8 InputValue = value;  // L?y gi¨¢ tr? n¨²t nh?n hi?n t?i
     Log = Log << 1;// D?ch tr¨¢i bi?n log ?? x¨®a gi¨¢ tr? n¨²t nh?n c? nh?t 
     if (InputValue > 0) Log |= 1;  
     if (ReadCnt < 64) ReadCnt ++;   
     else 
     {
        float Logic1Percent = 0;
        uint16_t Logic1Cnt = 0;
        for ( i = 0; i< 64; i++)    
           if (((Log >> i)&&0x01) == 1) Logic1Cnt ++;
        Logic1Percent = (Logic1Cnt*100.0 / 64.0) ;
        if (Logic1Percent > 70 ) ButtonValue = 1; else ButtonValue = 0; 
      }
		 
		 return ButtonValue;
 }


///////////////////////////////////////////////////////////////////////
int answer;

int sendAT(char* ATcmd, char* rec_answer, unsigned int timeout) {
  int x=0, answer=0; u8 c=0;
  char response[100];
  //unsigned long previous;
  memset(response, '\0', 100);      // Initialize the string
  delay_ms(100);
  while( USART_Getc(USART1) > 0) ;
  printf("%s\r\n",ATcmd);
  x = 0;
  //previous = millis();
  do {
		delay_ms(1);
		c= USART_Getc(USART1);//
    if(c){    
      response[x] = c;
      x++;
      if (strstr(response, rec_answer) != NULL)    
      {
        answer = 1; //ok
      }
    }
  }
  while((answer == 0) && ( --timeout>0 ));    
  return answer;
}


void init_SIM808(void) {
  do { answer = sendAT("AT+CREG?","+CREG: 0,1",1000);} while(answer==0);     // Connecting to the network
// config GSM
  do { answer = sendAT("ATE0","OK",1000);} while(answer==0);
  do { answer = sendAT("AT+CMGF=1", "OK",1000);} while(answer==0);          // Set SMS into text mode
  do { answer = sendAT("AT+CNMI=2,2,2,0,0", "OK",1000);} while(answer==0);  // Hien thi truc tiep noi dung tin nhan
  do { answer = sendAT("AT+CLIP=1", "OK",1000);} while(answer==0);          //Hien thi thon g tin nguoi goi den 
 
  }
 
 int main(void)
 {	
	 //u8 c=0;
	 
	 char data[2], rec [2];
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
//	 	Led7Control(PB15,PB13,PB12,1);
//	/*
//   The MAX72XX is in power-saving mode on startup,
//   we have to do a wakeup call
//   */
//  LedControl_shutdown(0,false);
//  /* Set the brightness to a medium values */
//  LedControl_setIntensity(0,15);
//  /* and clear the display */
//  LedControl_clearDisplay(0);
//	
//	delay_ms(delaytime);
//	LedControl_setChar(0,0,'a',false);
//  delay_ms(delaytime);
//  LedControl_setRow(0,0,0x05);
//  delay_ms(delaytime);
//  LedControl_setChar(0,0,'d',false);
//  delay_ms(delaytime);
//  LedControl_setRow(0,0,0x1c);
//  delay_ms(delaytime);
//  LedControl_setRow(0,0,0x10);
//  delay_ms(delaytime);
//  LedControl_setRow(0,0,0x15);
//  delay_ms(delaytime);
//  LedControl_setRow(0,0,0x1D);
//  delay_ms(delaytime);
//  LedControl_clearDisplay(0);
//  delay_ms(delaytime);
//	
//	for( int i=0;i<13;i++) {
//    LedControl_setDigit(0,7,i,false);
//    LedControl_setDigit(0,6,i+1,false);
//    LedControl_setDigit(0,5,i+2,false);
//    LedControl_setDigit(0,4,i+3,false);
//    delay_ms(delaytime);
//  }
//  LedControl_clearDisplay(0);
//  delay_ms(delaytime);
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	data[0]=7;data[1]=8;
	FLASH_Write(0x08019000, data, 2);
	delay_ms(1000);
	FLASH_Read(0x08019000, rec, 2);
	printf("%d == %d\r\n",rec[0],rec[1]);
	
	delay_ms(1000);
	data[0]=6;data[1]=5;
	FLASH_Write(0x08019001, data, 2);
	delay_ms(1000);
	FLASH_Read(0x08019001, rec, 2);
	printf("%d == %d\r\n",rec[0],rec[1]);
	 
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
