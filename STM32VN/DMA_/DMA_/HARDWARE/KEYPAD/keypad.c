#include "keypad.h" 

void Keypad_init(){
	GPIO_Set(GPIOA,GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7,GPIO_Mode_Out_OD,GPIO_Speed_50MHz);//COL
	
	GPIO_Set(GPIOA,GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3,GPIO_Mode_IPU,GPIO_Speed_50MHz);//ROW
}

u8 Keypad(){
	COL0=0;COL1=1;COL2=1;COL3=1;
	delay_ms(1);
	if(!ROW0) return '1';
	if(!ROW1) return '2';
	if(!ROW2) return '3';
	if(!ROW3) return 'A';
	
	COL0=1;COL1=0;COL2=1;COL3=1;
	delay_ms(1);
	if(!ROW0) return '4';
	if(!ROW1) return '5';
	if(!ROW2) return '6';
	if(!ROW3) return 'B';
	
	COL0=1;COL1=1;COL2=0;COL3=1;
	delay_ms(1);
	if(!ROW0) return '7';
	if(!ROW1) return '8';
	if(!ROW2) return '9';
	if(!ROW3) return 'C';
	
	COL0=1;COL1=1;COL2=1;COL3=0;
	delay_ms(1);
	if(!ROW0) return '*';
	if(!ROW1) return '0';
	if(!ROW2) return '#';
	if(!ROW3) return 'D';
	
	return 0xFF;
	
}


