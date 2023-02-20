/*
+�������� ������ ������� ������� - ���������� �� �������
+�������� �� ���� ������� �� ������� �� ������ POWER 3 ����
�������� ����������� �������� ��������� � �����������
��������� ���������� ������ ��� ������������
*/

#include "main.h"
#include "stm32f4xx_hal.h"
#include "usart.h"
#include "gpio.h"
#include "adc.h"
#include <string.h>
#include <stdint.h>
#include "pins.h"
#include "timer.h"
#include "GlobalVars.h"
#include "iwdg.h"
#include "pwm.h"
#include "check_sensors.h"
#include "connection.h"
#include "onewire.h"
#include "power_on_off.h"

#define WDT {IWDG->KR = 0x0000AAAAU;}

#define BUFFER_SIZE ((uint32_t)0x000100)
#define WRITE_READ_ADDR ((uint32_t)0x000800)

const char ver[] __attribute__((at(VERSION_ADDRESS))) = "20210201";
volatile const char check_sum[] __attribute__((at(CHECKSUM_ADDRESS))) = "1234";

extern int flag_low_battery;

void init(void);

int main(void)
{
int rfid_num = 0;

	SCB->VTOR = APPLICATION_ADDRESS;

	__enable_irq();		//��� ���������� ���������� ����������. ����� ��?

	power_on();
	
	flag_shoot_down = 0;
	init();
	POWER_LED(1); STATUS_LED(1);
	LED_OK(0);
	delay_ms(1000);
	MX_IWDG_Init();

	if(ADM40100 != 0) //���� ������� ����� ������������� � ���100 
	{	
		RFID_READER(1); 
		VALIDATOR_ON(1);
	}
	else {	VALIDATOR(1);	}
	COMPUTER_POWER(1);
	MON_POWER(1);
	PRINTER_POWER(1);
	CARD_READER_POWER(1);
	
	
	flag_low_battery = 0;

	LOCK(0);
//	if(ADM40100 != 0) {	RFID_READER(1); }

while(1)
  {
		ExecuteBattery();				//��������� ������ �� �������
		check_sensors();				//��������� �������
		recieve_command();			//��������� � ������ �������
		power_off();						//���� ����, ��������� �������
		repeat_message();				//��������� ��������� ���������
		delay_ms(1);
		WDT;										//����� IWDT

		if(ADM40100 != 0)
			{
				rfid_num++;
				if(rfid_num == 400)	{ rfid_num = 0; check_card(); }
			}
	}
}
