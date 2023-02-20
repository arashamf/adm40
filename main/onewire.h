//Prototype

extern unsigned int card_insert;

void OneWireWriteByte(unsigned char dat,unsigned char channel);
void do_crc_8(unsigned char byte, unsigned char *crc8); //Вычисляет контрольную сумму
unsigned int getCardNumber (void);
unsigned char OneWireReset(unsigned char channel);
unsigned char OneWireReadByte(unsigned char channel);
void setResolution_DS19B20 (unsigned char resolution,unsigned char channel);
unsigned int getTemperature_DS19B20 (unsigned char channel);
void check_card (void);

unsigned int getCardNumber_5 (void);


void OneWireDirOut (unsigned char channel);
void OneWireDirIn (unsigned char channel);
void OneWireOn (unsigned char channel);
void OneWireOff (unsigned char channel);
unsigned int OneWireLevel (unsigned char channel);
