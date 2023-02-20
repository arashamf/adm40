#include "defs.h"
#include "timer.h"
#include "gpio.h"
#include "pins.h"
#include "GlobalVars.h"
#include "pwm.h"

#define	F_APB1	90000000

void delay(unsigned int x)
{
	while(x--);
}


//value 1 = 0.1ms 
void EnableTimer7(unsigned int value)
{
	// enable timer 7
	RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;
	//	TIM7->EGR |= TIM_EGR_UG;
	TIM7->PSC = F_APB1/10000-1; 
	TIM7->ARR = value;
	TIM7->CR1 |= TIM_CR1_CEN;
	TIM7->DIER |= TIM_DIER_UIE;
	NVIC_EnableIRQ(TIM7_IRQn); 
}


unsigned int ReadTimer7(void)
{
	return TIM7->CNT;
}

void DisableTimer7(void)
{
	TIM7->CR1&=~TIM_CR1_CEN;
	TIM7->CNT=0;
}

void TIM7_IRQHandler(void) 
{	  
	TIM7->SR &= ~TIM_SR_UIF;
	DisableTimer7();
}




//value in us
void EnableTimer6(unsigned int value)
{
	// enable timer 6
	RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
	//		TIM6->EGR |= TIM_EGR_UG;
	TIM6->PSC = F_APB1/1000000-1; 
	TIM6->ARR = value;
	TIM6->CR1 |= TIM_CR1_CEN;
	TIM6->DIER |= TIM_DIER_UIE;
	NVIC_EnableIRQ(TIM6_DAC_IRQn); 
	TIM6->SR &= ~TIM_SR_UIF;
}


//state of butons
#define CLEAR_PRESS 0
#define PROCESS_PRESS 1
#define SHORT_PRESS 2
#define LONG_PRESS 3

extern unsigned int count_bat_state;
extern int time_beep;

void TIM6_DAC_IRQHandler(void) 
{	  
	TIM6->SR &= ~TIM_SR_UIF;	
	
	if(count_bat_state != 0) { count_bat_state--; }

//Здесь пикаем, если работа от бесперебойника
	if(battery_beep_delay != 0)
	   {
				battery_beep--;
				if(battery_beep <= 0) {beep(BATTERY_BEEP_WIDTH); battery_beep = battery_beep_delay; }
	   }
//	else
//		 {
//				beep(0);
//		 }

	if(shutdown_delay > 0) { shutdown_delay--; }
	if(monitor_button_delay > 0)
		{
			monitor_button_delay--;
			if(monitor_button_delay == 0){MONITOR_BUTTON(0);}
		}

	if(shutdown_battery_delay > 0) { shutdown_battery_delay--; }

	if((shutdown_delay == 1)||(shutdown_battery_delay == 1)) 
					{
						shutdown_delay = 0;
						shutdown_battery_delay = 0;
						flag_shoot_down = 1;
					}
/*
				if(flag_low_battery != 0) 
							{
								NVIC_SystemReset();		//Выключение при питании от батарей						
								while(1){}
							}
						else
							{
								ALL_POWER_OFF; 				//При включенном 220V выключили всю периферию
							}
					}
*/

	if(time_beep !=0)
		{
			time_beep--;
			if(time_beep <=0 ) { time_beep = 0; TIM3->CCR2 = TIM3->ARR + 1; }			//Выключили звук
		}
}

void DisableTimer6(void)
{
	TIM6->CR1&=~TIM_CR1_CEN;
}

/*
int inkey(void)
 {
   int tmp;
   tmp=scan;
   scan=0;
   return tmp;
 } 
*/

//value in us
void EnableTimer5(unsigned int value)
{
	// enable timer 5
	RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;
	TIM5->PSC = F_APB1/1000000-1; 
	TIM5->ARR = value;
	TIM5->CR1 |= TIM_CR1_CEN;
	TIM5->DIER |= TIM_DIER_UIE;	
	NVIC_EnableIRQ(TIM5_IRQn); 
}


void DisableTimer5(void)
{
	TIM5->CR1&=~TIM_CR1_CEN;
}

//Based on TIM14
void delay_ms(unsigned int delay)
{
	unsigned int clock=0;
	
	if(delay==0) return;
	clock=F_APB1/2;

	RCC->APB1ENR |= RCC_APB1ENR_TIM14EN;
	TIM14->PSC = clock/1000-1;
	TIM14->ARR = delay;
	TIM14->EGR |= TIM_EGR_UG;
	TIM14->CR1 |= TIM_CR1_OPM;
	TIM14->CR1 |= TIM_CR1_CEN;
	while (TIM14->CR1&TIM_CR1_CEN);

	TIM14->PSC = clock/1000-1;
	TIM14->ARR = delay;
	TIM14->EGR |= TIM_EGR_UG;
	TIM14->CR1 |= TIM_CR1_OPM;
	TIM14->CR1 |= TIM_CR1_CEN;
	while (TIM14->CR1&TIM_CR1_CEN);
}
//Based on TIM14
void delay_us(unsigned int delay)
{
	if(delay==0) return;
	RCC->APB1ENR |= RCC_APB1ENR_TIM14EN;
	TIM14->PSC = F_APB1/1000000-1;
	TIM14->ARR = delay;
	TIM14->EGR |= TIM_EGR_UG;
	TIM14->CR1 |= TIM_CR1_OPM;
	TIM14->CR1 |= TIM_CR1_CEN;
	while (TIM14->CR1&TIM_CR1_CEN);
}
