
#include "WS2812B.h"
#include "sys.h"
#include "SPI.h"


 u8
    begun;         // true if begin() previously called
  uint16_t
    numLEDs,       // Number of RGB LEDs in strip
    numBytes;      // Size of 'pixels' buffer
	
  uint8_t
    brightness,
   *pixels,        // Holds the current LED color values, which the external API calls interact with 9 bytes per pixel + start + end empty bytes
   *doubleBuffer,	// Holds the start of the double buffer (1 buffer for async DMA transfer and one for the API interaction.
    rOffset,       // Index of red byte within each 3- or 4-byte pixel
    gOffset,       // Index of green byte
    bOffset,       // Index of blue byte
    wOffset;       // Index of white byte (same as rOffset if no white)


// Constructor when n is the number of LEDs in the strip
void WS2812B(uint16_t number_of_leds)
  
{
	brightness=0; pixels=NULL;
  WS2812B_updateLength(number_of_leds);
}


void WS2812B_WS2812B() 
{
  if(pixels)   
  {
	  free(pixels);
  }
  //SPIx.end();
}

void WS2812B_begin(void) {

if (!begun)
{
  
	WS2812B(60);
	
//  SPIx.begin();
//	
//	SPIx.setClockDivider(SPI_CLOCK_DIV32);
	SPIx_Init(SPI1, SPI_BaudRatePrescaler_32);//9Mhz
	
	SPIx_Init_DMA(SPI1);
	
	//co the dung PWM tren TIM =72Mhz (thay cho SPI)
	// Basic Init
//  TIM2->PSC = 0;										// no prescale -> clock TIM2 = 72MHz
//	TIM2->ARR = 90;										// Counting 0 ... 89 @ 72MHz = 800kHz (DT = 1,25?)
//	TIM2->CCR1 = 0;										// Capture Compare = 0 -> register will be modified by DMA-Transfer
	
	
  begun = true;
}
}

void WS2812B_updateLength(uint16_t n)
{
  if(doubleBuffer) 
  {
	  free(doubleBuffer); 
  }

  numBytes = (n<<3) + n + 2; // 9 encoded bytes per pixel. 1 byte empty peamble to fix issue with SPI MOSI and on byte at the end to clear down MOSI 
							// Note. (n<<3) +n is a fast way of doing n*9
  if((doubleBuffer = (uint8_t *)malloc(numBytes*2)))
  {
    numLEDs = n;	 
	pixels = doubleBuffer;
	// Only need to init the part of the double buffer which will be interacted with by the API e.g. setPixelColor
	*pixels=0;//clear the preamble byte
	*(pixels+numBytes-1)=0;// clear the post send cleardown byte.
	WS2812B_clear();// Set the encoded data to all encoded zeros 
  } 
  else 
  {
    numLEDs = numBytes = 0;
  }
}

// Sends the current buffer to the leds
void WS2812B_show(void) 
{
  //SPI.dmaSendAsync(pixels,numBytes);// Start the DMA transfer of the current pixel buffer to the LEDs and return immediately.
	//Serial.println(pixels[0]);
	//printf("ok");
	
	//printf("%d-%d-%d",pixels[0],pixels[1],pixels[2]);
	SPI_DMA_Send(SPI1,pixels,numBytes);
  // Need to copy the last / current buffer to the other half of the double buffer as most API code does not rebuild the entire contents
  // from scratch. Often just a few pixels are changed e.g in a chaser effect
  
  if (pixels==doubleBuffer)
  {
	// pixels was using the first buffer
	pixels	= doubleBuffer+numBytes;  // set pixels to second buffer
	memcpy(pixels,doubleBuffer,numBytes);// copy first buffer to second buffer		
  }
  else
  {
	// pixels was using the second buffer	  
	pixels	= doubleBuffer;  // set pixels to first buffer
	memcpy(pixels,doubleBuffer+numBytes,numBytes);	 // copy second buffer to first buffer 		
  }	
}

/*Sets a specific pixel to a specific r,g,b colour 
* Because the pixels buffer contains the encoded bitstream, which is in triplets
* the lookup table need to be used to find the correct pattern for each byte in the 3 byte sequence.
*/
void WS2812B_setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b)
 {
   uint8_t *bptr = pixels + (n<<3) + n +1;
   uint8_t *tPtr = (uint8_t *)encoderLookup + g*2 + g;// need to index 3 x g into the lookup
   
   *bptr++ = *tPtr++;
   *bptr++ = *tPtr++;
   *bptr++ = *tPtr++;

   tPtr = (uint8_t *)encoderLookup + r*2 + r;
   *bptr++ = *tPtr++;
   *bptr++ = *tPtr++;
   *bptr++ = *tPtr++;   
   
   tPtr = (uint8_t *)encoderLookup + b*2 + b;
   *bptr++ = *tPtr++;
   *bptr++ = *tPtr++;
   *bptr++ = *tPtr++;
 }

