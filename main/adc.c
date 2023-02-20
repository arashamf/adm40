#include "defs.h"
#include "adc.h"
#include "pins.h"
#include "timer.h"
#include "GlobalVars.h"
#include "stm32f4xx_hal.h"
#include "gpio.h"
#include "connection.h"

ADC_HandleTypeDef hadc1;					//Из за какой-то заморочки в HALовских библиотеках
								//повторную инициализацию АЦП надо делать новым хендлом 

void show_acc(void);


/* ADC1 init function */
void MX_ADC1_Init(ADC_HandleTypeDef hadc)
{
  ADC_ChannelConfTypeDef sConfig = {0};

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion) 
  */
  hadc.Instance = ADC1;
  hadc.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc.Init.Resolution = ADC_RESOLUTION_12B;
  hadc.Init.ScanConvMode = DISABLE;
  hadc.Init.ContinuousConvMode = DISABLE;
  hadc.Init.DiscontinuousConvMode = DISABLE;
  hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc.Init.NbrOfConversion = 1;
  hadc.Init.DMAContinuousRequests = DISABLE;
  hadc.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc) != HAL_OK)
  {
    Error_Handler();
  }
  sConfig.Channel = ADC_CHANNEL_14;        //Только канал 7
  sConfig.Rank = 1;                       //Опрашивается только 1 канал
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
}

void HAL_ADC_MspInit(ADC_HandleTypeDef* adcHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(adcHandle->Instance==ADC1)
  {
//     ADC1 clock enable
    __HAL_RCC_ADC1_CLK_ENABLE();
  
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

//    ADC1 GPIO Configuration    
//    PA7     ------> ADC1_IN7
//    PC4     ------> ADC1_IN14 

    GPIO_InitStruct.Pin = GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
  }
}

void HAL_ADC_MspDeInit(ADC_HandleTypeDef* adcHandle)
{

  if(adcHandle->Instance==ADC1)
  {
    /* Peripheral clock disable */
    __HAL_RCC_ADC1_CLK_DISABLE();
  
    /**ADC1 GPIO Configuration    
    PA2     ------> ADC1_IN2
    PA3     ------> ADC1_IN3 
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_7);
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_4);

  }
} 

//https://radiohlam.ru/stm32_13/#2.1
//Регистры ADC
void ADC1_init(void)
{
	ADC1->CR2 &= ~ADC_CR2_ADON; // запретить АЦП

	ADC1->CR1 |= (1<<24); //Недокументированный бит !!!											// (1<<25) - 8 bit conversion result, (1<<24) - 10 bit
	//ADC2->SMPR1 &= ~((1<<14)|(1<<13)|(1<<12));	// the fastest samling time=3, 700ns
	ADC1->SMPR1 = 0;														// sampling time = 3 for all the channels
	ADC->CCR &= ~((1<<17)|(1<<16));							// ADC prescaler = PCLK2 divided by 2
	ADC1->SQR1 =0; // 1 регулярный канал
	ADC1->SQR3 = ADC_VIN; 												// channel#1 selected
//	ADC1->CR2 |= 0x01;														// ADC1 is ON
	ADC1->CR2 &= ~ADC_CR2_CONT; // запрет непрерывного режима
	ADC1->CR1 &= ~ADC_CR1_SCAN; // запрет режима сканирования
	ADC1->CR2 |= ADC_CR2_ADON; // разрешить АЦП
}


/*
void ADC1_calobr(void)
{
// внутренняя калибровка АЦП

	__NOP();		// после включения, перед калибровкой
	__NOP();		// нужны минимум два пустых такта проца
	ADC_ResetCalibration(ADC1);				// сброс калибровки
	while(ADC_GetResetCalibrationStatus(ADC1) == SET){}	// просто тупить, пока не завершится сброс калибровки
	ADC_StartCalibration(ADC1);				// старт калибровки
	while(ADC_GetCalibrationStatus(ADC1) == SET){}		// просто тупить, пока не завершится калибровка
}
*/

unsigned int ADC1_getValue (void)
{
//	ADC1->CR2 |= ADC_CR2_SWSTART;								// SWSTART and ADON to start single ADC conversion
	ADC1->CR2 |= (1<<30)|(1<<0);								// SWSTART and ADON to start single ADC conversion
	while(!(ADC1->SR&(1<<1)));
	return (uint16_t) ADC1->DR;
}


void ADC1_ChannelSelect(unsigned int channel)
{
	channel = channel & 0x1f;
	ADC1->SQR3 = channel;
}

void ReadVoltage(void)
{
	unsigned int i=0;
	vin_voltage=0;
	vbat_voltage=0;
	

	for(i=0;i<100;i++)
	 {
	  ADC1_ChannelSelect(ADC_VIN);
	  vin_voltage=vin_voltage+ADC1_getValue();
	  ADC1_ChannelSelect(ADC_VBAT);
	  vbat_voltage=vbat_voltage+ADC1_getValue();
	 }

	ADC1_ChannelSelect(ADC_VIN);

	vin_voltage=vin_voltage/100;
	vbat_voltage=vbat_voltage/100;
	
	vin_voltage=(vin_voltage*3300)/1023;	
	vbat_voltage=(vbat_voltage*3300)/1023;	
	
	vin_voltage=vin_voltage*(10000+1100)/1100;
	vbat_voltage=vbat_voltage*(10000+1100)/1100;

}
