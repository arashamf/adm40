//connection.c for connection with ADM protocol by RS-232                          
#include "connection.h"
#include "usart.h"
#include <string.h>
#include <stdio.h>                   
#include <ctype.h>
#include "defs.h"
#include "pins.h"  				//Using pins/ports
//#include "onewire.h"
#include "timer.h"
#include "time.h"
#include "GlobalVars.h"
#include "ds3231.h"
#include "pwm.h"
#include "adc.h"
#include "onewire.h"

void voltage_to_string(char *str);

char buf_rec_adm1[100];	//Буфер приема посылки UART1 от PC
char buf_rec_adm4[100];	//Буфер приема посылки UART4 от PC

unsigned int counter_buf_adm1=0;   //Счетчик числа принятых байт в буфере buf_rec
unsigned int counter_buf_adm4=0;   //Счетчик числа принятых байт в буфере buf_rec

//char buf_send_adm1[100];	//Буфер отправки посылки через UART1 к PC
//char buf_send_adm4[100];	//Буфер отправки посылки через UART4 к PC

//Флаг сброса
//=0 - сброс не надо делать
//=1 - нужно сделать сброс
unsigned int reset_run=0;

unsigned char inputBuf1[INPUT_BUFFER_SIZE];	// буфер FIFO для хранения полученных по UART данных. 
volatile unsigned int pFifo1 = 0;					  // указатель на текущее местоположение принятого по UART байта
volatile unsigned int lenFifo1 = 0;				  // текущий размер принятого по UART, но еще не выведенного на печать, текста
unsigned char inputBuf4[INPUT_BUFFER_SIZE];	// буфер FIFO для хранения полученных по UART данных. 
volatile unsigned int pFifo4 = 0;					  // указатель на текущее местоположение принятого по UART байта
volatile unsigned int lenFifo4 = 0;				  // текущий размер принятого по UART, но еще не выведенного на печать, текста


// кладет очередной байт в текстовый FIFO буфер (так мы будем укладывать принятые по UART байты)
// возвращает 0 если удалось, 1 если буфер занят
unsigned char putFifo(int num_UART, char byte)
{	
	if(num_UART == 1)
	{
		if(lenFifo1>=INPUT_BUFFER_SIZE) { return 1; }	// если FIFO заполнено, принимать больше некуда, выход с ошибкой
		inputBuf1[pFifo1] = byte;												// положили байт
		pFifo1++;
		if(pFifo1>=INPUT_BUFFER_SIZE) { pFifo1 = 0; }		// У нас кольцевой буфер, поэтому указатель на текущее местоположение ходит по кругу
		lenFifo1++;
	}

	if(num_UART == 4)
	{
		if(lenFifo4>=INPUT_BUFFER_SIZE) { return 1; }	// если FIFO заполнено, принимать больше некуда, выход с ошибкой
		inputBuf4[pFifo4] = byte;												// положили байт
		pFifo4++;
		if(pFifo4>=INPUT_BUFFER_SIZE) { pFifo4 = 0; }		// У нас кольцевой буфер, поэтому указатель на текущее местоположение ходит по кругу
		lenFifo4++;
	}

	return 0;

}


// берет из текстового FIFO буфера очередной байт (так мы будем распечатывать полученный текст)
unsigned char getFifo(int num_UART)
{
	unsigned int startPointer1;
	unsigned int startPointer4;
	unsigned char tmp;
	
	if(num_UART == 1)
		{
			NVIC_DisableIRQ(USART1_IRQn);									// выключаем UART0 что бы не было putFifoText в это время
			if (lenFifo1 == 0) { NVIC_EnableIRQ(USART1_IRQn);	return 0; }	// если FIFO кончился, вернем 0
			if (pFifo1>=lenFifo1) { startPointer1 = pFifo1-lenFifo1; }		// вычисляем указатель на начало FIFO (байт, пришедший раньше всех)
			else { startPointer1 = INPUT_BUFFER_SIZE - lenFifo1 + pFifo1; }
			lenFifo1--;																		// уменьшаем текущий объем данных в FIFO
			tmp = inputBuf1[startPointer1];									// собственно извлечение байта и декремент текущего размера FIFO
			NVIC_EnableIRQ(USART1_IRQn);										// включаем UART0	
		}

	if(num_UART == 4)
		{
			NVIC_DisableIRQ(UART4_IRQn);									// выключаем UART0 что бы не было putFifoText в это время
			if (lenFifo4 == 0) { NVIC_EnableIRQ(UART4_IRQn);	return 0; }	// если FIFO кончился, вернем 0
			if (pFifo4>=lenFifo4) { startPointer4 = pFifo4-lenFifo4; }		// вычисляем указатель на начало FIFO (байт, пришедший раньше всех)
			else { startPointer4 = INPUT_BUFFER_SIZE - lenFifo4 + pFifo4; }
			lenFifo4--;																		// уменьшаем текущий объем данных в FIFO
			tmp = inputBuf4[startPointer4];									// собственно 7извлечение байта и декремент текущего размера FIFO
			NVIC_EnableIRQ(UART4_IRQn);										// включаем UART0	
		}

	return tmp; 
}

//прием команды, распарсивание, выполнение и ответа на команду
void recieve1_command (void)
{
		unsigned char data=0;

		data=getFifo(1);
		if(data)
		{				
			//принят начальный байт посылки STX
			if(data==0x02)
			 {			
				counter_buf_adm1=1;
				buf_rec_adm1[0]=data;
				return;
			 }
		
			//запись данных в буфер команды
			if(counter_buf_adm1>0)
				{
					buf_rec_adm1[counter_buf_adm1]=data;
					counter_buf_adm1++;
			  }
					
			//Проверка на окончание посылки
			if(data==0x0A)
				{				
				 counter_buf_adm1=0;		
				 check_message(1);  //проверка команды, ее выполнение и ответ	
			  }
		}	
}

void recieve4_command (void)
{
		unsigned char data=0;
		data=getFifo(4);
		if(data)
		{				
			//принят начальный байт посылки STX
			if(data==0x02)
				{			
				 counter_buf_adm4=1;
				 buf_rec_adm4[0]=data;
				 return;
		    }
		
			//запись данных в буфер команды
			if(counter_buf_adm4>0)
				{
					buf_rec_adm4[counter_buf_adm4]=data;
					counter_buf_adm4++;
			  }
					
			//Проверка на окончание посылки
			if(data==0x0A)
				{				
				 counter_buf_adm4=0;		
				 check_message(4);  //проверка команды, ее выполнение и ответ	
			  }
		}
}