void WS2812B_setPixelColorX(uint16_t n, uint32_t c)
  {
     uint8_t r,g,b;	
   
    if(brightness) 
	{ 
      r = ((int)((uint8_t)(c >> 16)) * (int)brightness) >> 8;
      g = ((int)((uint8_t)(c >>  8)) * (int)brightness) >> 8;
      b = ((int)((uint8_t)c) * (int)brightness) >> 8;
	}
	else
	{
      r = (uint8_t)(c >> 16),
      g = (uint8_t)(c >>  8),
			b = (uint8_t)c;
	}
	
   uint8_t *bptr = pixels + (n<<3) + n +1;
   uint8_t *tPtr = (uint8_t *)encoderLookup + g*2 + g;// need to index 3 x g into the lookup

   *bptr++ = *tPtr++;
   *bptr++ = *tPtr++;
   *bptr++ = *tPtr++;

   tPtr = (uint8_t *)encoderLookup + r*2 + r;
   *bptr++ = *tPtr++;
   *bptr++ = *tPtr++;
   *bptr++ = *tPtr++;   
   
   tPtr = (uint8_t *)encoderLookup + b*2 + b;
   *bptr++ = *tPtr++;
   *bptr++ = *tPtr++;
   *bptr++ = *tPtr++;
}

// Convert separate R,G,B into packed 32-bit RGB color.
// Packed format is always RGB, regardless of LED strand color order.
uint32_t WS2812B_Color(uint8_t r, uint8_t g, uint8_t b) {
  return ((uint32_t)r << 16) | ((uint32_t)g <<  8) | b;
}

// Convert separate R,G,B,W into packed 32-bit WRGB color.
// Packed format is always WRGB, regardless of LED strand color order.
uint32_t WS2812B_Color1(uint8_t r, uint8_t g, uint8_t b, uint8_t w) {
  return ((uint32_t)w << 24) | ((uint32_t)r << 16) | ((uint32_t)g <<  8) | b;
}


uint16_t WS2812B_numPixels(void) {
  return numLEDs;
}

// Adjust output brightness; 0=darkest (off), 255=brightest.  This does
// NOT immediately affect what's currently displayed on the LEDs.  The
// next call to show() will refresh the LEDs at this level.  However,
// this process is potentially "lossy," especially when increasing
// brightness.  The tight timing in the WS2811/WS2812 code means there
// aren't enough free cycles to perform this scaling on the fly as data
// is issued.  So we make a pass through the existing color data in RAM
// and scale it (subsequent graphics commands also work at this
// brightness level).  If there's a significant step up in brightness,
// the limited number of steps (quantization) in the old data will be
// quite visible in the re-scaled version.  For a non-destructive
// change, you'll need to re-render the full strip data.  C'est la vie.
void WS2812B_setBrightness(uint8_t b) {
  // Stored brightness value is different than what's passed.
  // This simplifies the actual scaling math later, allowing a fast
  // 8x8-bit multiply and taking the MSB.  'brightness' is a uint8_t,
  // adding 1 here may (intentionally) roll over...so 0 = max brightness
  // (color values are interpreted literally; no scaling), 1 = min
  // brightness (off), 255 = just below max brightness.
  uint8_t newBrightness = b + 1;
  if(newBrightness != brightness) { // Compare against prior value
    // Brightness has changed -- re-scale existing data in RAM
    uint8_t  c,
            *ptr           = pixels,
             oldBrightness = brightness - 1; // De-wrap old brightness value
    uint16_t scale;
    if(oldBrightness == 0) scale = 0; // Avoid /0
    else if(b == 255) scale = 65535 / oldBrightness;
    else scale = (((uint16_t)newBrightness << 8) - 1) / oldBrightness;
    for(uint16_t i=0; i<numBytes; i++) 
	{
      c      = *ptr;
      *ptr++ = (c * scale) >> 8;
    }
    brightness = newBrightness;
  }
}

//Return the brightness value
uint8_t WS2812B_getBrightness(void)  {
  return brightness - 1;
}

/*
*	Sets the encoded pixel data to turn all the LEDs off.
*/
void WS2812B_clear() 
{
	uint8_t * bptr= pixels+1;// Note first byte in the buffer is a preable and is always zero. hence the +1
	uint8_t *tPtr;

	for(int i=0;i< (numLEDs *3);i++)
	{
	   tPtr = (uint8_t *)encoderLookup;
   	   *bptr++ = *tPtr++;
	   *bptr++ = *tPtr++;
	   *bptr++ = *tPtr++;	
	}
}

