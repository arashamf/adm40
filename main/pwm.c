#include "pwm.h"
#include <stm32f4xx.h>

int time_beep=0;
//������������� TIM3 ����� 2 �� ����� ���
void pwm_Init(void)
{
    // �������� ������������ ������ TIM3
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
    
    // ������������:
    TIM3->PSC = 8;						//������� ������� 10 ���
		TIM3->ARR = (10000000 / 2000) -1;		//��� 2 ���

// �����: ���1 0b110
    TIM3->CCMR1 &= ~TIM_CCMR1_OC2M;  // ������� ���� ������
    TIM3->CCMR1 |= (TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_0); // ������� 0b110
    // ����� 2 ��������
    TIM3->CCER |= TIM_CCER_CC2E;
    
    // ��������� ������ � ������ ��������, ���� ��� ���������� �������� ����� ����������.
    TIM3->EGR = TIM_EGR_UG;
    TIM3->CCR2 = TIM3->ARR + 1;
    
    // �������� ���� TIM3
    TIM3->CR1 |= TIM_CR1_CEN;
}

// ������ ���������� � %
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