void recieve_command (void)
{
	recieve1_command();
	recieve4_command();
}

//Очистка 1 приемного буфера
void clear_buf_rec_adm(int num_COM)
{
	unsigned int i=0;
	if(num_COM == 1) { for(i=0;i<sizeof(buf_rec_adm1);i++) buf_rec_adm1[i]=0; }
	if(num_COM == 4) { for(i=0;i<sizeof(buf_rec_adm4);i++) buf_rec_adm4[i]=0; }
}

//Очистка 1 буфера на передачу
//void clear_buf_send_adm(int num_COM)
//{
//	unsigned int i=0;
//	if(num_COM == 1) { for(i=0;i<sizeof(buf_send_adm1);i++) buf_send_adm1[i]=0; }
//	if(num_COM == 4) { for(i=0;i<sizeof(buf_send_adm4);i++) buf_send_adm4[i]=0; }
//}

void SaveLog(int type,char *com)
{
}

//Подпрограмма, разбирающая пришедшее сообщение
void check_message(int num_UART)
{
	unsigned char crc=0;
	unsigned int i=0;
	char com[4]={0,0,0,0};
//	int type=0; 
		
	if(num_UART == 1)
		{
	//Проверка начала и конца сообщения
			if((buf_rec_adm1[0]!=0x02)||(buf_rec_adm1[LENGTH_DATA_REC1+6]!=0x03))
				{
					counter_buf_adm1=0;
					clear_buf_rec_adm(1);
					return;
				}

	//подсчет и проверка контрольной суммы пришедшего сообщения
			for(i=1;i<=LENGTH_DATA_REC1+3;i++) {crc=crc^buf_rec_adm1[i];}

			i=atoh(buf_rec_adm1[LENGTH_DATA_REC1+4],buf_rec_adm1[LENGTH_DATA_REC1+5]);	

			if(i!=crc){counter_buf_adm1=0;clear_buf_rec_adm(1);return;} //если не прошла проверка контрольной суммы	

			com[0]=buf_rec_adm1[4];
			com[1]=buf_rec_adm1[5];
			com[2]=buf_rec_adm1[6];
			counter_buf_adm1=0;

		//сохранить в лог файл команду
			buf_rec_adm1[LENGTH_DATA_REC1+4]=0;
		}	

	if(num_UART == 4)
		{
	//Проверка начала и конца сообщения
			if((buf_rec_adm4[0]!=0x02)||(buf_rec_adm4[LENGTH_DATA_REC4+6]!=0x03))
				{
					counter_buf_adm4=0;
					clear_buf_rec_adm(4);
					return;
				}

	//подсчет и проверка контрольной суммы пришедшего сообщения
			for(i=1;i<=LENGTH_DATA_REC4+3;i++) {crc=crc^buf_rec_adm4[i];}

			i=atoh(buf_rec_adm4[LENGTH_DATA_REC4+4],buf_rec_adm4[LENGTH_DATA_REC4+5]);	

			if(i!=crc){counter_buf_adm4=0;clear_buf_rec_adm(4);return;} //если не прошла проверка контрольной суммы	

			com[0]=buf_rec_adm4[4];
			com[1]=buf_rec_adm4[5];
			com[2]=buf_rec_adm4[6];
			counter_buf_adm4=0;

		//сохранить в лог файл команду
			buf_rec_adm4[LENGTH_DATA_REC4+4]=0;
		}	
				
//Common
	if (!memcmp (com, "CON",3)){send_answer(CHECK_CONNECTION,num_UART);}
	if (!memcmp (com, "SHD",3)){send_answer(SHUTDOWN_CONTROLLER,num_UART);}
	if (!memcmp (com, "VER",3)){send_answer(CHECK_VERSION,num_UART);}
//	if (!memcmp (com, "RST",3)){send_answer(UNKNOWN_COMMAND,num_UART);} //{send_answer(READ_STATUS,num_UART);}
	if (!memcmp (com, "RST",3)){send_answer(READ_STATUS,num_UART);}
	if (!memcmp (com, "CST",3)){send_answer(UNKNOWN_COMMAND,num_UART);} //{send_answer(CLEAR_STATUS,num_UART);}
	if (!memcmp (com, "VEH",3)){send_answer(CHECK_HW_VERSION,num_UART);}
	//Motor
	if (!memcmp (com, "SM1",3)){send_answer(UNKNOWN_COMMAND,num_UART);}//{send_answer(STOP_MOTOR_1,num_UART);}
	if (!memcmp (com, "FM1",3)){send_answer(UNKNOWN_COMMAND,num_UART);}//{send_answer(FORWARD_MOTOR_1,num_UART);}
	if (!memcmp (com, "RM1",3)){send_answer(UNKNOWN_COMMAND,num_UART);}//{send_answer(REVERSE_MOTOR_1,num_UART);}
	if (!memcmp (com, "LM1",3)){send_answer(UNKNOWN_COMMAND,num_UART);}//{send_answer(LOAD_MOTOR_1,num_UART);}
	if (!memcmp (com, "SM2",3)){send_answer(UNKNOWN_COMMAND,num_UART);}//{send_answer(STOP_MOTOR_2,num_UART);}
	if (!memcmp (com, "FM2",3)){send_answer(UNKNOWN_COMMAND,num_UART);}//{send_answer(FORWARD_MOTOR_2,num_UART);}
	if (!memcmp (com, "RM2",3)){send_answer(UNKNOWN_COMMAND,num_UART);}//{send_answer(REVERSE_MOTOR_2,num_UART);}
	if (!memcmp (com, "LM2",3)){send_answer(UNKNOWN_COMMAND,num_UART);}//{send_answer(LOAD_MOTOR_2),num_UART;}
	if (!memcmp (com, "SM3",3)){send_answer(UNKNOWN_COMMAND,num_UART);}//{send_answer(STOP_MOTOR_3),num_UART;}
	if (!memcmp (com, "FM3",3)){send_answer(UNKNOWN_COMMAND,num_UART);}//{send_answer(FORWARD_MOTOR_3,num_UART);}
	if (!memcmp (com, "RM3",3)){send_answer(UNKNOWN_COMMAND,num_UART);}//{send_answer(REVERSE_MOTOR_3,num_UART);}
	if (!memcmp (com, "LM3",3)){send_answer(UNKNOWN_COMMAND,num_UART);}//{send_answer(LOAD_MOTOR_3,num_UART);}
	//Dat
	if (!memcmp (com, "RD1",3)){send_answer(READ_DAT_1,num_UART);}
	if (!memcmp (com, "RD2",3)){send_answer(READ_DAT_2,num_UART);}
	if (!memcmp (com, "RD3",3)){send_answer(READ_DAT_3,num_UART);}
	if (!memcmp (com, "RD4",3)){send_answer(READ_DAT_4,num_UART);}
	if (!memcmp (com, "RD5",3)){send_answer(READ_DAT_5,num_UART);}
	if (!memcmp (com, "RD6",3)){send_answer(READ_DAT_6,num_UART);}
	if (!memcmp (com, "RDA",3)){send_answer(READ_DAT_ALL,num_UART);}
	//Switch power
	if (!memcmp (com, "AH1",3)){send_answer(ACTIVATED_HIGH_1,num_UART);}
	if (!memcmp (com, "DH1",3)){send_answer(DEACTIVATED_HIGH_1,num_UART);}
	if (!memcmp (com, "AH2",3)){send_answer(UNKNOWN_COMMAND,num_UART);}//{send_answer(ACTIVATED_HIGH_2,num_UART);}
	if (!memcmp (com, "DH2",3)){send_answer(UNKNOWN_COMMAND,num_UART);}//{send_answer(DEACTIVATED_HIGH_2,num_UART);}
	if (!memcmp (com, "AMP",3)){send_answer(ACTIVATED_MONITOR_POWER,num_UART);}
	if (!memcmp (com, "DMP",3)){send_answer(DEACTIVATED_MONITOR_POWER,num_UART);}
	if (!memcmp (com, "ACR",3)){send_answer(ACTIVATED_CARDREADER,num_UART);}
	if (!memcmp (com, "DCR",3)){send_answer(DEACTIVATED_CARDREADER,num_UART);}
	if (!memcmp (com, "APC",3)){send_answer(ACTIVATED_PC,num_UART);}
	if (!memcmp (com, "DPC",3)){send_answer(DEACTIVATED_PC,num_UART);}	
	
	if (!memcmp (com, "ARP",3)){send_answer(ACTIVATED_RESERVE_POWER,num_UART);}
	if (!memcmp (com, "DRP",3)){send_answer(DEACTIVATED_RESERVE_POWER,num_UART);}		
	
	//Button monitor
	if (!memcmp (com, "PMB",3)){send_answer(PRESS_MONITOR_BUTTON,num_UART);}
	//Solenoid
	if (!memcmp (com, "AS1",3)){send_answer(UNKNOWN_COMMAND,num_UART);}//{send_answer(ACTIVATED_SOLENOID_1);}
	if (!memcmp (com, "DS1",3)){send_answer(UNKNOWN_COMMAND,num_UART);}//{send_answer(DEACTIVATED_SOLENOID_1);}
	// Light
	if (!memcmp (com, "AEL",3)){send_answer(ACTIVATED_ESCROW_LED,num_UART);}
	if (!memcmp (com, "DEL",3)){send_answer(DEACTIVATED_ESCROW_LED,num_UART);}
	if (!memcmp (com, "APL",3)){send_answer(ACTIVATED_PRINTER_LIGHT,num_UART);}
	if (!memcmp (com, "DPL",3)){send_answer(DEACTIVATED_PRINTER_LIGHT,num_UART);}
	//Lock                                     
	if (!memcmp (com, "AL1",3)){send_answer(ACTIVATED_LOCK_1,num_UART);}
	if (!memcmp (com, "DL1",3)){send_answer(DEACTIVATED_LOCK_1,num_UART);}
	if (!memcmp (com, "AL2",3)){send_answer(UNKNOWN_COMMAND,num_UART);}//{send_answer(ACTIVATED_LOCK_2,num_UART);}
	if (!memcmp (com, "DL2",3)){send_answer(UNKNOWN_COMMAND,num_UART);}//{send_answer(DEACTIVATED_LOCK_2,num_UART);}
	//Optosensors

	if (!memcmp (com, "RO1",3)){send_answer(UNKNOWN_COMMAND,num_UART);}//{send_answer(READ_OPT_1,num_UART);}
	if (!memcmp (com, "RO2",3)){send_answer(UNKNOWN_COMMAND,num_UART);}//{send_answer(READ_OPT_2,num_UART);}
	if (!memcmp (com, "RO3",3)){send_answer(UNKNOWN_COMMAND,num_UART);}//{send_answer(READ_OPT_3,num_UART);}
	if (!memcmp (com, "RO4",3)){send_answer(UNKNOWN_COMMAND,num_UART);}//{send_answer(READ_OPT_4,num_UART);}	
	if (!memcmp (com, "RO5",3)){send_answer(UNKNOWN_COMMAND,num_UART);}//{send_answer(READ_OPT_5,num_UART);}
	if (!memcmp (com, "RO6",3)){send_answer(UNKNOWN_COMMAND,num_UART);}//{send_answer(READ_OPT_6,num_UART);}	
	if (!memcmp (com, "RO7",3)){send_answer(UNKNOWN_COMMAND,num_UART);}//{send_answer(READ_OPT_7,num_UART);}
	if (!memcmp (com, "RO8",3)){send_answer(UNKNOWN_COMMAND,num_UART);}//{send_answer(READ_OPT_8,num_UART);}	
	if (!memcmp (com, "RO9",3)){send_answer(UNKNOWN_COMMAND,num_UART);}//{send_answer(READ_OPT_9,num_UART);}
	if (!memcmp (com, "O10",3)){send_answer(UNKNOWN_COMMAND,num_UART);}//{send_answer(READ_OPT_10,num_UART);}
	if (!memcmp (com, "ROA",3)){send_answer(UNKNOWN_COMMAND,num_UART);}//{send_answer(READ_OPT_ALL,num_UART);}	

//Shutter
	if (!memcmp (com, "OS1",3)){send_answer(UNKNOWN_COMMAND,num_UART);}//{send_answer(SHUTTER_OPEN,num_UART);}
	if (!memcmp (com, "CS1",3)){send_answer(UNKNOWN_COMMAND,num_UART);}//{send_answer(SHUTTER_CLOSE,num_UART);}
	//Saving money to safe
	if (!memcmp (com, "SMO",3)){send_answer(UNKNOWN_COMMAND,num_UART);}//{send_answer(SAVING_RUN,num_UART);}
	//Temperature sensor
	if (!memcmp (com, "RT1",3)){send_answer(READ_TEMP_1,num_UART);}
	if (!memcmp (com, "RT2",3)){send_answer(UNKNOWN_COMMAND,num_UART);}//{send_answer(READ_TEMP_2,num_UART);}	
	if (!memcmp (com, "RTA",3)){send_answer(UNKNOWN_COMMAND,num_UART);}//{send_answer(READ_TEMP_ALL,num_UART);}
	//Read number RFID card 125kHz
	if (!memcmp (com, "RC1",3)){send_answer(READ_CARD_1,num_UART);}	
	//Diagnostics
	if (!memcmp (com, "DIR",3)){send_answer(UNKNOWN_COMMAND,num_UART);}//{send_answer(DIAGNOCTICS_RUN,num_UART);}	
	if (!memcmp (com, "DIS",3)){send_answer(UNKNOWN_COMMAND,num_UART);}//{send_answer(DIAGNOCTICS_STOP,num_UART);}		
	//Reset
	if (!memcmp (com, "RES",3)){send_answer(RESET_CONTROLLER,num_UART);}
	//Read shutter state/position
	if (!memcmp (com, "SS1",3)){send_answer(UNKNOWN_COMMAND,num_UART);}//{send_answer(READ_STATUS_SHUTTER,num_UART);} 
	if (!memcmp (com, "PS1",3)){send_answer(UNKNOWN_COMMAND,num_UART);}//{send_answer(READ_POSITION_SHUTTER,num_UART);}
	//Read saving state
	if (!memcmp (com, "SMS",3)){send_answer(UNKNOWN_COMMAND,num_UART);}//{send_answer(READ_STATUS_SAVING,num_UART);}
	//Read door recovery state
	if (!memcmp (com, "SD1",3)){send_answer(UNKNOWN_COMMAND,num_UART);}//{send_answer(READ_STATUS_RECOVERY_DOOR,num_UART);}	
	//Get/set time
	if (!memcmp (com, "GTI",3)){send_answer(GET_TIME,num_UART);}	
	
	if (!memcmp (com, "RPS",3)){send_answer(READ_POWER_STATUS,num_UART);}	
	
	if (!memcmp (com, "ATP",3)){send_answer(ACTIVATED_THERMAL_PRINTER,num_UART);}	
	if (!memcmp (com, "DTP",3)){send_answer(DEACTIVATED_THERMAL_PRINTER,num_UART);}	
	if (!memcmp (com, "APT",3)){send_answer(UNKNOWN_COMMAND,num_UART);}//{send_answer(ACTIVATED_POS_TERMINAL,num_UART);}	
	if (!memcmp (com, "DPT",3)){send_answer(UNKNOWN_COMMAND,num_UART);}//{send_answer(DEACTIVATED_POS_TERMINAL,num_UART);}	
	
	if (strstr (com, "STI")) //{send_answer(UNKNOWN_COMMAND,num_UART);}
	{
		//Заносим считанное время в структуру
		if(num_UART == 1)
		 {
			SystemTime.Day=(buf_rec_adm1[8]-0x30)*10+(buf_rec_adm1[9]-0x30);//Day
			SystemTime.Month=(buf_rec_adm1[11]-0x30)*10+(buf_rec_adm1[12]-0x30);//Month
			SystemTime.Year=(buf_rec_adm1[14]-0x30)*1000+(buf_rec_adm1[15]-0x30)*100+(buf_rec_adm1[16]-0x30)*10+(buf_rec_adm1[17]-0x30); //Year
			SystemTime.H=(buf_rec_adm1[19]-0x30)*10+(buf_rec_adm1[20]-0x30);//H
			SystemTime.M=(buf_rec_adm1[22]-0x30)*10+(buf_rec_adm1[23]-0x30);//M
			SystemTime.S=(buf_rec_adm1[25]-0x30)*10+(buf_rec_adm1[26]-0x30);//S
		 }

		if(num_UART == 4)
		 {
			SystemTime.Day=(buf_rec_adm4[8]-0x30)*10+(buf_rec_adm4[9]-0x30);//Day
			SystemTime.Month=(buf_rec_adm4[11]-0x30)*10+(buf_rec_adm4[12]-0x30);//Month
			SystemTime.Year=(buf_rec_adm4[14]-0x30)*1000+(buf_rec_adm4[15]-0x30)*100+(buf_rec_adm4[16]-0x30)*10+(buf_rec_adm4[17]-0x30); //Year
			SystemTime.H=(buf_rec_adm4[19]-0x30)*10+(buf_rec_adm4[20]-0x30);//H
			SystemTime.M=(buf_rec_adm4[22]-0x30)*10+(buf_rec_adm4[23]-0x30);//M
			SystemTime.S=(buf_rec_adm4[25]-0x30)*10+(buf_rec_adm4[26]-0x30);//S
			SetTimeDS();
		 }
		send_answer(SET_TIME,num_UART);
	}

	if(num_UART == 1) { clear_buf_rec_adm(1);	}	
	if(num_UART == 4) { clear_buf_rec_adm(4);	}	
}

