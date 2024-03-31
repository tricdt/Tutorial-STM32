#ifndef __KEYPAD_H
#define __KEYPAD_H 			   
#include "sys.h"  
//////////////////////////////////////////////////////////////////////////////////	 
#define COL0 PAout(7) 
#define COL1 PAout(6) 
#define COL2 PAout(5) 
#define COL3 PAout(4) 

#define ROW0 PAin(0) 
#define ROW1 PAin(1) 
#define ROW2 PAin(2) 
#define ROW3 PAin(3) 

void Keypad_init(void);
u8 Keypad(void);

#endif


