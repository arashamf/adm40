#include "ds3231.h"
#include "bincod.h"
#include <stdio.h>
#include "SoftI2C.h"
#include <string.h>
#include "pins.h"

const char Months[13]={0,31,28,31,30,31,30,31,31,30,31,30,31};
const char MonthsLY[13]={0,31,29,31,30,31,30,31,31,30,31,30,31};

SysTime SystemTime;					  //��������� ������� ��� ������� ���
SysTime qTime;
SysTime dTime;

#define ACK 1
#define NACK 0
char buf_short_ds[10];

void WriteDSReg(int AdrReg, int DatReg)
{
 I2C_SoftStart();
 I2C_SoftWriteByte(b1101_0000);
 I2C_SoftWriteByte(AdrReg);
 I2C_SoftWriteByte(DatReg);
 I2C_SoftStop();
}

unsigned char ReadDSReg(int AdrReg)
{
 unsigned char tmp;
	
 I2C_SoftStart(); 
 I2C_SoftWriteByte(b1101_0000);
 I2C_SoftWriteByte(AdrReg);
 I2C_SoftStart(); 
 I2C_SoftWriteByte(b1101_0001);
 tmp=I2C_SoftReadByte(NACK) & 0xFF;
 I2C_SoftStop();
	
 return tmp;
}


//������������� BCD � ������� ���. lim - ����������� �������� �������
//255 - ������������ �����
unsigned char BCD2BIN(unsigned char x, int lim)
{
 int tmp;
 
 tmp=(x >> 4) & 15;
 if(tmp > lim) { return 255; }

 return (tmp*10+(x & 15));
}

//������������� �� �������� ���� � BCD
unsigned char BIN2BCD(unsigned char x)
{
 if(x > 99) return 255;
 return ((x / 10)<<4)+(x % 10);
}


extern unsigned char NewH;
extern unsigned char NewM;

//��������� ������� ����� � DS3231M.
SysTime DSGetTime(void)
{
 SysTime DSTime;

 I2C_SoftStart(); 						                     //�����
 I2C_SoftWriteByte(b1101_0000);				                     //������� ������
 I2C_SoftWriteByte(0);						                     //����� �������� ������
 I2C_SoftStart(); 						                     //�������
 I2C_SoftWriteByte(b1101_0001);				                     //������� ������

 DSTime.S=BCD2BIN(I2C_SoftReadByte(ACK) & 0xFF,5);		         //������ �������� ������. MASTER ACK
 DSTime.M=BCD2BIN(I2C_SoftReadByte(ACK) & 0xFF,5);		         //������ �������� �����. MASTER ACK
 DSTime.H=BCD2BIN(I2C_SoftReadByte(ACK) & b0011_1111,2);          //������ �������� �����. MASTER NACK
 I2C_SoftReadByte(ACK);								             //������ ������ ��� ������. ��� �������, ��� ������������ �����
 DSTime.Day=BCD2BIN(I2C_SoftReadByte(ACK) & 0xFF,3);	             //������ ����
 DSTime.Month=BCD2BIN(I2C_SoftReadByte(ACK) & b0011_1111,1);      //������ ������
 DSTime.Year=BCD2BIN(I2C_SoftReadByte(NACK) & 0xFF,9)+StartYear;  //������ ����
 
	
/*	I2C_SoftStart(); 						                     //�����
 I2C_SoftWriteByte(b1101_0000);				                     //������� ������
 I2C_SoftWriteByte(0x0F);					                     //����� �������� �������
 I2C_SoftStart(); 						                     //�������
 I2C_SoftWriteByte(b1101_0001);				                     //������� ������
 DSTime.Alarm=I2C_SoftReadByte(NACK) & 1;	                     //������ �������� �����. MASTER NACK
 */
 
 I2C_SoftStop();						                     //����
 
 return DSTime;
}


//���������� ������� ����� � DS3231M �� ��������� SysTime
void DSSetTime(SysTime DSTime)
{
// wLastTime();												 //�����
 WriteDSReg(0x00,BIN2BCD(DSTime.S));										 //������� �������
 WriteDSReg(0x01,BIN2BCD(DSTime.M)); 						 //������� ������
 WriteDSReg(0x02,BIN2BCD(DSTime.H) & b0011_1111);		     //������� ���� � ���������� 24 ������� �����
 WriteDSReg(0x04,BIN2BCD(DSTime.Day)); 						 //������� ����
 WriteDSReg(0x05,BIN2BCD(DSTime.Month)); 				     //������� �����
 WriteDSReg(0x06,BIN2BCD(DSTime.Year-StartYear)); 	         //������� ���
}

