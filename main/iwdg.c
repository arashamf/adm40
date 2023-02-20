#include "stm32f4xx_hal.h"
#include "iwdg.h"

IWDG_HandleTypeDef hiwdg;

void MX_IWDG_Init(void)
{

  hiwdg.Instance = IWDG;
//  hiwdg.Init.Prescaler = IWDG_PRESCALER_128;
  hiwdg.Init.Prescaler = IWDG_PRESCALER_64;
//  hiwdg.Init.Prescaler = IWDG_PRESCALER_16;

  hiwdg.Init.Reload = 4095;
  if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
  {
    Error_Handler();
  }

}

//IWDG->KR = 0x0000AAAAU;