void send_answer (unsigned char command, int num_UART)
{
	unsigned char crc=0;
	unsigned int len=0,i=0;
	char buf_short[20];
	int type=0;
	int flag_log=0;
	char buf_for_voltage[8];
	char buf_send_adm[100];
	char buf_for_temperature[8];
	
	delay_ms(1);
	
	for(i=0; i<100; i++) {buf_send_adm[i]=0; }
//	clear_buf_send_adm();
	//Пропишем начало сообщения
	buf_send_adm[0]=0x02;
	//Пропишем нулевую длинну сообщения в буфер
	buf_send_adm[1]=0x30;
	buf_send_adm[2]=0x30;
	buf_send_adm[3]=0x30;
		 	
		switch(command)
		{
/**/		case CHECK_CONNECTION:
					strcat(buf_send_adm,"CON");
				break;

/**/		case CHECK_VERSION:
					strcat(buf_send_adm,"VER:");
					strcat(buf_send_adm,SW_VERSION);
				break;

/**/		case CHECK_HW_VERSION:
					strcat(buf_send_adm,"VEH:");
					strcat(buf_send_adm,HW_VERSION);
					if(ADM40100 != 0) {strcat(buf_send_adm,"-1");}  //Если конфигурация ADM100, то в конце будет -1
					else							{strcat(buf_send_adm,"-4");}  //Если конфигурация ADM40, то в конце будет -4
				break;

/**/		case READ_DAT_1:
					strcat(buf_send_adm,"RD1:");
					if(trig_door_sensor != 0){strcat(buf_send_adm,"OPEN");}
					else{strcat(buf_send_adm,"CLOSE");}
				break;

/**/		case READ_DAT_2:
					strcat(buf_send_adm,"RD2:");
					if(trig_service_sensor !=0 ){strcat(buf_send_adm,"OPEN");}
					else{strcat(buf_send_adm,"CLOSE");}
				break;

/**/		case READ_DAT_3:
					strcat(buf_send_adm,"RD3:");
					if(trig_bag_sensor != 0){strcat(buf_send_adm,"OPEN");}
					else{strcat(buf_send_adm,"CLOSE");}
				break;

/**/		case READ_DAT_4:
					strcat(buf_send_adm,"RD4:");
					if(trig_rigel_sensor !=0){strcat(buf_send_adm,"OPEN");}
					else{strcat(buf_send_adm,"CLOSE");}
				break;

/**/		case READ_DAT_5:
					strcat(buf_send_adm,"RD5:");
					if(trig_validator_sensor){strcat(buf_send_adm,"OPEN");}
					else{strcat(buf_send_adm,"CLOSE");}
				break;

/**/		case READ_DAT_6:
					if(ADM40100 != 0)
						{ strcat(buf_send_adm,"ERR:UNKNOWN COMMAND"); }
					else
						{
							strcat(buf_send_adm,"RD6:");
							if(trig_cover_sensor){strcat(buf_send_adm,"OPEN");}
							else{strcat(buf_send_adm,"CLOSE");}
						}
				break;

/**/		case READ_DAT_ALL:
					strcat(buf_send_adm,"RDA:");
					if(trig_door_sensor !=0){strcat(buf_send_adm,"OPEN");}
					else{strcat(buf_send_adm,"CLOSE");}
					strcat(buf_send_adm,";");
				
					if(trig_service_sensor !=0){strcat(buf_send_adm,"OPEN");}
					else{strcat(buf_send_adm,"CLOSE");}
					strcat(buf_send_adm,";");

					if(trig_bag_sensor !=0){strcat(buf_send_adm,"OPEN");}
					else{strcat(buf_send_adm,"CLOSE");}
					strcat(buf_send_adm,";");

					if(trig_rigel_sensor !=0){strcat(buf_send_adm,"OPEN");}
					else{strcat(buf_send_adm,"CLOSE");}
					strcat(buf_send_adm,";");

					if(trig_validator_sensor){strcat(buf_send_adm,"OPEN");}
					else{strcat(buf_send_adm,"CLOSE");}

					if(ADM40100 == 0)
						{
							strcat(buf_send_adm,";");
							if(trig_cover_sensor){strcat(buf_send_adm,"OPEN");}
							else{strcat(buf_send_adm,"CLOSE");}
						}
				break;

/**/		case ACTIVATED_HIGH_1:
					if(ADM40100 == 0) { VALIDATOR(1); }
					else 							{ VALIDATOR_ON(1); }
					strcat(buf_send_adm,"AH1");
				break;

/**/		case DEACTIVATED_HIGH_1:
					if(ADM40100 == 0) { VALIDATOR(0); }
					else 							{ VALIDATOR_ON(0); }
					strcat(buf_send_adm,"DH1");
				break;

/**/		case ACTIVATED_HIGH_2:
					if(ADM40100 == 0) { strcat(buf_send_adm,"ERR:UNKNOWN COMMAND"); }
					else 							{ strcat(buf_send_adm,"ERR:UNKNOWN COMMAND"); }
			
//				Router_ON;
//				strcat(buf_send_adm,"AH2");
				break;

/**/		case DEACTIVATED_HIGH_2:
					if(ADM40100 == 0) { strcat(buf_send_adm,"ERR:UNKNOWN COMMAND"); }
					else 							{ strcat(buf_send_adm,"ERR:UNKNOWN COMMAND"); }
			
//					Router_OFF;
//					strcat(buf_send_adm,"DH2");
				break;

/**/		case ACTIVATED_SOLENOID_1:
					if(ADM40100 == 0) { strcat(buf_send_adm,"ERR:UNKNOWN COMMAND"); }
					else 							{ strcat(buf_send_adm,"ERR:UNKNOWN COMMAND"); }
				//ShutterSolenoid(1);		
				//strcat(buf_send_adm,"AS1");
			break;

/**/	case DEACTIVATED_SOLENOID_1:
					if(ADM40100 == 0) { strcat(buf_send_adm,"ERR:UNKNOWN COMMAND"); }
					else 							{ strcat(buf_send_adm,"ERR:UNKNOWN COMMAND"); }
				//ShutterSolenoid(0);
				//strcat(buf_send_adm,"DS1");
				break;

/**/	case ACTIVATED_ESCROW_LED:
					//if(ADM40100 == 0) { strcat(buf_send_adm,"ERR:UNKNOWN COMMAND"); }
					//else 							{ LED_HOPPER(1); strcat(buf_send_adm,"AEL"); }
				LED_HOPPER(1);
				strcat(buf_send_adm,"AEL");
			break;

/**/	case DEACTIVATED_ESCROW_LED:
					//if(ADM40100 == 0) { strcat(buf_send_adm,"ERR:UNKNOWN COMMAND"); }
					//else 							{ LED_HOPPER(0); strcat(buf_send_adm,"DEL"); }
				LED_HOPPER(0);
				strcat(buf_send_adm,"DEL");
			break;

/**/	case ACTIVATED_LOCK_1:
				LOCK(1);
				strcat(buf_send_adm,"AL1");
			break;

/**/	case DEACTIVATED_LOCK_1:
				LOCK(0);
				strcat(buf_send_adm,"DL1");
			break;

/*-*/	case READ_OPT_1:
				//strcat(buf_send_adm,"RO1:");
			break;

/*-*/	case READ_OPT_2:                            
				//strcat(buf_send_adm,"RO2:");
			break;				

/*-*/	case READ_OPT_3:
				//strcat(buf_send_adm,"RO3:");
			break;

/*-*/	case READ_OPT_4:
				//strcat(buf_send_adm,"RO4:");
			break;

/*-*/	case READ_OPT_5:
				//strcat(buf_send_adm,"RO5:");
			break;

/*-*/	case READ_OPT_6:
				//strcat(buf_send_adm,"RO6:");
			break;

/*-*/	case READ_OPT_7:
				//strcat(buf_send_adm,"RO7:");
			break;

/*-*/	case READ_OPT_8:
				//strcat(buf_send_adm,"RO8:");
			break;

/*-*/	case READ_OPT_9:
				//strcat(buf_send_adm,"RO9:");
			break;

/*-*/	case READ_OPT_10:
				//strcat(buf_send_adm,"O10:");
			break;

/*-*/	case READ_OPT_ALL:
				//strcat(buf_send_adm,"ROA:");
			break;

/**/	case READ_STATUS:
				strcat(buf_send_adm,"RST:READY");		
				//WriteStatus(status,buf_send_adm);		
			break;

/*-*/	case READ_STATUS_SAVING: 
				//strcat(buf_send_adm,"SMS:");	
				//WriteStatus(saving_state,buf_send_adm);
			break;

/*-*/	case READ_STATUS_SHUTTER:
				//strcat(buf_send_adm,"SS1:");
				//WriteStatus(shutter_state,buf_send_adm);
			break;

/*-*/	case READ_POSITION_SHUTTER:
				//strcat(buf_send_adm,"PS1:");
				//WriteShutterPosition();
			break;

/*-*/	case CLEAR_STATUS:
				//strcat(buf_send_adm,"CST");
				//status=READY;
			break;				

/*-*/	case SAVING_RUN:  
				//strcat(buf_send_adm,"SMO");
			break;

/*-*/	case SHUTTER_OPEN: 
				//strcat(buf_send_adm,"OS1");
				//shutter_run=1;
			break;		

/*-*/	case SHUTTER_CLOSE: 
				//strcat(buf_send_adm,"CS1");
				//shutter_run=2;
			break;	

/**/			case READ_TEMP_1:  
				get_temperature(buf_for_temperature);				
				strcat(buf_send_adm,"RT1:");
				strcat(buf_send_adm,buf_for_temperature);
			break;

/*-*/	case READ_TEMP_2:  
				//strcat(buf_send_adm,"RT2:");
				//read_tem_to_buffer(2);
			break;	

/*-*/	case READ_TEMP_ALL:
				//strcat(buf_send_adm,"RTA:");
				//read_tem_to_buffer(1);
				//strcat(buf_send_adm,";");
				//read_tem_to_buffer(2);
			break;

/**/	case READ_CARD_1:  
				if(ADM40100 == 0) { strcat(buf_send_adm,"ERR:UNKNOWN COMMAND"); }
				else
					{
						strcat(buf_send_adm,"RC1:");
						i=getCardNumber();
						if(i==0){strcat(buf_send_adm,"NO CARD");}
						else
							{
								sprintf(buf_short,"%X",i);		
								strcat(buf_send_adm,buf_short);	
							}
					}
			break;

/**/	case NOTIFY_READ_CARD_1:
				if(ADM40100 == 0) { return; }			//Если вдруг произошло событие на ADM40, то ничего не делать
				strcat(buf_send_adm,"NTF:RC1;");
				i=getCardNumber();
				if(i==0){return;}
				sprintf(buf_short,"%X",i);		
				strcat(buf_send_adm,buf_short);	
			break;				

/**/	case NOTIFY_READ_DAT_1_2:
				strcat(buf_send_adm,"NTF:RDA;");
				if(trig_door_sensor      != 0) {strcat(buf_send_adm,"OPEN;");} else {strcat(buf_send_adm,"CLOSE;");}
				if(trig_service_sensor   != 0) {strcat(buf_send_adm,"OPEN;");} else {strcat(buf_send_adm,"CLOSE;");}
				if(trig_bag_sensor       != 0) {strcat(buf_send_adm,"OPEN;");} else {strcat(buf_send_adm,"CLOSE;");}
				if(trig_rigel_sensor     != 0) {strcat(buf_send_adm,"OPEN;");} else {strcat(buf_send_adm,"CLOSE;");}
				if(trig_validator_sensor != 0) {strcat(buf_send_adm,"OPEN" );} else {strcat(buf_send_adm,"CLOSE" );}

				if(ADM40100 == 0)
					{
						if(trig_cover_sensor != 0) {strcat(buf_send_adm,";OPEN" );} else {strcat(buf_send_adm,";CLOSE" );}
					}
			break;		

/**/	case NOTIFY_READ_POWER_STATUS:
				if(ADM40100 != 0) { return; }
				voltage_to_string(buf_for_voltage);
				if(flag_low_battery == 0) {strcat(buf_send_adm,"NTF:RPS:AC/DC");}	//Система питается от сети
				else									    {strcat(buf_send_adm,"NTF:RPS:BAT");strcat(buf_send_adm,buf_for_voltage);}
			break;		

/**/	case NOTIFY_EXTRA_SHUTDOWN:
				if(ADM40100 != 0) { return; }
				strcat(buf_send_adm,"NTF:RPS:BATSHD");
			break;		

	case NOTIFY_PRESS_BUTTON:
				if(ADM40100 != 0) { return; }
				strcat(buf_send_adm,"NTF:BUTTON");
			break;

/**/	case 	RESET_CONTROLLER:
				strcat(buf_send_adm,"RES");
				reset_run=1;
			break;

/**/	case 	SHUTDOWN_CONTROLLER:
				if(ADM40100 == 0)
					{
						strcat(buf_send_adm,"SHD");
//				if(flag_low_battery != 0) {	shutdown_delay = DELAY_COMMAND_SHUTDOWN;	shutdown_no_stop = 1;}	//Запуск задержки выключения только при питании от батареи
						shutdown_delay = DELAY_COMMAND_SHUTDOWN;			//Запуск задержки выключения только при питании от батареи
					}
				else
					{strcat(buf_send_adm,"ERR:UNKNOWN COMMAND");}
			break;

/**/	case 	ACTIVATED_PRINTER_LIGHT:
				//if(ADM40100 == 0){strcat(buf_send_adm,"ERR:UNKNOWN COMMAND");}
				//else
				//	{
						strcat(buf_send_adm,"APL");
						LED_PRINTER(1);
				//	}
			break;			
		
/**/	case 	DEACTIVATED_PRINTER_LIGHT:
				//if(ADM40100 == 0){strcat(buf_send_adm,"ERR:UNKNOWN COMMAND");}
				//else
				//	{
						strcat(buf_send_adm,"DPL");
						LED_PRINTER(0);
				//	}
			break;			

/**/	case 	ACTIVATED_MONITOR_POWER:
				strcat(buf_send_adm,"AMP");
				//MON_POWER(1);
			break;			

/**/	case 	DEACTIVATED_MONITOR_POWER:
				strcat(buf_send_adm,"DMP");
				//MON_POWER(0);
			break;		

/**/	case 	PRESS_MONITOR_BUTTON:
				strcat(buf_send_adm,"PMB");
				MONITOR_BUTTON(1);		//"Нажали" кнопку
				monitor_button_delay=DELAY_MONITOR_BUTTON;	//"Отпустим" кнопку в прерывании по таймеру
			break;			

/*-*/	case ACTIVATED_LOCK_2:
				//strcat(buf_send_adm,"AL2");
			break;

/*-*/	case DEACTIVATED_LOCK_2:
				//strcat(buf_send_adm,"DL2");
			break;

/*-*/	case READ_STATUS_RECOVERY_DOOR:
				//strcat(buf_send_adm,"SD1:");
			break;
		
/**/	case GET_TIME:
				strcat(buf_send_adm,"GTI:");
				GetTime(0);	     
				sprintf(buf_short,"%02d.%02d.%04d_%02d:%02d:%02d",SystemTime.Day,SystemTime.Month,SystemTime.Year,SystemTime.H,SystemTime.M,SystemTime.S);
				strcat(buf_send_adm,buf_short);
			break;

/**/	case SET_TIME:
				strcat(buf_send_adm,"STI");
			break;

				case READ_POWER_STATUS:
					if(ADM40100 != 0)
						{
							strcat(buf_send_adm,"RPS:AC/DC");
						}
					else
						{
							voltage_to_string(buf_for_voltage);
							if(flag_low_battery == 0) {strcat(buf_send_adm,"RPS:AC/DC");}	//Система питается от сети
							else								     	{strcat(buf_send_adm,"RPS:BAT");strcat(buf_send_adm,buf_for_voltage);}
						}
			break;	

/**/	case ACTIVATED_THERMAL_PRINTER:
				PRINTER_POWER(1);
				strcat(buf_send_adm,"ATP");
			break;	

/**/	case DEACTIVATED_THERMAL_PRINTER:
				PRINTER_POWER(0);
				strcat(buf_send_adm,"DTP");
			break;	

/*-*/	case ACTIVATED_POS_TERMINAL:
				strcat(buf_send_adm,"ERR:UNKNOWN COMMAND");
			//strcat(buf_send_adm,"APT");
			break;	

/*-*/	case DEACTIVATED_POS_TERMINAL:
				strcat(buf_send_adm,"ERR:UNKNOWN COMMAND");
//	strcat(buf_send_adm,"DPT");
			break;	

			case  ACTIVATED_CARDREADER: 
				CARD_READER_POWER(1);
				strcat(buf_send_adm,"ACR");
				break;
			case DEACTIVATED_CARDREADER:
				CARD_READER_POWER(0);
				strcat(buf_send_adm,"DCR");
				break;
			case ACTIVATED_PC:
				COMPUTER_POWER(1);
				strcat(buf_send_adm,"APC");
				break;
			case DEACTIVATED_PC:
				COMPUTER_POWER(0);
				strcat(buf_send_adm,"DPC");
				break;
			case ACTIVATED_RESERVE_POWER:
				RESERVE_POWER(1);
				strcat(buf_send_adm,"ARP");
				break;
			case DEACTIVATED_RESERVE_POWER:
				RESERVE_POWER(0);
				strcat(buf_send_adm,"DRP");
				break;

			default:	
			strcat(buf_send_adm,"ERR:UNKNOWN COMMAND"); 	
				break;
		}
	
		//узнаем длину сообщения	
		len=write_len(buf_send_adm);
		//Пропишем длину сообщения в буфер
		buf_send_adm[1]=len/100+0x30;
		buf_send_adm[2]=len%100/10+0x30;
		buf_send_adm[3]=len%100%10+0x30;
		//Пропишем конец сообщения
		buf_send_adm[len+6]=0x03;
		//подсчет контрольной суммы сообщения	
		for(i=1;i<=len+3;i++)
		{crc=crc^buf_send_adm[i];}
		//запись контрольной суммы в буфер отправки
		i=htoa(crc);
		buf_send_adm[len+4]=i>>8;
		buf_send_adm[len+5]=i; 
	
		//отправка сообщения на порт контроллера АДМ
		if(num_UART == 0)
			{
				UART_SendBytes(buf_send_adm, len+6, 1);
				UART_SendBytes(buf_send_adm, len+6, 4);
			}
		else
			{
				UART_SendBytes(buf_send_adm, len+6, num_UART);
			}

		buf_send_adm[len+4]=0;
		if(flag_log==1){SaveLog(type,buf_send_adm+4);}
//		clear_buf_send_adm();	
	
	//Если есть флаг сброса, то делаем сброс контроллера
	if(reset_run==1)
		{
			reset_run=0;
			NVIC_SystemReset();
		}
}

