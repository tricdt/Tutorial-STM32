#ifndef __DHT_H
#define __DHT_H	 
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
							  
////////////////////////////////////////////////////////////////////////////////// 
u8 DHT11_Init(void);
uint8_t DHT11_Read_Data(u8 *temp,u8 *humi);

		 				    
#endif



