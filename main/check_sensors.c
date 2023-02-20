#include "defs.h"
#include "gpio.h"
#include "pins.h"
#include "GlobalVars.h"
#include "check_sensors.h"
#include "connection.h"
#include "pwm.h"

//Вход датчика
//Признак инверсии датчика
//*sensor - указатель на переменную для сглаживания дребезга
//*trig - указатель на переменную, в которой будет записано сглаженное состояние датчика
//*flag - указатель на переменную, в которую будет записан признак изменения состояния датчиков
void check_sensor(int sens,int inv, int *sensor, int *trig, int *flag)
{
	if(*sensor < -SENSOR_EXP) { *sensor = -SENSOR_EXP;}
	if(*sensor >  SENSOR_EXP) { *sensor =  SENSOR_EXP; }

	if(inv == 0)
		{
			if((sens != 0)&&(*sensor <  SENSOR_EXP)) { (*sensor)++; }
			if((sens == 0)&&(*sensor > -SENSOR_EXP)) { (*sensor)--; }
		}
	else
		{
			if((sens == 0)&&(*sensor <  SENSOR_EXP)) { (*sensor)++; }
			if((sens != 0)&&(*sensor > -SENSOR_EXP)) { (*sensor)--; }
		}
		
	if(((*sensor) ==  SENSOR_EXP)&&((*trig) == 0)) { *trig = 1; (*flag) |= 1;} 
	if(((*sensor) == -SENSOR_EXP)&&((*trig) == 1)) { *trig = 0; (*flag) |= 1;} 
}

void show_sensors(void)
{
//Датчик двери
	if(SAFE_DOOR != 0){LED8(1);}
	else							{LED8(0);}	
	
//Датчик положения сервисной зоны
	if(SERVICE_DOOR !=0){LED7(1);}
	else								{LED7(0);}	

//Датчик мешка
	if(MONEY_BAG != 0){LED6(1);}
	else							{LED6(0);}
	
//Датчик ригеля
	if(LOCK_RIGEL !=0){LED5(1);}
	else							{LED5(0);}			

//Датчик положения валидатора
	if(VALIDATOR_UNIT !=0){LED4(1);}
	else									{LED4(0);}

//Крышка сервисной зоны
	if(ADM40100 == 0)
		{
			if(SERVICE_COVER !=0)	{LED3(1);}
			else									{LED3(0);}
		}
	else
	{
		LED3(0);
	}
//Кнопка POWER
//	if(BUT_POWER == 0) 	{LED2(1);}
//	else								{LED2(0);}
}

//проверка датчиков на двери сейфа и на крышке системного модуля
void check_sensors(void)
{
	int flag = 0;
	int flag_but = 0;
	
#if SHOW_LEDS !=0
	show_sensors();
#endif
	
//Чтение датчиков, устранение дребезга
	check_sensor(SAFE_DOOR      ,0                ,&door_sensor,&trig_door_sensor,&flag);
	check_sensor(SERVICE_DOOR   ,0                ,&service_sensor,&trig_service_sensor,&flag);
	check_sensor(MONEY_BAG      ,0                ,&bag_sensor,&trig_bag_sensor,&flag);
	check_sensor(LOCK_RIGEL     ,INVERS_LOCK_RIGEL,&rigel_sensor,&trig_rigel_sensor,&flag);
	check_sensor(VALIDATOR_UNIT ,0                ,&validator_sensor,&trig_validator_sensor,&flag);
	if(ADM40100 == 0)
		{
			check_sensor(SERVICE_COVER,0,&cover_sensor,&trig_cover_sensor,&flag);
			check_sensor(BUT_POWER,0,&button_power,&trig_button_power,&flag_but);
		}

	if(flag !=0)      { send_answer(NOTIFY_READ_DAT_1_2,0); /*beep(100);*/ }
	if((trig_button_power == 0)&&(flag_but !=0))
		{
			repeat_message_time=0;
			repeat_button_message=REPEAT_BUTTON_MESSAGE;
#if BEEP_IF_PRESS_BUTTON != 0
			beep(50);
#endif
		} 
}
