#include "Led7Control.h"

//the opcodes for the MAX7221 and MAX7219
#define OP_NOOP   0
#define OP_DIGIT0 1
#define OP_DIGIT1 2
#define OP_DIGIT2 3
#define OP_DIGIT3 4
#define OP_DIGIT4 5
#define OP_DIGIT5 6
#define OP_DIGIT6 7
#define OP_DIGIT7 8
#define OP_DECODEMODE  9
#define OP_INTENSITY   10
#define OP_SCANLIMIT   11
#define OP_SHUTDOWN    12
#define OP_DISPLAYTEST 15

/* The array for shifting the data to the devices */
        byte spidata[16];
        
        /* We keep track of the led-status for all 8 devices in this array */
        byte status[64];
        /* Data is shifted out of this pin*/
        int SPI_MOSI;
        /* The clock is signaled on this pin */
        int SPI_CLK;
        /* This one is driven LOW for chip selectzion */
        int SPI_CS;
        /* The maximum number of devices we use */
        int maxDevices;
				
				/*
 * Segments to be switched on for characters and digits on
 * 7-Segment Displays
 */
const static byte charTable [] PROGMEM  = {
    0x7E,0x30,0x6D,0x79,0x33,0x5B,0x5F,0x70,
    0x7F,0x7B,0x77,0x1F,0xD,0x3D,0x4F,0x47,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x80,0x01,0x80,0x00,
    0x7E,0x30,0x6D,0x79,0x33,0x5B,0x5F,0x70,
    0x7F,0x7B,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x77,0x1F,0xD,0x3D,0x4F,0x47,0x00,
    0x37,0x00,0x00,0x00,0xE,0x00,0x00,0x00,
    0x67,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x8,
    0x00,0x77,0x1F,0xD,0x3D,0x4F,0x47,0x00,
    0x37,0x00,0x00,0x00,0xE,0x00,0x15,0x1D,
    0x67,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

void Led7Control(int dataPin, int clkPin, int csPin, int numDevices) {
    SPI_MOSI=dataPin;
    SPI_CLK=clkPin;
    SPI_CS=csPin;
    if(numDevices<=0 || numDevices>8 )
        numDevices=8;
    maxDevices=numDevices;
    pinMode(SPI_MOSI,OUTPUT);
    pinMode(SPI_CLK,OUTPUT);
    pinMode(SPI_CS,OUTPUT);
    digitalWrite(SPI_CS,HIGH);
    SPI_MOSI=dataPin;
    for(int i=0;i<64;i++) 
        status[i]=0x00;
    for(int i=0;i<maxDevices;i++) {
        spiTransfer(i,OP_DISPLAYTEST,0);
        //scanlimit is set to max on startup
        LedControl_setScanLimit(i,7);
        //decode is done in source
        spiTransfer(i,OP_DECODEMODE,0);
        LedControl_clearDisplay(i);
        //we go into shutdown-mode on startup
        LedControl_shutdown(i,true);
    }
}

int LedControl_getDeviceCount() {
    return maxDevices;
}

void LedControl_shutdown(int addr, boolean b) {
    if(addr<0 || addr>=maxDevices)
        return;
    if(b)
        spiTransfer(addr, OP_SHUTDOWN,0);
    else
        spiTransfer(addr, OP_SHUTDOWN,1);
}

void LedControl_setScanLimit(int addr, int limit) {
    if(addr<0 || addr>=maxDevices)
        return;
    if(limit>=0 && limit<8)
        spiTransfer(addr, OP_SCANLIMIT,limit);
}

void LedControl_setIntensity(int addr, int intensity) {
    if(addr<0 || addr>=maxDevices)
        return;
    if(intensity>=0 && intensity<16)	
        spiTransfer(addr, OP_INTENSITY,intensity);
}

void LedControl_clearDisplay(int addr) {
    int offset;

    if(addr<0 || addr>=maxDevices)
        return;
    offset=addr*8;
    for(int i=0;i<8;i++) {
        status[offset+i]=0;
        spiTransfer(addr, i+1,status[offset+i]);
    }
}

void LedControl_setLed(int addr, int row, int column, boolean state) {
    int offset;
    byte val=0x00;

    if(addr<0 || addr>=maxDevices)
        return;
    if(row<0 || row>7 || column<0 || column>7)
        return;
    offset=addr*8;
    val=0x80 >> column;
    if(state)
        status[offset+row]=status[offset+row]|val;
    else {
        val=~val;
        status[offset+row]=status[offset+row]&val;
    }
    spiTransfer(addr, row+1,status[offset+row]);
}

void LedControl_setRow(int addr, int row, byte value) {
    int offset;
    if(addr<0 || addr>=maxDevices)
        return;
    if(row<0 || row>7)
        return;
    offset=addr*8;
    status[offset+row]=value;
    spiTransfer(addr, row+1,status[offset+row]);
}

void LedControl_setColumn(int addr, int col, byte value) {
    byte val;

    if(addr<0 || addr>=maxDevices)
        return;
    if(col<0 || col>7) 
        return;
    for(int row=0;row<8;row++) {
        val=value >> (7-row);
        val=val & 0x01;
        LedControl_setLed(addr,row,col,val);
    }
}

void LedControl_setDigit(int addr, int digit, byte value, boolean dp) {
    int offset;
    byte v;

    if(addr<0 || addr>=maxDevices)
        return;
    if(digit<0 || digit>7 || value>15)
        return;
    offset=addr*8;
    v=pgm_read_byte_near(charTable + value); 
    if(dp)
        v|=0x80;
    status[offset+digit]=v;
    spiTransfer(addr, digit+1,v);
}

void LedControl_setChar(int addr, int digit, char value, boolean dp) {
    int offset;
    byte index,v;

    if(addr<0 || addr>=maxDevices)
        return;
    if(digit<0 || digit>7)
        return;
    offset=addr*8;
    index=(byte)value;
    if(index >127) {
        //no defined beyond index 127, so we use the space char
        index=32;
    }
    v=pgm_read_byte_near(charTable + index); 
    if(dp)
        v|=0x80;
    status[offset+digit]=v;
    spiTransfer(addr, digit+1,v);
}



u8 SPI_Read_Byte(){
	int i=0;
	u8 rec=0;	
//	for(i=0;i<8;i++){//11111111
//		SPI_SCLK=1;	
//		
//		rec<<=1;
//		if(SPI_DI) rec++;
//		
//		SPI_SCLK=0;
//	}	
	for(i=0;i<8;i++) 
	{
		SPI_SCLK=1;	
		rec |= SPI_DI << (7 - i) ;
		SPI_SCLK=0;
	}
	return rec;
}


void SPI_Send_Byte(u8 txd){//11011111&0000 0000
	int i=0;
	
//	for(i=0;i<8;i++){
//		SPI_DO=(txd&0x80)>>7;
//		txd<<=1;
//		
//		SPI_SCLK=1;
//		SPI_SCLK=0;
//		
//	}
	
	for(i=0;i<8;i++){
		
		SPI_DO=!!(txd&(1<<(7-i)));		
		
		SPI_SCLK=1;
		SPI_SCLK=0;
		
	}
	
	
}
//////////////////////////////////////////////////////////////

void spiTransfer(int addr, volatile byte opcode, volatile byte data) {
    //Create an array with the data to shift out
    int offset=addr*2;
    int maxbytes=maxDevices*2;
		int i=0;	

    for( i=0;i<maxbytes;i++)
        spidata[i]=(byte)0;
    //put our device data into the array
    spidata[offset+1]=opcode;
    spidata[offset]=data;
    //enable the line 
    //digitalWrite(SPI_CS,LOW);
	 SPI_SS=0;
    //Now shift out the data 
    for( i=maxbytes;i>0;i--)
     //shiftOut(SPI_MOSI,SPI_CLK,MSBFIRST,spidata[i-1]);
	
		SPI_Send_Byte(spidata[i-1]);
	
		SPI_SS=1;	
    //latch the data onto the display
    //digitalWrite(SPI_CS,HIGH);
}   

