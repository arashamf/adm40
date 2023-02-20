
void I2C_SoftInit(void);
void I2C_SoftStart(void);
void I2C_SoftStop(void);
unsigned char I2C_SoftWriteByte(unsigned char data);
unsigned char I2C_SoftReadByte(unsigned char ACK);
void reset_i2c(int time);
