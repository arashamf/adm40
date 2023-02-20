
void read_tem_to_buffer (unsigned char channel)
{
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
}
