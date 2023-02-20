#define SW_VERSION "20201013;1.0"
#define HW_VERSION "20200313;3.0"

//������ ��� ���������� ��������
#define VOLTAGE_FOR_START_CHARGE 12000				/* ��������� ������ */ 
#define VOLTAGE_FOR_STOP_CHARGE  12500				/* ���������� ������ */
#define VOLTAGE_FOR_BATTERY_LOW  10000				/* ���������� ������������, ��� ������� ���������� ��������� � �������*/
#define VOLTAGE_FOR_BATTERY_OFF  9000				/* ���������� ������������, ��� ������� ���������� ��������� � ������� � ������������ ���������� */
#define VOLTAGE_FOR_BATTERY_ON   11500				/* ����������, ��� ������� ������������ ��������� ���������� */
#define VOLTAGE_FOR_POWER_ON     11000				/* ������� ����������, ��� ������� ���������, ��� �������� ������� �� ���� */
#define VOLTAGE_FOR_POWER_OFF    10000				/* ������� ����������, ��� ������� ���������, ��� ������������ ������� �� ���� */

#define SENSOR_EXP 40
#define DELAY_COMMAND_SHUTDOWN 90000						/* �������� ���������� ��������������� ����� ���������� ������� ������� */
#define DELAY_BATTERY_SHUTDOWN 90000						/* �������� ���������� ��������������� ����� ���������� ������� ������� */
#define SWITCH_POWER_DELAY 8
#define DELAY_MONITOR_BUTTON 500

#define BATTERY_BEEP_WIDTH 200
#define BATTERY_BEEP_LONG  4000
#define BATTERY_BEEP_SHORT 1000
#define BATTERY_BEEP_EXTRA 400

#define TIMER_COMPUTER 10000         /* ����� �������� ������� ����� ��� ������ ������������ ��������� ���������� */

#define SHOW_LEDS 1                  /* ���� == 1, �� �������� �� ���������� ��������� �������� � ������ */
#define BEEP_WHEN_LOCK 1             /* ���� == 1, ������ ��� ����������/������������� ����� */
#define LOCK_BEEP 100                /* ����� ���� ��� ����������/������������� ����� */

#define REPEAT_MESSAGE_TIME 1000     /* ����� ������� �������� �������� ����� ��������� �������� */
#define REPEAT_BUTTON_MESSAGE 3      /* ������� ��� ��������� ��������� � ������� ������ ������� */
#define BEEP_IF_PRESS_BUTTON 1       /* ==1 - �������� ���, ���� ������ ������ */

//#define CHECK_FLASH 0                /* ==1 - ��� ������ ��������� ����������� �������� */
