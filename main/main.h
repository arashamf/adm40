/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H__
#define __MAIN_H__

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* USER CODE BEGIN Includes */
#include <stdint.h>
#include "stm32f4xx_hal.h"


#ifdef __cplusplus
 extern "C" {
#endif
void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)
#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H__ */

#define APPLICATION_ADDRESS    0x08010000
//#define VERSION_ADDRESS (APPLICATION_ADDRESS + 0x10000)
#define VERSION_ADDRESS (APPLICATION_ADDRESS + 0x7000)
#define CHECKSUM_ADDRESS (VERSION_ADDRESS + 0x10)

extern const char ver[];
extern volatile const char check_sum[];
