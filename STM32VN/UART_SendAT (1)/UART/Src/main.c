/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  *
  * COPYRIGHT(c) 2016 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/

#include "sys.h"
#include "exti.h"
#include "usart.h"
/////////////////////////////////////////////////////////////////////////////////////

/* Private variables ---------------------------------------------------------*/
uint8_t serial_available_1 = 0;
/* Private variables ---------------------------------------------------------*/


/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);


/* Private function prototypes -----------------------------------------------*/
//void MX_GPIO_Init(void);
//	

char buffer[100];
uint8_t rx_buff[100];
int i=0;

int answer;

int sendAT(char* ATcmd, char* rec_answer, unsigned int timeout) {
  int x=0, answer=0; uint8_t c=0;
  char response[100];
  //unsigned long previous;
  memset(response, '\0', 100);      // Initialize the string
  delay(100);
  while( USART_Getc(USART1) > 0) ;
  printf("%s\r\n",ATcmd);
  x = 0;
  //previous = millis();
  do {
		delay(1);
		c= USART_Getc(USART1);//
    if(c){    
      response[x] = c;
      x++;
      if (strstr(response, rec_answer) != NULL)    
      {
        answer = 1; //ok
      }
    }
  }
  while((answer == 0) && ( --timeout>0 ));    
  return answer;
}


void init_SIM808(void) {
  do { answer = sendAT("AT+CREG?","+CREG: 0,1",1000);} while(answer==0);     // Connecting to the network
// config GSM
  do { answer = sendAT("ATE0","OK",1000);} while(answer==0);
  do { answer = sendAT("AT+CMGF=1", "OK",1000);} while(answer==0);          // Set SMS into text mode
  do { answer = sendAT("AT+CNMI=2,2,2,0,0", "OK",1000);} while(answer==0);  // Hien thi truc tiep noi dung tin nhan
  do { answer = sendAT("AT+CLIP=1", "OK",1000);} while(answer==0);          //Hien thi thon g tin nguoi goi den 
 
  }


int main(void)
{
	HAL_Init();
	SystemClock_Config(); 
	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
  //////////////////////////////////
	//MX_GPIO_Init();
//	GPIO_Set(GPIOC,GPIO_PIN_12|GPIO_PIN_13,GPIO_MODE_OUTPUT_PP,GPIO_PULLUP,GPIO_SPEED_FREQ_LOW);

//	GPIO_Set(GPIOD,GPIO_PIN_12|GPIO_PIN_13,GPIO_MODE_OUTPUT_PP,GPIO_PULLUP,GPIO_SPEED_FREQ_LOW);

//	HAL_Delay(2000);
//	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
//	
//	EXTIx_Init(GPIOA,GPIO_PIN_0, Trigger_Rising);
	
		pinMode(PC13,OUTPUT);
		digitalWrite( PC13, LOW );	
		delay(2000);
		
	
	USART_Init(USART1, Pins_PA9PA10, 115200);
	USART_Puts(USART1, "STM32VN\n");
	//delay(500);
	
//	DMA_USARTx_Init(USART1);
//	HAL_UART_Receive_DMA(&uartdma,rx_buff,100);

	 do { answer = sendAT("AT","OK",1000);} while(answer==0);
	
	 init_SIM808();
	
	
  while (1)
  {
		
//		digitalWrite( PC13, LOW );	
//		delay_us(500000);
//		digitalWrite( PC13, HIGH );
//		delay_us(500000);
		
//		char	c = USART_Getc(USART1);//USART1->DR;
//		if (c) {
//			/* If anything received, put it back to terminal */
//			USART_Putc(USART1, c);
//		}
//		
		if (USART_Gets(USART1, buffer, sizeof(buffer))) 	printf("%s\r\n",buffer);//USART_Puts(USART1,buffer);
//		
//		if (serial_available_1) {
//			
//        serial_available_1 = 0;   
//			
//				if (strstr((char*)rx_buff, "\n") != NULL) 
//					{
//						HAL_UART_DMAStop(&uartdma);
//					  USART_Puts(USART1, (char*) rx_buff); 
//					  for(i=0;i<100;i++)rx_buff[i]=0;
//					  HAL_UART_Receive_DMA(&uartdma,rx_buff,100);
//					}
//				
//		}
//		delay(1500);
		
		
  }
  

}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct ;
  RCC_ClkInitTypeDef RCC_ClkInitStruct ;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

  /* Initializes the CPU, AHB and APB busses clocks */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    //Error_Handler();
  }
  /* Initializes the CPU, AHB and APB busses clocks */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
    //Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC | RCC_PERIPHCLK_USB;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
    //Error_Handler();
  }

	
}

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
//void MX_GPIO_Init(void)
//{

//  GPIO_InitTypeDef GPIO_InitStruct;

//  /* GPIO Ports Clock Enable */
//  __HAL_RCC_GPIOC_CLK_ENABLE();

//  /*Configure GPIO pin Output Level */
//  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

//  /*Configure GPIO pins : PC13 */
//  GPIO_InitStruct.Pin = GPIO_PIN_13;
//  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
//  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
//  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

//}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
    if (huart==&uartdma) serial_available_1 = 1;
    //if (huart==&huart2) serial_available_2 = 1;
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
 

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
