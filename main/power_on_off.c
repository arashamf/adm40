#include "stm32f4xx_hal.h"
#include "defs.h"
#include "gpio.h"
#include "adc.h"
#include "pins.h"
#include "timer.h"
#include "GlobalVars.h"
#include "iwdg.h"
#include "pwm.h"
#include "connection.h"

#define WDT {IWDG->KR = 0x0000AAAAU;}

unsigned int vin_voltage=0,vbat_voltage=0;
unsigned int charge_state=0; //=0 charge off, =1 charge on
unsigned int charge_count=0;
unsigned int count_bat_state=0;

ADC_HandleTypeDef hadc1_pon;

void power_off(void)
{
	int delay_LED = 0;
//Проверим здесь состояние линии RX
//Если линия стоит в нуле TIMER_COMPUTER оборотов main, то
//значит конмьютер выключен и надо все выключать
	if(CHECK_COMPORT == 0) { if(timer_computer < (TIMER_COMPUTER+100)) {timer_computer++;}}
	else									 { timer_computer = 0; }

	if(timer_computer < TIMER_COMPUTER)
		{
			if( flag_shoot_down == 0) { return; }		//Компьютер не выключен, флаг отключения не установлен
																							//Ничего не делаем
		}
			
  //Установлен флаг отключения flag_shoot_down или обнаружено выключение компьютера 
	//Приступаем к выключению периферии и контроллера
	 if(flag_low_battery != 0) 
							{
								beep(1000);
								delay_ms(1500);

								NVIC_SystemReset();		//Выключение при питании от батарей						
								while(1){}
							}

		beep(1000);
		delay_ms(1500);

		ALL_LED_OFF;
		ALL_POWER_OFF;

//При включенном 220V выключили всю периферию и контроллер и крутимся здесь
	 while((flag_low_battery == 0)&&(BUT_POWER !=0))
			{
				if(delay_LED > 0)     {delay_LED--;}  
				if(delay_LED == 0)    {delay_LED = 2000; POWER_LED(1); STATUS_LED(1); }
				if(delay_LED == 1900) {POWER_LED(0); STATUS_LED(0);}
				
				ExecuteBattery(); //Крутимся здесь, пока не перешли на батареи
				delay_ms(1);
				WDT;
			}
		
	if(BUT_POWER !=0) { delay_ms(1000); }
			
	NVIC_SystemReset();		  					//Если перешли на батареи, то выключились						
	while(1){}                        //Если на сети, то включение после сброса
}

void ExecuteBattery(void)
{
	if(ADM40100 != 0)
		{
			BAT_ENABLE(0);
			CHARGE_ENABLE(0);
			return;
		}
	
//Измерили входное напряжение и напряжение аккумулятора
		ReadVoltage();
		BAT_ENABLE(1);

		if(BAT_COMP == 0)	//Система питается от сети
			{
				LED_BAT(0);
				if(flag_low_battery !=0)
					{
						flag_low_battery = 0; old_flag_low_battery = 0; 
						battery_beep_delay = 0;
						send_answer(NOTIFY_READ_POWER_STATUS,0);
					}

				shutdown_battery_delay = 0;								//Отмена выключения

				count_bat_state = 2000;
			//Проверим, не зарядился ли аккумулятор
				if(vbat_voltage>VOLTAGE_FOR_STOP_CHARGE)
					{
						CHARGE_ENABLE(0);
						charge_state=0;
					}

//Проверим, не нужно ли заряжать аккумулятор
				if(vbat_voltage<VOLTAGE_FOR_START_CHARGE)
					{
						CHARGE_ENABLE(1);
						charge_state=1;
					}
			}
		else							//Система питается от батареи
			{
				CHARGE_ENABLE(0);					//Отключили заряд батареи						
				LED_BAT(1);
				charge_state=0;

				if( vbat_voltage<VOLTAGE_FOR_BATTERY_OFF) 																					   { flag_low_battery = 3; }
				if((vbat_voltage>VOLTAGE_FOR_BATTERY_OFF+200)&&(vbat_voltage<VOLTAGE_FOR_BATTERY_LOW)) { flag_low_battery = 2; }
				if( vbat_voltage>VOLTAGE_FOR_BATTERY_LOW+200)                                          { flag_low_battery = 1; }

				if(flag_low_battery != old_flag_low_battery)
					{
						if(flag_low_battery == 3)
							{
								send_answer(NOTIFY_EXTRA_SHUTDOWN,0);									//Сообщение об аварийном разряде батареи и отключении через какое-то время
								battery_beep_delay = BATTERY_BEEP_EXTRA;
								shutdown_battery_delay = DELAY_BATTERY_SHUTDOWN;			//Запуск задержки выключения
								repeat_message_time=REPEAT_MESSAGE_TIME;
							}

						if(flag_low_battery == 2)
							{
								send_answer(NOTIFY_READ_POWER_STATUS,0);					//Сообщение об аварийном разряде батареи и отключении через какое-то время
								battery_beep_delay = BATTERY_BEEP_SHORT;
								shutdown_battery_delay = 0;						  }

						if(flag_low_battery == 1)
							{
								send_answer(NOTIFY_READ_POWER_STATUS,0);					//Сообщение об аварийном разряде батареи и отключении через какое-то время
								battery_beep_delay = BATTERY_BEEP_LONG;
								shutdown_battery_delay = 0;
							}
					}
				old_flag_low_battery = flag_low_battery;
					
				if(count_bat_state == 0)								//Ждем, пока счетчик не станет равный нулю 
					{
						if(vin_voltage>VOLTAGE_FOR_POWER_ON) //Проверяем возможность перехода на питание от сети
							{
								BAT_ENABLE(0);										//И переходим на сеть
								delay_ms(SWITCH_POWER_DELAY);			//Минимальная задержка 2
								BAT_ENABLE(1);
								shutdown_battery_delay = 0;
								flag_low_battery = -1;						//Индикация перехода на сетевое питание
								old_flag_low_battery = 0;
								battery_beep_delay = 0;
							}
					}
			}
}

