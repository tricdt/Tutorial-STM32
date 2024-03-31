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

/////////////////////////////////////////////////////////////////////

void SPIx_Init_DMA(SPI_TypeDef* SPIx)
{
  
    /**USART1 GPIO Configuration    

		goi ham khoi tao SPI_Init(SPI1);
    */
//    pinMode(A6,AF_PP);
//		pinMode(A7,AF_PP);
//	#ifdef SPI1
//	if(SPIx==SPI1){
//	

//	/* SPI interrupt Init */

//	}
//	#endif
//	


	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	/* Configure the Priority Group to 2 bits */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	/* Enable the SPI1 RX DMA Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	/* Enable the SPI1 TX DMA Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	///////////////////////////////////////////////////////

	


}

/**************************************************************************************/
 void DMA1_Channel3_IRQHandler(void) // SPI1_TX
{
/* Test on DMA Transfer Complete interrupt */
if (DMA_GetITStatus(DMA1_IT_TC3))
{
/* Clear DMA Transfer Complete interrupt pending bit */
DMA_ClearITPendingBit(DMA1_IT_TC3);
/* ... */
}
}
/**************************************************************************************/
 void DMA1_Channel2_IRQHandler(void) //SPI1_RX
{
/* Test on DMA Transfer Complete interrupt */
	//printf("ok");
if (DMA_GetITStatus(DMA1_IT_TC2))
{
/* Clear DMA Transfer Complete interrupt pending bit */
DMA_ClearITPendingBit(DMA1_IT_TC2);
/* ... */
}
}
/**************************************************************************************/
void SPI_Receive_DMA(SPI_TypeDef* SPIx,char *pData,uint16_t len)
{
	DMA_InitTypeDef DMA_InitStructure;
			/* SPI2_RX DMA1 Channel 4 (See RM0008) */
		DMA_DeInit(DMA1_Channel2);
		DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&SPIx->DR;
		DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)pData;
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
		DMA_InitStructure.DMA_BufferSize = len - 1;
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
		DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
		DMA_InitStructure.DMA_Priority = DMA_Priority_High;
		DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
		DMA_Init(DMA1_Channel2, &DMA_InitStructure);
		SPI_I2S_DMACmd(SPIx, SPI_I2S_DMAReq_Rx, ENABLE);
		/* Enable DMA Stream Transfer Complete interrupt */
		DMA_ITConfig(DMA1_Channel2, DMA_IT_TC, ENABLE);
		
		DMA_Cmd(DMA1_Channel2, ENABLE);
}
///////////////
uint8_t SPI_DMA_Send(SPI_TypeDef* SPIx, const void* DataArray, uint16_t count) {
	
	
	
	DMA_InitTypeDef DMA_InitStructure;
/* SPI2_TX DMA1 Channel 3 (See RM0008) */
		DMA_DeInit(DMA1_Channel3);
		DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&SPIx->DR;
		DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)DataArray;
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
		DMA_InitStructure.DMA_BufferSize = count - 1;
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
		DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;//DMA_Mode_Circular;//
		DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
		DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
		DMA_Init(DMA1_Channel3, &DMA_InitStructure);
		SPI_I2S_DMACmd(SPIx, SPI_I2S_DMAReq_Tx, ENABLE);
		/* Enable DMA Stream Transfer Complete interrupt */
		DMA_ITConfig(DMA1_Channel3, DMA_IT_TC, ENABLE);
	
		DMA_Cmd(DMA1_Channel3, ENABLE);
//		

	
	/* Enable SPI TX DMA */
	SPIx->CR2 |= SPI_CR2_TXDMAEN;
	/* DMA has started */
	
	return 1;
}

uint8_t SPI_DMA_Puts(SPI_TypeDef* SPIx, const void* DataArray) {
	/* Call DMA Send function */
	//printf("%d",sizeof(DataArray));
	return SPI_DMA_Send(SPIx, DataArray, sizeof(DataArray));
}

///////////////////////////////////////////////////////////////////////////////////////////


