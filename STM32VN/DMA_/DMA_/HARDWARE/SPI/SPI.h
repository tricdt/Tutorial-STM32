#ifndef __SPI_H
#define __SPI_H 			   
#include "sys.h"  
//////////////////////////////////////////////////////////////////////////////////	 
void SPIx_Init(SPI_TypeDef * SPIx, u16 SPI_BaudRate);
uint8_t SPI_Transfer(SPI_TypeDef * SPIx,u8 data);
////////////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////////////// 
void SPIx_Init_DMA(SPI_TypeDef* SPIx);
void SPI_Receive_DMA(SPI_TypeDef* SPIx,char *pData,uint16_t len);
////////////
uint8_t SPI_DMA_Send(SPI_TypeDef* SPIx, const void* DataArray, uint16_t count);
uint8_t SPI_DMA_Puts(SPI_TypeDef* SPIx, const void* DataArray);
/////////////////////////////////////////////////////////////////////	 
#endif


