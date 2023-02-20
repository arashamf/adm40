// глобальные переменные проекта - header
//#define sw_version "20190607;1.0"
//#define hw_version "20190412;2.0"

/*
extern unsigned char status;
extern unsigned char saving_run;
extern unsigned char shutter_run;
extern unsigned int reset_run;
extern unsigned int card_insert;
extern unsigned char shutter_state;
extern unsigned char saving_state;
extern unsigned char door_recovery_state;
extern unsigned char attempt_recovering_carriage;
extern unsigned char attempt_recovering_shutter;
extern unsigned int wait_shutter;
extern unsigned int monitor_button_run;
*/

extern unsigned char number_card[8];		// Буфер для записи номера карты

extern unsigned char check_ports;
extern unsigned int count_check_ports;

extern volatile unsigned int tes;

//temperature sensor
extern unsigned int temperature_1;
extern unsigned int temperature_2;

extern int door_sensor;
extern int service_sensor;
extern int rigel_sensor;
extern int bag_sensor;
extern int validator_sensor;
extern int cover_sensor;
extern int button_power;

extern int trig_door_sensor;
extern int trig_service_sensor;
extern int trig_rigel_sensor;
extern int trig_bag_sensor;
extern int trig_validator_sensor;
extern int trig_cover_sensor;
extern int trig_button_power;

//extern int flag_battery;
extern int flag_low_battery;
extern int old_flag_low_battery;

extern int battery_beep;
extern int battery_beep_delay;
extern int monitor_button_delay;
extern int shutdown_delay;
extern int shutdown_battery_delay;

//extern int shutdown_no_stop;

extern unsigned int Error;
extern unsigned int CountError;

extern volatile int flag_shoot_down;
extern volatile int timer_computer;

extern int repeat_message_time;
extern int repeat_button_message;

