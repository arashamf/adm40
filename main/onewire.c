// Library for onewire device
// CPU: LPC1788
// Author: Dyachenko Oleg
// Date: 17.04.13

#include "onewire.h"
#include "pins.h"
#include "gpio.h"
#include "timer.h"
#include "GlobalVars.h"
#include "connection.h"

// DS18B20 temperature sensor
/*#define OneWireDirOut {LPC_GPIO0->DIR |=  (1<<19);}			// 1-Wire output
#define OneWireDirIn	{LPC_GPIO0->DIR &= ~(1<<19);}			// 1-Wire input
#define OneWireOn  		{LPC_GPIO0->SET  = (1<<19);} 			// set 1
#define OneWireOff 		{LPC_GPIO0->CLR  = (1<<19);} 			// set 0
#define OneWireLevel (LPC_GPIO0->PIN & (1<<19))					// read 1-Wire
*/

unsigned int card_insert=0;



void OneWireDirOut (unsigned char channel)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
  GPIO_InitStruct.Pin = ONE_WIRE_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(ONE_WIRE_PORT, &GPIO_InitStruct);
}
void OneWireDirIn (unsigned char channel)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	GPIO_InitStruct.Pin = ONE_WIRE_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(ONE_WIRE_PORT, &GPIO_InitStruct);
}
void OneWireOn (unsigned char channel)
{
	ONE_WIRE(1);
}
void OneWireOff (unsigned char channel)
{
	ONE_WIRE(0);
}
unsigned int OneWireLevel (unsigned char channel)
{
return	(ONE_WIRE_READ);
}


const unsigned char crc_table[256] = {
	0x00,0x5E,0xBC,0xE2,0x61,0x3F,0xDD,0x83,0xC2,0x9C,0x7E,0x20,0xA3,0xFD,0x1F,0x41,
	0x9D,0xC3,0x21,0x7F,0xFC,0xA2,0x40,0x1E,0x5F,0x01,0xE3,0xBD,0x3E,0x60,0x82,0xDC,
	0x23,0x7D,0x9F,0xC1,0x42,0x1C,0xFE,0xA0,0xE1,0xBF,0x5D,0x03,0x80,0xDE,0x3C,0x62,
	0xBE,0xE0,0x02,0x5C,0xDF,0x81,0x63,0x3D,0x7C,0x22,0xC0,0x9E,0x1D,0x43,0xA1,0xFF,
	0x46,0x18,0xFA,0xA4,0x27,0x79,0x9B,0xC5,0x84,0xDA,0x38,0x66,0xE5,0xBB,0x59,0x07,
	0xDB,0x85,0x67,0x39,0xBA,0xE4,0x06,0x58,0x19,0x47,0xA5,0xFB,0x78,0x26,0xC4,0x9A,
	0x65,0x3B,0xD9,0x87,0x04,0x5A,0xB8,0xE6,0xA7,0xF9,0x1B,0x45,0xC6,0x98,0x7A,0x24,
	0xF8,0xA6,0x44,0x1A,0x99,0xC7,0x25,0x7B,0x3A,0x64,0x86,0xD8,0x5B,0x05,0xE7,0xB9,
	0x8C,0xD2,0x30,0x6E,0xED,0xB3,0x51,0x0F,0x4E,0x10,0xF2,0xAC,0x2F,0x71,0x93,0xCD,
	0x11,0x4F,0xAD,0xF3,0x70,0x2E,0xCC,0x92,0xD3,0x8D,0x6F,0x31,0xB2,0xEC,0x0E,0x50,
	0xAF,0xF1,0x13,0x4D,0xCE,0x90,0x72,0x2C,0x6D,0x33,0xD1,0x8F,0x0C,0x52,0xB0,0xEE,
	0x32,0x6C,0x8E,0xD0,0x53,0x0D,0xEF,0xB1,0xF0,0xAE,0x4C,0x12,0x91,0xCF,0x2D,0x73,
	0xCA,0x94,0x76,0x28,0xAB,0xF5,0x17,0x49,0x08,0x56,0xB4,0xEA,0x69,0x37,0xD5,0x8B,
	0x57,0x09,0xEB,0xB5,0x36,0x68,0x8A,0xD4,0x95,0xCB,0x29,0x77,0xF4,0xAA,0x48,0x16,
	0xE9,0xB7,0x55,0x0B,0x88,0xD6,0x34,0x6A,0x2B,0x75,0x97,0xC9,0x4A,0x14,0xF6,0xA8,
	0x74,0x2A,0xC8,0x96,0x15,0x4B,0xA9,0xF7,0xB6,0xE8,0x0A,0x54,0xD7,0x89,0x6B,0x35
};

/**********************************************************
Name:               unsigned char OneWireReset(unsigned char mask)
Description:        Send a reset on the buss,
Input:              mask : Where the device are on the port
Output:             0 -> Device not present
                    1 -> Device present
**********************************************************/
unsigned char OneWireReset(unsigned char channel)
{
	OneWireOff(channel);                               // Normal input no pull up
	OneWireDirOut(channel);                            // out at 0
	delay_us(500);
	OneWireDirIn(channel);                             // Set to input
	delay_us(70);
	if ((OneWireLevel(channel)) == 0)
	{
		delay_us(500);
		return 1;
	}
	delay_us(500);
	return 0;
}

