#ifndef Led7Control_h
#define Led7Control_h

#include "sys.h"

// TODO: to be checked
typedef uint8_t boolean ;
typedef uint8_t byte ;

#include <inttypes.h>

#define PROGMEM
#define PGM_P  const char *
#define PSTR(str) (str)

#define pgm_read_byte(addr) (*(const unsigned char *)(addr))
#define pgm_read_byte_near(addr) pgm_read_byte(addr)


#define SPI_DO PBout(15) 
#define SPI_DI PBout(14) 
#define SPI_SCLK PBout(13) 
#define SPI_SS PBout(12) 

#define true 0x1
#define false 0x0
//////////////////////////////

/* Send out a single command to the device */
        void spiTransfer(int addr, byte opcode, byte data);

        /* 
         * Create a new controler 
         * Params :
         * dataPin		pin on the Arduino where data gets shifted out
         * clockPin		pin for the clock
         * csPin		pin for selecting the device 
         * numDevices	maximum number of devices that can be controled
         */
        //void LedControl(int dataPin, int clkPin, int csPin, int numDevices=1);
				void Led7Control(int dataPin, int clkPin, int csPin, int numDevices);

        /*
         * Gets the number of devices attached to this LedControl.
         * Returns :
         * int	the number of devices on this LedControl
         */
        int LedControl_getDeviceCount(void);

        /* 
         * Set the shutdown (power saving) mode for the device
         * Params :
         * addr	The address of the display to control
         * status	If true the device goes into power-down mode. Set to false
         *		for normal operation.
         */
        void LedControl_shutdown(int addr, boolean status);

        /* 
         * Set the number of digits (or rows) to be displayed.
         * See datasheet for sideeffects of the scanlimit on the brightness
         * of the display.
         * Params :
         * addr	address of the display to control
         * limit	number of digits to be displayed (1..8)
         */
        void LedControl_setScanLimit(int addr, int limit);

        /* 
         * Set the brightness of the display.
         * Params:
         * addr		the address of the display to control
         * intensity	the brightness of the display. (0..15)
         */
        void LedControl_setIntensity(int addr, int intensity);

        /* 
         * Switch all Leds on the display off. 
         * Params:
         * addr	address of the display to control
         */
        void LedControl_clearDisplay(int addr);

        /* 
         * Set the status of a single Led.
         * Params :
         * addr	address of the display 
         * row	the row of the Led (0..7)
         * col	the column of the Led (0..7)
         * state	If true the led is switched on, 
         *		if false it is switched off
         */
        void LedControl_setLed(int addr, int row, int col, boolean state);

        /* 
         * Set all 8 Led's in a row to a new state
         * Params:
         * addr	address of the display
         * row	row which is to be set (0..7)
         * value	each bit set to 1 will light up the
         *		corresponding Led.
         */
        void LedControl_setRow(int addr, int row, byte value);

        /* 
         * Set all 8 Led's in a column to a new state
         * Params:
         * addr	address of the display
         * col	column which is to be set (0..7)
         * value	each bit set to 1 will light up the
         *		corresponding Led.
         */
        void LedControl_setColumn(int addr, int col, byte value);

        /* 
         * Display a hexadecimal digit on a 7-Segment Display
         * Params:
         * addr	address of the display
         * digit	the position of the digit on the display (0..7)
         * value	the value to be displayed. (0x00..0x0F)
         * dp	sets the decimal point.
         */
        void LedControl_setDigit(int addr, int digit, byte value, boolean dp);

        /* 
         * Display a character on a 7-Segment display.
         * There are only a few characters that make sense here :
         *	'0','1','2','3','4','5','6','7','8','9','0',
         *  'A','b','c','d','E','F','H','L','P',
         *  '.','-','_',' ' 
         * Params:
         * addr	address of the display
         * digit	the position of the character on the display (0..7)
         * value	the character to be displayed. 
         * dp	sets the decimal point.
         */
        void LedControl_setChar(int addr, int digit, char value, boolean dp);


#endif	//LedControl.h

