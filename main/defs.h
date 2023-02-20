#define SW_VERSION "20201013;1.0"
#define HW_VERSION "20200313;3.0"

//Пороги для управления питанием
#define VOLTAGE_FOR_START_CHARGE 12000				/* Включение заряда */ 
#define VOLTAGE_FOR_STOP_CHARGE  12500				/* Выключение заряда */
#define VOLTAGE_FOR_BATTERY_LOW  10000				/* Напряжение аккумулятора, при котором передается сообщение о событии*/
#define VOLTAGE_FOR_BATTERY_OFF  9000				/* Напряжение аккумулятора, при котором передается сообщение о событии и производится выключение */
#define VOLTAGE_FOR_BATTERY_ON   11500				/* Напряжение, при котором прекращается процедура отключения */
#define VOLTAGE_FOR_POWER_ON     11000				/* Входное напряжение, при котором считается, что началось питание от сети */
#define VOLTAGE_FOR_POWER_OFF    10000				/* Входное напряжение, при котором считается, что прекратилось питание от сети */

#define SENSOR_EXP 40
#define DELAY_COMMAND_SHUTDOWN 90000						/* Задержка выключения беспреребойника после аварийного разряда батареи */
#define DELAY_BATTERY_SHUTDOWN 90000						/* Задержка выключения беспреребойника после аварийного разряда батареи */
#define SWITCH_POWER_DELAY 8
#define DELAY_MONITOR_BUTTON 500

#define BATTERY_BEEP_WIDTH 200
#define BATTERY_BEEP_LONG  4000
#define BATTERY_BEEP_SHORT 1000
#define BATTERY_BEEP_EXTRA 400

#define TIMER_COMPUTER 10000         /* Число оборотов главной петли для оценки выключенного состояния компьютера */

#define SHOW_LEDS 1                  /* Если == 1, то выводить на светодиоды состояние датчиков и ключей */
#define BEEP_WHEN_LOCK 1             /* Если == 1, бикать при блокировке/разблокировке замка */
#define LOCK_BEEP 100                /* Время бика при блокировке/разблокировке замка */

#define REPEAT_MESSAGE_TIME 1000     /* Через сколько оборотов главного цикла повторяем передачу */
#define REPEAT_BUTTON_MESSAGE 3      /* Сколько раз повторить сообщение о нажатии кнопки питания */
#define BEEP_IF_PRESS_BUTTON 1       /* ==1 - короткий бип, если нажата кнопка */

//#define CHECK_FLASH 0                /* ==1 - при старте проверять целостность прошивки */