/**********************************************************
Name:               void OneWireWriteByte(unsigned char mask,unsigned char data)
Description:        Write a byte on the OneWire buss,
Input:              mask : Where the device are on the port,
                    data : Data to write on the port
Output:             none
**********************************************************/
void OneWireWriteByte(unsigned char dat,unsigned char channel)
{
	unsigned char i;

	OneWireDirIn(channel);OneWireOff(channel);     	//input 0
	for (i=0;i<=7;i++)
	{
		OneWireDirOut(channel);  						//out0
		if (dat & 0x01) {
			delay_us(7);              // Send 1
			OneWireDirIn(channel);        			// input
			delay_us(70);
		}
		else {
			delay_us(70);            	// Send 0
			OneWireDirIn(channel);
			delay_us(7);
		}
		dat>>=1;
	}
}

/**********************************************************
Name:               unsigned char OneWireReadByte(unsigned char mask)
Description:        Read a byte on the OneWire buss,
Input:              mask : Where the device are on the port
Output:             data that had been read
**********************************************************/
unsigned char OneWireReadByte(unsigned char channel)
{
	unsigned char dat = 0;
	unsigned char i;

	OneWireDirIn(channel);OneWireOff(channel);                  // Output '0' or input without pullup
	for (i=0;i<=7;i++)	{
		OneWireDirOut(channel);                          // Set output to '0'
		delay_us(7);
		OneWireDirIn(channel);                           // Set to input
		delay_us(7);
		dat >>=1;
		if ((OneWireLevel(channel))) { dat |= 0x80; }
		else { dat &= 0x7f; }
		delay_us(70);
	}
	return dat;
}

//Подпрограмма опроса считывателя бесконтактных карт
//return 0 - карту считать не удалось, !=0 - ID карты в 32-битном представлении (байты с 1 по 4 включ.)
unsigned int getCardNumber (void)
{
	unsigned char crc8, i;
   
	if(OneWireReset(3)==1) {       

		OneWireWriteByte(0x33,3); 
		
		crc8 = 0;
		for (i=0;i<8;i++)
		{
			number_card[i]=OneWireReadByte(3); 
			crc8 = crc_table[(crc8^number_card[i])];
		}
		if(crc8==0) { return ((number_card[1]<<24)|(number_card[2]<<16)|(number_card[3]<<8)|number_card[4]);}       
	}
	
	return 0;  
}

unsigned int getCardNumber_5 (void)
{
	unsigned int i=0,tmp=0;
	for(i=0;i<5;i++)
	{
		tmp=getCardNumber();
		if(tmp==0){
			return 0;
		}
	}
	return tmp;
}



/*
//Программа установки точности преобразования температуры
//resolution:
//=0 - 9 bit, 93.75ms
//=1 - 10 bit, 187.5ms
//=2 - 11 bit, 375ms
//=3 - 12 bit, 750ms
void setResolution_DS19B20 (unsigned char resolution,unsigned char channel)
{
	if(OneWireReset(channel)) {
		OneWireWriteByte(0xCC,channel);		//пропуск идентификации
		OneWireWriteByte(0x4E,channel);		//команда записи в память
		OneWireWriteByte(0x00,channel);		//1 пользовательский байт в 0
		OneWireWriteByte(0x00,channel);		//2 пользовательский байт в 0
		
		if(resolution==0){OneWireWriteByte(0x1F,channel);}
		if(resolution==1){OneWireWriteByte(0x3F,channel);}	
		if(resolution==2){OneWireWriteByte(0x5F,channel);}
		if(resolution==3){OneWireWriteByte(0x7F,channel);}
		
	}
}



//Программа измерения температуры
//Возврат: 0x0000XXXX, где XXXX-два байта температуры, если 0xFFFFFFFF - ошибка
unsigned int getTemperature_DS19B20 (unsigned char channel)
{
	unsigned char crc8=0, i=0;
	unsigned char memory[9];		// Буфер для записи памяти температурного датчика
	
	if(OneWireReset(channel)) {
	
	OneWireWriteByte(0xCC,channel);		//пропуск идентификации
	OneWireWriteByte(0x44,channel);		//запуск преобразования температуры
		
	while(!OneWireLevel(channel)){}    //ожидание преобразования
	
	OneWireReset(channel);						//сброс
	OneWireWriteByte(0xCC,channel);		//пропуск идентификации
	OneWireWriteByte(0xBE,channel);		//чтение памяти
		
	crc8 = 0;
	for (i=0;i<9;i++)         //чтение памяти термодатчика
		{
			memory[i]=OneWireReadByte(channel); 
			crc8 = crc_table[(crc8^memory[i])];
		}
	
	if(crc8==0) {return ((memory[1]<<8)+(memory[0]));}  //проверка контрольной суммы
	else{return 0xFFFFFFFF;}	

	}
	
	return 0xFFFFFFFF;  //error
}
*/


//проверим поднесена ли карта
void check_card (void)
{	
	if(OneWireReset(3)){
		if(card_insert==0){
			if(getCardNumber_5()){
				card_insert=1;
				send_answer(NOTIFY_READ_CARD_1,0);
			}
		}
	}
	else{
		card_insert=0;
		number_card[1]=0;
		number_card[2]=0;
		number_card[3]=0;
		number_card[4]=0;
	}	
}
