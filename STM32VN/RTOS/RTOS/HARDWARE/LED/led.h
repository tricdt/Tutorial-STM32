#ifndef __LED_H
#define __LED_H	 
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
							  
////////////////////////////////////////////////////////////////////////////////// 
#define LED1 PBout(5)// PB5
#define LED0 PCout(13)// PE5	

void LED_Init(void);

		 				    
#endif