//запишем нужный статус в буфер 
void WriteStatus (unsigned int status, char * buf)
{
	switch(status)
	{
		case READY: 
			strcat(buf,"READY"); 
			break;
		case SAVING_IS_RUN:
			strcat(buf,"SAVING IS RUN");
			break;
		case SAVING_IS_DONE:
			strcat(buf,"SAVING IS DONE");
			break;
		case SAVING_IS_RECOVERING:
			strcat(buf,"SAVING IS RECOVERING");
			break;
		case SHUTTER_IS_RUN:
			strcat(buf,"SHUTTER IS RUN");
			break;
		case SHUTTER_IS_OPEN:
			strcat(buf,"SHUTTER IS OPEN");
			break;
		case SHUTTER_IS_CLOSE:
			strcat(buf,"SHUTTER IS CLOSE");
			break;
		case SHUTTER_IS_RECOVERING:
			strcat(buf,"SHUTTER IS RECOVERING");
			break;
		case ERROR_MOTOR1_JAM:
			strcat(buf,"ERROR MOTOR 1 JAM");
			break;
		case ERROR_MOTOR1_SCROLL:
			strcat(buf,"ERROR MOTOR 1 SCROLL");
			break;
		case ERROR_MOTOR1_ABSENT:
			strcat(buf,"ERROR MOTOR 1 ABSENT");
			break;
		case ERROR_MOTOR2_JAM:
			strcat(buf,"ERROR MOTOR 2 JAM");
			break;
		case ERROR_MOTOR2_SCROLL:
			strcat(buf,"ERROR MOTOR 2 SCROLL");
			break;
		case ERROR_MOTOR2_ABSENT:
			strcat(buf,"ERROR MOTOR 2 ABSENT");
			break;
		case ERROR_MOTOR3_JAM:
			strcat(buf,"ERROR MOTOR 3 JAM");
			break;
		case ERROR_MOTOR3_SCROLL:
			strcat(buf,"ERROR MOTOR 3 SCROLL");
			break;
		case ERROR_MOTOR3_ABSENT:
			strcat(buf,"ERROR MOTOR 3 ABSENT");
			break;
		case ERROR_SENSOR1_JAM:
			strcat(buf,"ERROR SENSOR 1 JAM");
			break;	
		case ERROR_SENSOR2_JAM:
			strcat(buf,"ERROR SENSOR 2 JAM");
			break;
		case ERROR_SENSOR3_JAM:
			strcat(buf,"ERROR SENSOR 3 JAM");
			break;
		case RECOVERY_DOOR_OPEN:
			strcat(buf,"RECOVERY DOOR IS OPEN");
			break;
		case RECOVERY_DOOR_CLOSE:
			strcat(buf,"RECOVERY DOOR IS CLOSE");
			break;
		default:
			strcat(buf,"UNKNOWN");
			break;
	}
}


