#include "pwm.h"
#include <stm32f4xx.h>

int time_beep=0;
//Инициализация TIM3 канал 2 на вывод ШИМ
void pwm_Init(void)
{
    // Включить тактирование модуля TIM3
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
    
    // Предделитель:
    TIM3->PSC = 8;						//Входная частота 10 Мгц
		TIM3->ARR = (10000000 / 2000) -1;		//Для 2 кГц

// Режим: ШИМ1 0b110
    TIM3->CCMR1 &= ~TIM_CCMR1_OC2M;  // очистим поле режима
    TIM3->CCMR1 |= (TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_0); // Запишем 0b110
    // Канал 2 включить
    TIM3->CCER |= TIM_CCER_CC2E;
    
    // Передадим сигнал о сбросе счётчика, чтоб все внутренние регистры точно обновились.
    TIM3->EGR = TIM_EGR_UG;
    TIM3->CCR2 = TIM3->ARR + 1;
    
    // Включить счёт TIM3
    TIM3->CR1 |= TIM_CR1_CEN;
}

// Задать скважность в %
void pwm_Set(uint16_t Value)
{
    TIM3->CCR2 = (100-Value)*(TIM3->ARR)/100;
}

void beep(int time)
{
	if(time != 0)
		{
			time_beep=time;
			TIM3->CCR2 = TIM3->ARR / 2;
		}
	else
		{
			time_beep=0;
			TIM3->CCR2 = TIM3->ARR + 1;
		}
}
