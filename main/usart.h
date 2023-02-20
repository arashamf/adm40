/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __usart_H
#define __usart_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "main.h"

/* USER CODE BEGIN Includes */
extern volatile int rec;
/* USER CODE END Includes */

extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart7;
extern UART_HandleTypeDef huart1;

/* USER CODE BEGIN Private defines */
	 
extern volatile unsigned char buf_rec[100];	//Буфер приема посылки с USART7
extern volatile unsigned int counter_buf;   //Счетчик числа принятых байт в буфере buf_rec1

/* USER CODE END Private defines */

extern void _Error_Handler(char *, int);

void MX_UART4_Init(void);
void MX_UART7_Init(void);
void MX_USART1_UART_Init(void);

/* USER CODE BEGIN Prototypes */
void UART7_SendString (const char * text);
void UART7_SendByte(char b);
void UART7_EnableInt (void);
void USART_SendData(USART_TypeDef* USARTx, uint16_t Data);
void UART7_SendBytes (const char * text, unsigned int len);

//void UART4_SendString (const char * text);
//void UART4_SendString (char * text);
//void UART4_SendByte(char b);

void UART4_SendByte(char b);
void UART4_SendBytes (char *text, unsigned int len);
int UART4_RecieveByte(void);
int UART4_wait_Byte(void);
void UART4_EnableInt (void);

void UART1_SendByte(char b);
void UART1_SendBytes (char *text, unsigned int len);
int UART1_RecieveByte(void);
int UART1_wait_Byte(void);
void UART1_EnableInt (void);

void UART_SendBytes (char *buf, unsigned int len, unsigned int num_UART);

//int UART1_RecieveByte_st(void);

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ usart_H */