//Чтение состояния температурного датчика и запись в буффер buf_short в формате +28,5
//sensor=1 - 1 датчик, =2 - 2 датчик
void read_tem_to_buffer (unsigned char channel)
{
/*	
	unsigned int tmp=0;
	unsigned short temp1=0,temp2=0;
	char buf_short[10];
	///получаем значение температуры
	//tmp=getTemperature_DS19B20(channel);

      	//проверка подключен ли датчик
	if(tmp==0xFFFFFFFF){
		strcat(buf_send_adm,"FAULTY");
		return;
		}
	//проверка не 0 градусов ли
	if(tmp==0){
		strcat(buf_send_adm,"0.0");
		return;
		}
	//проверка положительная ли температура
	if (((tmp>>8)&128) == 0){
		//целая часть температуры
		temp1 = (((tmp>>8)&7)<<4)|(tmp>>4);
		//дробная часть температуры
		temp2 = (tmp&15);
		temp2 = (temp2<<1) + (temp2<<3);
		temp2 = (temp2>>4);
		sprintf(buf_short,"+%d.%d",temp1,temp2);			
		}
	else{ //если температура отрицательная
		
		tmp = ~tmp + 1;
		tmp=tmp&0x0000FFFF;
		//целая часть температуры
		temp1 = (((tmp>>8)&7)<<4)|(tmp>>4);
		//дробная часть температуры
		temp2 = (tmp&15);
		temp2 = (temp2<<1) + (temp2<<3);
		temp2 = (temp2>>4);
		sprintf(buf_short,"-%d.%d",temp1,temp2);
	}
	strcat(buf_send_adm,buf_short);
*/
}

