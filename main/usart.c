/* Includes ------------------------------------------------------------------*/
#include "usart.h"
#include "gpio.h"
#include "timer.h"
#include "connection.h"
#include "pins.h"
#include "GlobalVars.h"

UART_HandleTypeDef huart4;
UART_HandleTypeDef huart1;

/* UART4 init function */
void MX_UART4_Init(void)
{

  huart4.Instance = UART4;
  huart4.Init.BaudRate = 115200;
  huart4.Init.WordLength = UART_WORDLENGTH_8B;
  huart4.Init.StopBits = UART_STOPBITS_1;
  huart4.Init.Parity = UART_PARITY_NONE;
  huart4.Init.Mode = UART_MODE_TX_RX;
  huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart4.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart4) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}
void MX_USART1_UART_Init(void)
{

  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if(uartHandle->Instance==UART4)
  {
    /* UART4 clock enable */
    __HAL_RCC_UART4_CLK_ENABLE();
  
//    UART4 GPIO Configuration    
//    PA0/WKUP     ------> UART4_TX
//    PA1          ------> UART4_RX 
    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF8_UART4;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  }
  else if(uartHandle->Instance==USART1)
  {
    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();
  
//    USART1 GPIO Configuration    
//    PA10     ------> USART1_RX
//    PA9      ------> USART1_TX 

    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==UART4)
  {
    /* Peripheral clock disable */
    __HAL_RCC_UART4_CLK_DISABLE();
  
    /**UART4 GPIO Configuration    
    PA0/WKUP     ------> UART4_TX
    PA1     ------> UART4_RX 
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_0|GPIO_PIN_1);

  }
  else if(uartHandle->Instance==UART7)
  {
    /* Peripheral clock disable */
//    __HAL_RCC_UART7_CLK_DISABLE();
  
    /**UART7 GPIO Configuration    
    PF6     ------> UART7_RX
    PF7     ------> UART7_TX 
    */
//    HAL_GPIO_DeInit(GPIOF, GPIO_PIN_6|GPIO_PIN_7);

  }
  else if(uartHandle->Instance==USART1)
  {
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();
  
    /**USART1 GPIO Configuration    
    PA10     ------> USART1_RX
    PA9     ------> USART1_TX 
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_10);
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9);
  }
} 

FlagStatus USART_GetFlagStatus(USART_TypeDef* USARTx, uint32_t USART_FLAG)
{
  FlagStatus bitstatus = RESET;
  /* Check the parameters */
  assert_param(IS_USART_ALL_PERIPH(USARTx));
  assert_param(IS_USART_FLAG(USART_FLAG));
  
  if ((USARTx->SR & USART_FLAG) != (uint16_t)RESET)
  {
    bitstatus = SET;
  }
  else
  {
    bitstatus = RESET;
  }
  return bitstatus;
}

uint16_t USART_ReceiveData(USART_TypeDef* USARTx)
{
  /* Check the parameters */
  assert_param(IS_USART_ALL_PERIPH(USARTx));
  
  /* Receive Data */
  return (uint16_t)(USARTx->DR & (uint16_t)0x01FF);
}

void USART_SendData(USART_TypeDef* USARTx, uint16_t Data)
{
  /* Check the parameters */
  assert_param(IS_USART_ALL_PERIPH(USARTx));
  assert_param(IS_USART_DATA(Data)); 
    
  /* Transmit Data */
  USARTx->DR = (Data & (uint16_t)0x01FF);
}


//Процедуры работы с UART4
void UART4_SendByte(char b)
{
	int timeout=300000;
	while (USART_GetFlagStatus(UART4,UART_FLAG_TXE)==RESET)
	{
		if(timeout--==0)return;
	}
	USART_SendData(UART4,b);
}

void UART4_SendBytes (char *text, unsigned int len)
{
	int i=0;
	for(i=0;i<len;i++)
	{
		UART4_SendByte(*text);
		text++;
	}
}	

//Прием байта с таймаутом 2 сек.
int UART4_RecieveByte(void)
{
	int i=0;
	
	while (!(UART4->SR & USART_SR_RXNE))
    {
			delay_us(10);
			i++;
			if(i>5000000) { return -1; }   // 200000 2000000 
		}
	return UART4->DR;
}

void UART4_EnableInt(void)
{
	UART4->CR1|= USART_CR1_RXNEIE;
	NVIC_EnableIRQ(UART4_IRQn);
}

void UART4_IRQHandler(void)
{
		
	unsigned char data=0;
	
	data=USART_ReceiveData(UART4);
	putFifo(4,data);
	timer_computer = 0;   //Произошел прием байта, значит компьютер не выключен
												//Обнулим таймер индикатора "выключенности" компьютера
}

//Процедуры работы с USART1
void UART1_SendByte(char b)
{
	int timeout=300000;
	while (USART_GetFlagStatus(USART1,UART_FLAG_TXE)==RESET)
	{
		if(timeout--==0)return;
	}
	USART_SendData(USART1,b);
}

void UART1_SendBytes (char *text, unsigned int len)
{
	int i=0;
	for(i=0;i<len;i++)
	{
		UART1_SendByte(*text);
		text++;
	}
}	

//Прием байта с таймаутом 2 сек.
int UART1_RecieveByte(void)
{
	int i=0;
	
	while (!(USART1->SR & USART_SR_RXNE))
    {
			delay_us(10);
			i++;
			if(i>5000000) { return -1; }   // 200000 2000000 
		}
	return USART1->DR;
}

void UART1_EnableInt(void)
{
	USART1->CR1|= USART_CR1_RXNEIE;
	NVIC_EnableIRQ(USART1_IRQn);
}


void USART1_IRQHandler(void)
{
	unsigned char data=0;
	
	data=USART_ReceiveData(USART1);
	putFifo(1,data);
	timer_computer = 0;   //Произошел прием байта, значит компьютер не выключен
												//Обнулим таймер индикатора "выключенности" компьютера
}

void UART_SendBytes (char *buf, unsigned int len, unsigned int num_UART)
{
	int i;

	if(num_UART == 1)
	{
		for(i=0;i<=len;i++)
			{
				UART1_SendByte(buf[i]);
			}
		UART1_SendByte(0x0D);
		UART1_SendByte(0x0A);
	}

	if(num_UART == 4)
	{
		for(i=0;i<=len;i++)
			{
				UART4_SendByte(buf[i]);
			}
		UART4_SendByte(0x0D);
		UART4_SendByte(0x0A);
	}
}

