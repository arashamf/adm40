#ifndef __adc_H
 #define __adc_H
 #ifdef __cplusplus
  extern "C" {
 #endif

 #include "main.h"
 extern ADC_HandleTypeDef hadc1;

 #define ADC_VIN  14
 #define ADC_VBAT 7

 extern unsigned int vin_voltage,vbat_voltage;
 extern unsigned int charge_state; //=0 charge off, =1 charge on
/* USER CODE END Private defines */

 void ADC1_init(void);
 void MX_ADC1_Init(ADC_HandleTypeDef hadc);
 unsigned int ADC1_getValue (void);
 void ADC1_ChannelSelect(unsigned int channel);
 void ReadVoltage(void);
 void ExecuteBattery(void);
 int get_system_reset_cause(void);
 void power_on(void);
#endif
