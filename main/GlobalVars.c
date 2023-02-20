// глобальные переменные проекта
#include "GlobalVars.h"

unsigned char number_card[8];		// Буфер для записи номера карты

int door_sensor=0;
int service_sensor=0;
int rigel_sensor=0;
int bag_sensor=0;
int validator_sensor=0;
int cover_sensor=0;
int button_power=0;

int trig_door_sensor=0;
int trig_service_sensor=0;
int trig_rigel_sensor=0;
int trig_bag_sensor=0;
int trig_validator_sensor=0;
int trig_cover_sensor=0;
int trig_button_power=0;

//int flag_battery=0;
int flag_low_battery=0;
int old_flag_low_battery=0;

int battery_beep = 0;
int battery_beep_delay = 0;
int monitor_button_delay = 0;
int shutdown_delay = 0;
int shutdown_battery_delay = 0;
//int shutdown_no_stop = 0;

volatile unsigned int tes = 0;
volatile int flag_shoot_down = 0;
volatile int timer_computer = 0;

int repeat_message_time = 0;
int repeat_button_message = 0;
