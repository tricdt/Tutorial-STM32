#include "SPI.h"  


void SPIx_Init(SPI_TypeDef * SPIx, u16 SPI_BaudRate)
{

	SPI_InitTypeDef SPI_InitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);		// Enable clock GPIO
	
	if(SPIx==SPI1)
	{
		GPIO_Set(GPIOA,GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7,GPIO_Mode_AF_PP,GPIO_Speed_50MHz);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);																// Enable clock SPI1
	}
	else if(SPIx==SPI2)
	{
		GPIO_Set(GPIOB,GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15,GPIO_Mode_AF_PP,GPIO_Speed_50MHz);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);																// Enable clock SPI2 or SPI3
	}

	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRate;
	SPI_InitStruct.SPI_Direction= SPI_Direction_2Lines_FullDuplex;
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStruct.SPI_CRCPolynomial = 7;
	SPI_Init(SPIx, &SPI_InitStruct);

	SPI_Cmd(SPIx, ENABLE);
}

uint8_t SPI_Transfer(SPI_TypeDef * SPIx,u8 data)
{
	while(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE)==RESET);
	SPI_I2S_SendData(SPIx,data);

	while(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE)==RESET);
	return SPI_I2S_ReceiveData(SPIx);
}