//������� ����� �� SystemTime � RTC
void SetTimeDS(void)
{
 SysTime DSTime;

 //wLastTime();
		
 DSTime.Year=SystemTime.Year;
 DSTime.Month=SystemTime.Month;
 DSTime.Day=SystemTime.Day;
 DSTime.H=SystemTime.H;
 DSTime.M=SystemTime.M;
 DSTime.S=SystemTime.S;
 DSSetTime(DSTime);
}

//���������� ������� ����� � DS3231M
void SetupTime(unsigned int Year, unsigned char Month, unsigned char Day, unsigned char H, unsigned char M, unsigned char S)
{
 SysTime DSTime;
	
//check for correct value
if((Month==2)&&(Day>29)){Day=29;}
if((Month==4)&&(Day>30)){Day=30;}
if((Month==6)&&(Day>30)){Day=30;}
if((Month==9)&&(Day>30)){Day=30;}
if((Month==11)&&(Day>30)){Day=30;}

 DSTime.M=M;
 DSTime.H=H;
 DSTime.S=S;
 DSTime.Day=Day;
 DSTime.Month=Month;
 DSTime.Year=Year;
 DSSetTime(DSTime);
 
 SystemTime=DSTime;
}

//���� Flag==0, �� ����������� �������� ���� Alarm1
void InitRTC(int Flag)
{
 if(Flag != 0)
  {
   WriteDSReg(0x0F, 0);					   //�������� ���� Alarm
  }
  WriteDSReg(0x0E, 0);					   //���������� ������
  WriteDSReg(0x10, 0);
}

//���������� alarm
//���� ��� ��� ������ ����� ����������� �������� - alarm �� �������
void SetupAlarm(unsigned char H, unsigned char M)
{
 if((M < 60) && (H < 24))
  {
   WriteDSReg(0x07,b0000_0000); 			     //������������ �������
   WriteDSReg(0x08,BIN2BCD(M) & b0111_1111);     //������� ������
   WriteDSReg(0x09,BIN2BCD(H) & b0111_1111);     //������� ����
   WriteDSReg(0x0A,b1000_0001);	                 //������������ ����
   WriteDSReg(0x0E, 1);							 //��������� Alarm1
  }
 else
  {
   WriteDSReg(0x0E, 0);							 //��������� Alarm
  }
}


