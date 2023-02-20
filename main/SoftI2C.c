
#include "pins.h"
#include "gpio.h"
#include "timer.h"
#include "SoftI2C.h"

#define DI2C	delay_us(1); 

void I2C_SoftInit(void)
{
	
	GPIO_InitTypeDef GPIO_InitStruct;
	
  GPIO_InitStruct.Pin = I2C_SDA_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(I2C_SDA_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = I2C_SCL_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(I2C_SCL_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = RESET_I2C_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(RESET1_I2C_PORT, &GPIO_InitStruct);

  RESET1_I2C;	
}

void I2C_SoftStart(void)
{
	unsigned int tmp=0;
		SDA1;
		DI2C;
		SCL1;
		DI2C;
		while(!SDAin)
		{
		tmp++;if(tmp>100000){return;}
		SCL0;
		DI2C;
		SCL1;
		DI2C;
		}
		SDA0;
		DI2C;
		SCL0;
		DI2C;
}
void I2C_SoftStop(void)
{
		SDA0;
		DI2C;
		SCL1;
		DI2C;
		SDA1;
		DI2C;
}

unsigned char I2C_SoftWriteByte(unsigned char data)
{
	 unsigned char i;
	 unsigned char ACK;
	 for(i=0;i<8;i++)
	 {
	 if(data & 0x80)
	 {
	 SDA1;
	 }
	 else
	 {
	 SDA0;
	 }
	 DI2C;
	 SCL1;
	 DI2C;        
	 SCL0;
	 data=data<<1;    
	 }
	 DI2C;        
	 SCL1;
	 DI2C;        
	 ACK = !SDAin;
	 SCL0;
	 SDA0;
	 return ACK;
}

unsigned char I2C_SoftReadByte(unsigned char ACK)
{
		unsigned char i;
		unsigned char data;
		
		SDA1;
	for(i=0;i<8;i++)
		{
		DI2C;
		SCL1;
		DI2C;       
		data<<=1;
	if(SDAin)
		data++; 
		SCL0;
		}
	if (ACK)
		SDA0;
		DI2C;       
		SCL1;
		DI2C;       
		SCL0;
		SDA1;
		return data;
}

void reset_i2c(int time)
{
	RESET0_I2C;
 	delay_ms(time);
	RESET1_I2C;
}