//Определяем причину системного сброса
//0 - не было сброса
//1 - LOW POWER RESET
//2 - сброс по WWWD
//3 - сброс по IWDT
//4 - soft reset
//5 - POWER ON RESET
//6 - EXTERNAL PIN RESET
//7 - BROWNOUT RESET (BOR)
//8 - Неизвестная причина сброса
int get_system_reset_cause(void)
 {
		int reset_cause = 0;
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_LPWRRST))
		  {
        reset_cause = 1; //LOW POWER RESET
      }
		 else if (__HAL_RCC_GET_FLAG(RCC_FLAG_WWDGRST))
      {
        reset_cause = 2; //сброс по WWWD
      }
    else if (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST))
      {
        reset_cause = 3; //сброс по IWDT 
      }
    else if (__HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST))
      {
        reset_cause = 4; //soft reset по NVIC_SystemReset()
			}
    else if (__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST))
      {
        reset_cause = 5; //POWER ON RESET
      }
    else if (__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST))
      {
        reset_cause = 6; //EXTERNAL PIN RESET
      }
    else if (__HAL_RCC_GET_FLAG(RCC_FLAG_BORRST))
      {
        reset_cause = 7; //BROWNOUT RESET (BOR)
      }
    else
      {
        reset_cause = 8; //Неизвестная причина сброса
      }

    // Clear all the reset flags or else they will remain set during future resets until system power is fully removed.
    __HAL_RCC_CLEAR_RESET_FLAGS();
    return reset_cause; 
 }


 
//Сброс, если поднялось питание от сети
void reset_if_power_on(void)
{
	while(1)
		{
			ReadVoltage();
			if(vin_voltage>VOLTAGE_FOR_POWER_ON) { NVIC_SystemReset(); }
		}
}
 
//Проверяем работу от сети.
//Если работаем от батареи - не включаемся
//Если сброс произошел по IWDT, то включаемся

void beep(int time);

void power_on(void)
{
	int reset_cause;

	reset_cause = get_system_reset_cause();
	HAL_Init();
	MX_GPIO_Init();
	MX_ADC1_Init(hadc1_pon);
	ADC1_init();
	
//От батареи не работаем, но если был сброс от IWDT, то работаем.
		BAT_ENABLE(0);
		ReadVoltage();
		ReadVoltage();

	if((vin_voltage<VOLTAGE_FOR_POWER_OFF)&&(reset_cause != 3))
			{
				reset_if_power_on();
			}

//		if((vin_voltage<VOLTAGE_FOR_POWER_OFF)&&(reset_cause != 3)) { while(1){}}		//Висим здесь и ждем, когда отключится питание
	
		BAT_ENABLE(1);
}