//�������� ����� � ��������� ��� ������� ��� �������
//���� Flag	== 0 - ��������� ����� �� ������������
unsigned int GetTime(unsigned int Flag)
{
	char time_1[20];
	char time_2[20];
	char time_3[20];
	unsigned int res_1=0,res_2=0,res_3=0;
	unsigned int select=0;
	
	SysTime DSTime;
 
 	DSTime=DSGetTime();	     //��������� ����� �� RTC
	sprintf(time_1,"%04d%02d%02d%02d%02d",DSTime.Year,DSTime.Month,DSTime.Day,DSTime.H,DSTime.M);
	DSTime=DSGetTime();	     //��������� ����� �� RTC
	sprintf(time_2,"%04d%02d%02d%02d%02d",DSTime.Year,DSTime.Month,DSTime.Day,DSTime.H,DSTime.M);
	DSTime=DSGetTime();	     //��������� ����� �� RTC
	sprintf(time_3,"%04d%02d%02d%02d%02d",DSTime.Year,DSTime.Month,DSTime.Day,DSTime.H,DSTime.M);

	//compare
	res_1=memcmp(time_1,time_2,12);
	res_2=memcmp(time_1,time_3,12);
	res_3=memcmp(time_2,time_3,12);
	//majority select
	if(res_1==0){select=1;}
	else if (res_2==0){select=1;}
	else if (res_3==0){select=3;}
	
		if(select==1)
	{
		DSTime.Year=(time_1[0]-0x30)*1000+(time_1[1]-0x30)*100+(time_1[2]-0x30)*10+(time_1[3]-0x30);
		DSTime.Month=(time_1[4]-0x30)*10+(time_1[5]-0x30);
		DSTime.Day=(time_1[6]-0x30)*10+(time_1[7]-0x30);
		DSTime.H=(time_1[8]-0x30)*10+(time_1[9]-0x30);
		DSTime.M=(time_1[10]-0x30)*10+(time_1[11]-0x30);
	}
	
	if(select==3)
	{
		DSTime.Year=(time_3[0]-0x30)*1000+(time_3[1]-0x30)*100+(time_3[2]-0x30)*10+(time_3[3]-0x30);
		DSTime.Month=(time_3[4]-0x30)*10+(time_3[5]-0x30);
		DSTime.Day=(time_3[6]-0x30)*10+(time_3[7]-0x30);
		DSTime.H=(time_3[8]-0x30)*10+(time_3[9]-0x30);
		DSTime.M=(time_3[10]-0x30)*10+(time_3[11]-0x30);
	}
	
	if(select==0){
		DSTime.Year=2018;
		DSTime.Month=1;
		DSTime.Day=1;
		DSTime.H=0;
		DSTime.M=0;
		DSTime.S=0;
	}
	
	
//��������� ����� �� RTC �� �������������� � �� ��, ��� ��� �����, ��� ����������� �����
//���� ���-�� �� ���, ��������� ��������� ����� ��� ����������
if(Flag == 0)
 {
    //���� ��������� ����� ����� ����������� ��������, �� ��������������� ���������� ����� � ������������ ������
  if((DSTime.Year < CorrectYearL)||(DSTime.Year > CorrectYearH        )) { goto eGetTime;}
  if((DSTime.Month  < 1      )||(DSTime.Month  > 12                   )) { goto eGetTime;}
  if((DSTime.H > 24                                                   )) { goto eGetTime;}
  if((DSTime.M > 60                                                   )) { goto eGetTime;}
  if((DSTime.S > 60                                                   )) { goto eGetTime;}
  if((DSTime.Day < 1      )||(DSTime.Day > 31                         )) { goto eGetTime;}
 }

 //���������� ����� ����� �� RTC
 SystemTime.Year=DSTime.Year;
 SystemTime.Month=DSTime.Month;
 SystemTime.Day=DSTime.Day;
 SystemTime.H=DSTime.H;						   
 SystemTime.M=DSTime.M;
 SystemTime.S=DSTime.S;
 
	if(Flag==3){goto eGetTime;}
 
 return 1;

eGetTime:;  
	SystemTime.Year=2018;
	SystemTime.Month=2;
	SystemTime.Day=2;
	SystemTime.H=0;
	SystemTime.M=0;
	SystemTime.S=0;
 
SetupTime(SystemTime.Year,SystemTime.Month,SystemTime.Day,SystemTime.H,SystemTime.M,SystemTime.S);
	return 0;
}

void get_temperature(char *str_temperature)
{
 int t_high;
 int t_low;
 int sign = 0;

// reset_i2c(1);
	
//�������, ��� ����������� ����� ������ ��������, � �� �������� ����������
 I2C_SoftStart(); 											//�����
 I2C_SoftWriteByte(b1101_0000);					//������� ������
 I2C_SoftWriteByte(0x11);								//����� �������� �������� �����������
 I2C_SoftStart(); 											//�������
 I2C_SoftWriteByte(b1101_0001);					//������� ������
 t_high = I2C_SoftReadByte(ACK);        //������ �������� �������� �����������
 t_low  = I2C_SoftReadByte(NACK);       //������ �������� �������� �����������
 I2C_SoftStop();			//����

 if((t_high == 0)&&(t_low == 0))
 	{
		strcat(str_temperature,"0.0");
	}
 else
	{
		t_high = ((t_high)&255) << 8 | (t_low&255);
		if((t_high & 0x8000) != 0){t_high=-t_high + 1; sign=1;}  //������������� �����������
		t_low  = (t_high & 0xFF)/64*25;		//����� ������� ����� �����������
		t_high = (t_high & 0xFF00)>>8;		//����� ����� ����� ����������� 

 		if(sign == 0) { sprintf(str_temperature,"+%d.%d",t_high,t_low);}
		else          { sprintf(str_temperature,"-%d.%d",t_high,t_low);}

	}
}
