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


#include "WS2812B.h"
// Note. Gamma is not really supported in the library, its only included as some functions used in this example require Gamma
const uint8_t LEDGamma[] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
    2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
    5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
   10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
   17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
   25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
   37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
   51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
   69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
   90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
  115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
  144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
  177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255 };
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

void colorWipe(uint32_t c, uint8_t wait) 
{
  for(uint16_t i=0; i<WS2812B_numPixels(); i++) 
  {
      WS2812B_setPixelColorX(i, c);
      WS2812B_show();
      delay(wait);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(u8 WheelPos) 
{
  if(WheelPos < 85) 
  {
    return WS2812B_Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } 
  else 
  {
    if(WheelPos < 170) 
    {
     WheelPos -= 85;
     return WS2812B_Color(255 - WheelPos * 3, 0, WheelPos * 3);
    } 
    else 
    {
     WheelPos -= 170;
     return WS2812B_Color(0, WheelPos * 3, 255 - WheelPos * 3);
    }
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<WS2812B_numPixels(); i++) 
  {
      WS2812B_setPixelColorX(i, Wheel((i+j) & 255));
    }
    WS2812B_show();
    delay(wait);
  }
}



// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) 
{
  uint16_t i, j;

  for(j=0; j<256*5; j++) 
  { // 5 cycles of all colors on wheel
    for(i=0; i< WS2812B_numPixels(); i++) 
  {
      WS2812B_setPixelColorX(i, Wheel(((i * 256 / WS2812B_numPixels()) + j) & 255));
    }
    WS2812B_show();
    delay(wait);
  }
}



//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < WS2812B_numPixels(); i=i+3) {
        WS2812B_setPixelColorX(i+q, c);    //turn every third pixel on
      }
      WS2812B_show();

      delay(wait);

      for (uint16_t i=0; i < WS2812B_numPixels(); i=i+3) {
        WS2812B_setPixelColorX(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < WS2812B_numPixels(); i=i+3) {
        WS2812B_setPixelColorX(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      WS2812B_show();

      delay(wait);

      for (uint16_t i=0; i < WS2812B_numPixels(); i=i+3) {
        WS2812B_setPixelColorX(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

void pulseWhite(uint8_t wait) {
  for(int j = 0; j < 256 ; j++){
      for(uint16_t i=0; i<WS2812B_numPixels(); i++) {
          WS2812B_setPixelColorX(i, WS2812B_Color(LEDGamma[j],LEDGamma[j],LEDGamma[j] ) );
        }
        delay(wait);
        WS2812B_show();
      }

  for(int j = 255; j >= 0 ; j--){
      for(uint16_t i=0; i<WS2812B_numPixels(); i++) {
          WS2812B_setPixelColorX(i, WS2812B_Color(LEDGamma[j],LEDGamma[j],LEDGamma[j] ) );
        }
        delay(wait);
        WS2812B_show();
      }
}

uint8_t red(uint32_t c) {
  return (c >> 16);
}
uint8_t green(uint32_t c) {
  return (c >> 8);
}
uint8_t blue(uint32_t c) {
  return (c);
}

void rainbowFade2White(uint8_t wait, int rainbowLoops, int whiteLoops) {
  float fadeMax = 100.0;
  int fadeVal = 0;
  uint32_t wheelVal;
  int redVal, greenVal, blueVal;

  for(int k = 0 ; k < rainbowLoops ; k ++){
    
    for(int j=0; j<256; j++) { // 5 cycles of all colors on wheel

      for(int i=0; i< WS2812B_numPixels(); i++) {

        wheelVal = Wheel(((i * 256 / WS2812B_numPixels()) + j) & 255);

        redVal = red(wheelVal) * (float)(fadeVal/fadeMax);
        greenVal = green(wheelVal) * (float)(fadeVal/fadeMax);
        blueVal = blue(wheelVal) * (float)(fadeVal/fadeMax);

        WS2812B_setPixelColorX( i, WS2812B_Color( redVal, greenVal, blueVal ) );

      }

      //First loop, fade in!
      if(k == 0 && fadeVal < fadeMax-1) {
          fadeVal++;
      }

      //Last loop, fade out!
      else if(k == rainbowLoops - 1 && j > 255 - fadeMax ){
          fadeVal--;
      }

        WS2812B_show();
        delay(wait);
    }
  
  }



  delay(500);


  for(int k = 0 ; k < whiteLoops ; k ++){

    for(int j = 0; j < 256 ; j++){

        for(uint16_t i=0; i < WS2812B_numPixels(); i++) {
            WS2812B_setPixelColorX(i, WS2812B_Color(LEDGamma[j],LEDGamma[j],LEDGamma[j] ) );
          }
          WS2812B_show();
          delay(wait);
        }

        delay(2000);
    for(int j = 255; j >= 0 ; j--){

        for(uint16_t i=0; i < WS2812B_numPixels(); i++) {
            WS2812B_setPixelColorX(i, WS2812B_Color(LEDGamma[j],LEDGamma[j],LEDGamma[j] ) );
          }
          WS2812B_show();
         delay(wait);
        }
  }

  delay(500);
}

void whiteOverRainbow(uint8_t wait, uint8_t whiteSpeed, uint8_t whiteLength ) {
  
  if(whiteLength >= WS2812B_numPixels()) whiteLength = WS2812B_numPixels() - 1;

  int head = whiteLength - 1;
  int tail = 0;

  int loops = 3;
  int loopNum = 0;

  static unsigned long lastTime = 0;


  while(true){
    for(int j=0; j<256; j++) {
      for(uint16_t i=0; i<WS2812B_numPixels(); i++) {
        if((i >= tail && i <= head) || (tail > head && i >= tail) || (tail > head && i <= head) ){
          WS2812B_setPixelColorX(i, WS2812B_Color(255,255,255 ) );
        }
        else{
          WS2812B_setPixelColorX(i, Wheel(((i * 256 / WS2812B_numPixels()) + j) & 255));
        }
        
      }

      if(millis() - lastTime > whiteSpeed) {
        head++;
        tail++;
        if(head == WS2812B_numPixels()){
          loopNum++;
        }
        lastTime = millis();
      }

      if(loopNum == loops) return;
    
      head%=WS2812B_numPixels();
      tail%=WS2812B_numPixels();
        WS2812B_show();
        delay(wait);
    }
  }
  
}
void fullWhite() {
  
    for(uint16_t i=0; i<WS2812B_numPixels(); i++) {
        WS2812B_setPixelColorX(i, WS2812B_Color(255, 255, 255 ) );
    }
      WS2812B_show();
}
 
 int main(void)
 {	
	 u16 i=0;
	 	 char buff[100];
	 /* Create USART working buffer */
	char USART_Buffer[100] = "Hello via USART with TX DMA\n";
	 
	char RxBuffer[300];
	 
	 // u8 key=0;

//	 u16 delaytime=1800;
//	 char buffer[100];
	 LED_init();
	 delay_init();
	 //pinMode(PB5,INPUT_PULLUP);
//	 pinMode(PB6,INPUT);
//	 
	 USARTx_Init(USART2, Pins_PA2PA3, 115200);
	 USARTx_Init_DMA(USART2);// ok
	 UART_Receive_DMA(USART2,RxBuffer,sizeof(RxBuffer));	 
	 
//	  MFRC522_Init();	
//		Keypad_init();
	 
	 WS2812B_begin();
		 
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
		
		
		 /////////////DMA-RX//////////////// 

	 
	//khong nen dung	chung DMA-RX/* Send data with DMA */
	//USART_DMA_Send(USART1, (uint8_t *)USART_Buffer, strlen(USART_Buffer));
	//USART_DMA_Puts(USART1, USART_Buffer);
				if (strstr((char*)RxBuffer, "\n") != NULL) 
						{ printf("%s\r\n",RxBuffer); for(i=0;i<sizeof(RxBuffer);i++)RxBuffer[i]=0;UART_Receive_DMA(USART2,RxBuffer,sizeof(RxBuffer));}//USART_DMA_Send(USART2, USART_Buffer, strlen(USART_Buffer));
			
					delay(1000);
	
	
//	colorWipe(WS2812B_Color(0, 255, 0), 20); // Green
//	delay(500);
//  colorWipe(WS2812B_Color(255, 0, 0), 20); // Red
//	delay(500);
//  colorWipe(WS2812B_Color(0, 0, 255), 20); // Blue
//	delay(500);
//  rainbow(10);
//  rainbowCycle(10);
//  theaterChase(WS2812B_Color(255, 0, 0), 20); // Red
//  theaterChase(WS2812B_Color(0, 255, 0), 20); // Green
//  theaterChase(WS2812B_Color(0, 0, 255), 20); // Blue
//  theaterChaseRainbow(10);
//  whiteOverRainbow(20,75,5);  
//  pulseWhite(5); 
//  delay(250);
//  fullWhite();
//  delay(250);
//  rainbowFade2White(3,3,1);

//		printf("Waiting your Card! \r\n");
//				
//		status = MFRC522_Request(PICC_REQIDL, str);	
//		if (status == MI_OK)
//		{
//			printf("Find out a card: %x, %x\r\n",str[0],str[1]);
//		}

//		//Chong va cham the, tra ve 5 byte ma the
//		status = MFRC522_Anticoll(str);
//		memcpy(serNum, str, 5);
//		if (status == MI_OK)
//		{
//			printf("Your card's number are: %x, %x, %x, %x, %x \r\n",serNum[0], serNum[1], serNum[2], serNum[3],serNum[4]);
//			for(i=0;i<5;i++)
//			{
//				if(Key_Card[i]!=serNum[i]) check = 0;
//				else check = 1;
//				if(Key_Card2[i]!=serNum[i]) check2 = 0;
//				else check2 = 1;
//			}
//		}
//		
//		key = Keypad();
//		if(key != 0xFF)
//		{
//			printf("Key: %c \r\n",key);
//			delay_ms(25);
//			if(key == '#')
//			{
//				EnterPassWord();
//			}
//			while(key!= 0xFF) {key = Keypad();}
//		}		
//	
//		if(check == 1||check2 ==1)
//		{
//			check = 0;
//			check2 = 0;
//			LED0=!LED0;
//			printf("The Card's number is Ok!\r\n");
//			delay_ms(1000);
//		}
//		delay_ms(50);	
	
		
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
