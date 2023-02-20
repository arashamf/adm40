#ifndef FlagTime
#define FlagTime                             //������������ ������������ define

#define StartYear 2000
#define CorrectYearH 2050
#define CorrectYearL 2018

typedef struct
{
 signed int Year;
 signed char Month;
 signed char Day;
 signed char H;
 signed char M;
 signed char S;
 signed char Alarm;
} SysTime;

extern SysTime SystemTime;					  //��������� ������� ��� ������� ���
extern SysTime qTime;
extern SysTime dTime;
#endif



void InitRTC(int Flag);

//�������� ����� � DS3231M
SysTime DSGetTime(void);

//���������� ������� ����� � DS3231M �� ��������� SysTime
void DSSetTime(SysTime DSTime);

//���������� ����� � ��������� ��� ������� ��� �������
//������� ����� �� SystemTime � RTC
void SetTime(void);

//���������� �����
void SetupTime(unsigned int Year, unsigned char Month, unsigned char Day, unsigned char H, unsigned char M, unsigned char S);

//���������� alarm
void SetupAlarm(unsigned char H, unsigned char M);

void TimeOutDS(unsigned char line);
void DateOutDS(unsigned char line);

unsigned int GetTime(unsigned int Flag);
unsigned long ReadSecondTime (void);

void SetTimeDS(void);
void get_temperature(char *str_temperature);
