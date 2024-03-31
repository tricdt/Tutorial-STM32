#include "stm32f10x.h"
#include "LED.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "stm32f1_rc522.h"
#include "keypad.h"

//#include "Led7Control.h"
/************************************************
www.stm32vn.com
************************************************/
void EnterPassWord(void);

///////////////////////////////////////////////////////////////////////
uchar i,tmp;
uchar status;
uchar str[MAX_LEN]; // Max_LEN = 16

uchar serNum[5];
char password[16]="123456"; //Max lenght of password is 16 charaters
char keypass[16];
int cnt=0;
uint8_t key;
uchar check,check2;

uchar Key_Card[5]  = {0x16, 0xe7, 0x23, 0xa3, 0x71};
uchar Key_Card2[5] = {0xc4, 0x24, 0x99, 0x22, 0x5b};
 
 int main(void)
 {	
	// u8 key=0;

//	 u16 delaytime=1800;
//	 char buffer[100];
	 LED_init();
	 delay_init();
	 //pinMode(PB5,INPUT_PULLUP);
//	 pinMode(PB6,INPUT);
//	 
	 USARTx_Init(USART2, Pins_PA2PA3, 115200);
	 
	  MFRC522_Init();	
		Keypad_init();
//	 do { answer = sendAT("AT","OK",1000);} while(answer==0);
//	
//	 init_SIM808();
////////////////////////////////////////////////////////////////////////////////////////////////////////////////	 
	LED0=1;
	////////////////////////////////////////////////////////////////////////////////////////////////////////

	 
  while(1)
	{
//		c=USART_Getc(USART1);
//		if(c) USART_Putc(USART1,c);
		
		
		
		//if (USART_Gets(USART1, buffer, sizeof(buffer))) 	printf("%s\r\n",buffer);//USART_Puts(USART1,buffer);
		
//		if(ReadPin(PBin(5))==0) printf("abc\r\n");
//		delay_ms(10);
		
//		key=Keypad();
//		if(key!=0xFF){
//			if(key=='*')
//				printf("abc\r\n");
//		}
		
		

		printf("Waiting your Card! \r\n");
				
		status = MFRC522_Request(PICC_REQIDL, str);	
		if (status == MI_OK)
		{
			printf("Find out a card: %x, %x\r\n",str[0],str[1]);
		}

		//Chong va cham the, tra ve 5 byte ma the
		status = MFRC522_Anticoll(str);
		memcpy(serNum, str, 5);
		if (status == MI_OK)
		{
			printf("Your card's number are: %x, %x, %x, %x, %x \r\n",serNum[0], serNum[1], serNum[2], serNum[3],serNum[4]);
			for(i=0;i<5;i++)
			{
				if(Key_Card[i]!=serNum[i]) check = 0;
				else check = 1;
				if(Key_Card2[i]!=serNum[i]) check2 = 0;
				else check2 = 1;
			}
		}
		
		key = Keypad();
		if(key != 0xFF)
		{
			printf("Key: %c \r\n",key);
			delay_ms(25);
			if(key == '#')
			{
				EnterPassWord();
			}
			while(key!= 0xFF) {key = Keypad();}
		}		
	
		if(check == 1||check2 ==1)
		{
			check = 0;
			check2 = 0;
			LED0=!LED0;
			printf("The Card's number is Ok!\r\n");
			delay_ms(1000);
		}
		delay_ms(50);	
	
		
//	digitalWrite( PB5, HIGH );delay_ms(300);
//	digitalWrite( PB5, LOW );delay_ms(300);
	}
 }
 
 
 uint8_t CheckPass(char *Str1, char *Str2)
{
	if(strlen(Str1)==strlen(Str2))
	{
		if(strstr(Str1,Str2)) return 1;
		else return 0;
	}
	else return 0;
}

 void EnterPassWord(void)
{
	memcpy(keypass," ", 16);
	delay_ms(25);
	do
	{
		key = Keypad();
		if(key != 0XFF && key!='*'&& key!='#')
		{
			delay_ms(25);
			keypass[cnt++] = key; 
			printf("%c\r",key);
			while(key!= 0xFF) {key = Keypad();}
			delay_ms(10);
		}
	}	while(key!='*'); //while dk dung
	while(key!= 0xFF) {key = Keypad();}
	delay_ms(10);
	
	printf("%s\n\n",keypass);
	cnt=0;	
	if(CheckPass(password,keypass)== 1) 
	{ 
		printf("Password is Right! \n\n"); 
		LED0=!LED0;
	}
}