//Подсчет длины поля data в сообщении
unsigned char write_len(char *buf_s_adm)
{
	unsigned int i=0;
	unsigned char len=0;
	
	for(i=0;i<100;i++)
	{
		if(buf_s_adm[i]==0){break;}
		len++;
	}
	
return len-4;	
}


//Перевод из шестнадцатиричного числа в два ascii, но вывод через int
//0xA5 - 0x4135 (0x41-'A',0x35-'5')
unsigned int htoa (unsigned char dat)
{
	unsigned char a=0,b=0;
	
	a=((dat & 0xF0)>>4);
	if (a>0x09) {a=a+0x37;}
	else {a=a+0x30;}

	b=(dat & 0x0F);
	if (b>0x09) {b=b+0x37;}
	else {b=b+0x30;}

	return (a<<8)+b;
}

//Перевод из двух ascii в одно шестадцатиричное
//'A''5' в 0хA5
unsigned char atoh (unsigned char x,unsigned char y)
{

	if(x>'9'){x=(x-0x37)<<4;}
	else{x=(x-0x30)<<4;}

	if(y>'9'){y=y-0x37;}
	else{y=y-0x30;}

	return x+y;
}

/*
//Переводим напряжение в мВ в напряжение в вольтах и точку
void voltage_to_string(int v, char *str)
{
	int tmp;
	
	str[0]='0';str[1]='0';str[2]='.';str[3]='0';str[4]='0';str[5]=0;
	if((v<0)||(v>99000)) { return; }
	tmp=v;
	
	str[0]=tmp/10000+'0'; tmp=tmp % 10000;
	str[1]=tmp/1000+'0';  tmp=tmp % 1000;
	str[3]=tmp/100+'0';   tmp=tmp % 100;
	str[4]=tmp/10+'0';
}
*/
//Заносим в str строку в зависимости от напряжения
void voltage_to_string(char *str)
{
	if(flag_low_battery == 1) {str[0]='F';str[1]='U';str[2]='L';str[3]='L';str[4]=0;}
	if(flag_low_battery == 2) {str[0]='L';str[1]='O';str[2]='W';str[3]=0;  str[4]=0;}
	if(flag_low_battery == 3) {str[0]='S';str[1]='H';str[2]='D';str[3]=0;  str[4]=0;}
}

void repeat_message(void)
{
	if(ADM40100 != 0) { return; }
	repeat_message_time--;
	if(repeat_message_time > 0) { return; } //Работаем только 1 раз за REPEAT_MESSAGE_TIME проходов главного цикла
	repeat_message_time = REPEAT_MESSAGE_TIME;

//Проверим, не надо ли передать "NTF:RPS:BATSHD" и передадим, если надо
	if(shutdown_battery_delay > 0) { send_answer(NOTIFY_EXTRA_SHUTDOWN,0); }

	//Проверим, не надо ли передать "NTF:BUTTON"	
	if(repeat_button_message > 0)
	{
		send_answer(NOTIFY_PRESS_BUTTON,0);
		repeat_button_message--;
	}
}

