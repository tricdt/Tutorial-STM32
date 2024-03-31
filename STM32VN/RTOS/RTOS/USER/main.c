#include "sys.h"
#include "LED.h"
#include "delay.h"
#include "usart.h"

#include "includes.h"

#include "myiic.h"
#include "DS1307.h"

/************************************************
www.stm32vn.com
************************************************/

/*********************Van de hàm printf voi data kieu float***************************

KEIL:
__align(8) static OS_STK  TaskEquipmentStk[TASK_EQUIPMENT_STK_SIZE];            

IAR:
#pragma data_alignment=8
OS_STK Task1_LED1_Stk[Task1_LED1_Stk_Size];
————————————————

************************************************/

/////////////////////////OS///////////////////////////////////

#define START_TASK_PRIO      			10 
#define START_STK_SIZE  				64
OS_STK START_TASK_STK[START_STK_SIZE];

void start_task(void *pdata);	
 			   

#define LED0_TASK_PRIO       			7 
#define LED0_STK_SIZE  		    		64
OS_STK LED0_TASK_STK[LED0_STK_SIZE];

void led0_task(void *pdata);



#define LED1_TASK_PRIO       			6 
#define LED1_STK_SIZE  					64
OS_STK LED1_TASK_STK[LED1_STK_SIZE];

void led1_task(void *pdata);


#define SHOW_TASK_PRIO    			5 
#define SHOW_STK_SIZE  		 		128
OS_STK SHOW_TASK_STK[SHOW_STK_SIZE];

void show_task(void *pdata);


#define MAIN_TASK_PRIO       			4 
#define MAIN_STK_SIZE  					256
__align(8) static OS_STK MAIN_TASK_STK[MAIN_STK_SIZE];//////////////////// 

void main_task(void *pdata);

////////////////////////////////////////////////////////////
OS_EVENT * msg_key;	
 

 
 int main(void)
 {	
	 
	 NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	 delay_init();	
	 
	 LED_Init();
	 USARTx_Init(USART1, Pins_PA9PA10, 115200);	 
	 DS1307_Init();
	 
	OSInit();   
 	OSTaskCreate(start_task,(void *)0,(OS_STK *)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO );
	OSStart();
	 
	
  
 }

 
 void start_task(void *pdata){
	 
	 OS_CPU_SR cpu_sr=0;
	  pdata = pdata; 
	 
	 msg_key=OSMboxCreate((void*)0);	
	 OS_ENTER_CRITICAL();//	
	 
	 OSTaskCreate(led0_task,(void *)0,(OS_STK *)&LED0_TASK_STK[LED0_STK_SIZE-1],LED0_TASK_PRIO );
	 OSTaskCreate(led1_task,(void *)0,(OS_STK *)&LED1_TASK_STK[LED1_STK_SIZE-1],LED1_TASK_PRIO );
	 OSTaskCreate(show_task,(void *)0,(OS_STK*)&SHOW_TASK_STK[SHOW_STK_SIZE-1],SHOW_TASK_PRIO);
	 OSTaskCreate(main_task,(void *)0,(OS_STK*)&MAIN_TASK_STK[MAIN_STK_SIZE-1],MAIN_TASK_PRIO);
	 
	 
	 OSTaskSuspend(START_TASK_PRIO);	
	 OS_EXIT_CRITICAL();//
	 
 }
 
 void led0_task(void *pdata)
{	  
	char buff[20];
	while(1)
	{
		LED0=0;
		sprintf(buff,"%ld",LED0);
		OSMboxPost(msg_key,(void*)buff);
		delay_ms(130);
		LED0=1;
		sprintf(buff,"%s","abc");
		OSMboxPost(msg_key,(void*)buff);
		delay_ms(50);
	};
}

void led1_task(void *pdata)
{	  
	while(1)
	{
		LED1=0;
		delay_ms(1300);
		LED1=1;
		delay_ms(5000);
	};
}
void main_task(void *pdata)
{	

	u8 err;
	void *key=0;
	
	char buffer[100];
	
	u32 UART6_Rx_data0=0 ;
	u32	UART6_Rx_data1=0 ;
	float UART6_Rx_data2=0;
	float UART6_Rx_data3=0;
	
 	while(1)
	{		
		
		key = OSMboxPend(msg_key,10,&err); //printf("%s\r\n",key);
		
		if (USART_Gets(USART1, buffer, sizeof(buffer))) {//'\n'
			//if(strstr(buffer,"Input")!=NULL){//"1,2,3,4,5\n"
			u8 ready = (uint8_t)(sscanf((const char*)buffer,"Input %d %d %f %f\n" ,		//
																																							&UART6_Rx_data0 ,
																																							&UART6_Rx_data1 ,
																																							&UART6_Rx_data2 ,
																																							&UART6_Rx_data3 ) == 4);
			
		if(ready) printf("%d %d %2.2f %2.2f\r\n",UART6_Rx_data0 ,UART6_Rx_data1 ,UART6_Rx_data2 ,UART6_Rx_data3);
					
		}
		
		//printf("%d\r\n",micros());
		delay_ms(100);
	}
}	

void show_task(void *pdata)
{

	u8 err;
	u8 gio=0,phut=0,giay=0,thu=0,ngay=0,thang=0,nam=0, sec=0;
	
	while(1)
	{
		 RTC_GetDateTime(&gio,&phut,&giay,&thu,&ngay,&thang,&nam);
		/* Toggle GREEN led if needed */
		if (giay != sec) {
			sec = giay;
			/* Display on LCD,UART, ... */
			printf("%d-%d-%d %d-%d-%d-%d\n",gio, phut, giay, thu, ngay, thang, nam);
			/* Toggle GREEN LED */
			LED0=!LED0;
			//printf("%X\n",I2C_Read(I2C1, 0xD0, 0x00));//giay
		}
		
		//printf("%d\r\n",micros());
		delay_ms(500);	 
	}									 
}



