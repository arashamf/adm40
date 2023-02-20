//connection.h for connection with ADM_control by RS-232
extern char buf_rec_adm1[100];	//Буфер приема посылки UART1 от PC
extern char buf_rec_adm4[100];	//Буфер приема посылки UART4 от PC

extern unsigned int counter_buf_adm1;   //Счетчик числа принятых байт в буфере buf_rec1
extern unsigned int counter_buf_adm4;   //Счетчик числа принятых байт в буфере buf_rec4

extern char buf_send_adm1[100];	//Буфер отправки посылки через UART1 к PC
extern char buf_send_adm4[100];	//Буфер отправки посылки через UART4 к PC

//extern char buf_rec_adm[100];	//Буфер приема посылки UART0 от PC
//extern unsigned int counter_buf_adm;     //Счетчик числа принятых байт в буфере buf_rec
//extern char buf_send_adm[100];	//Буфер отправки посылки через UART0 к PC

//extern volatile unsigned int flag_recieved;

extern unsigned int reset_run;

#define LENGTH_DATA_REC1 ((buf_rec_adm1[1]-0x30)*100+(buf_rec_adm1[2]-0x30)*10+(buf_rec_adm1[3]-0x30))
#define LENGTH_DATA_REC4 ((buf_rec_adm4[1]-0x30)*100+(buf_rec_adm4[2]-0x30)*10+(buf_rec_adm4[3]-0x30))

//FIFO1
#define INPUT_BUFFER_SIZE 1000								// ?acia? i?eaiiiai aooa?a, a aaeoao

extern unsigned char inputBuf[INPUT_BUFFER_SIZE];	// буфер FIFO для хранения полученных по UART данных. 
extern volatile unsigned int pFifo;					  // указатель на текущее местоположение принятого по UART байта
extern volatile unsigned int lenFifo;				  // текущий размер принятого по UART, но еще не выведенного на печать, текста

unsigned char putFifo(int num_UART, char byte);
unsigned char getFifo(int num_UART);

void check_message (int num_COM);
void clear_buf_rec_adm(int num_COM);
//void clear_buf_send_adm(int num_COM);
//void send_answer (unsigned char command);
void send_answer (unsigned char command, int num_UART);
unsigned char write_len (char *buf_s_adm);
unsigned int htoa (unsigned char dat);
unsigned char atoh (unsigned char x,unsigned char y);
void read_tem_to_buffer (unsigned char channel);
void recieve_command (void);
void check_uarts(void);
void WriteShutterPosition (void);
void WriteStatus (unsigned int status, char *buf);
void repeat_message(void);

//commands
enum {
CHECK_PORTS,
CHECK_CONNECTION,
CHECK_VERSION,
CHECK_HW_VERSION,
STOP_MOTOR_1, 
FORWARD_MOTOR_1, 
REVERSE_MOTOR_1, 
LOAD_MOTOR_1, 
STOP_MOTOR_2, 
FORWARD_MOTOR_2, 
REVERSE_MOTOR_2, 
LOAD_MOTOR_2, 
STOP_MOTOR_3, 
FORWARD_MOTOR_3, 
REVERSE_MOTOR_3, 
LOAD_MOTOR_3, 
READ_DAT_1, 
READ_DAT_2, 
READ_DAT_3, 
READ_DAT_4,
READ_DAT_5,
READ_DAT_6,
READ_DAT_ALL,
ACTIVATED_HIGH_1, 
DEACTIVATED_HIGH_1, 
ACTIVATED_HIGH_2, 
DEACTIVATED_HIGH_2,
ACTIVATED_SOLENOID_1, 
DEACTIVATED_SOLENOID_1, 
ACTIVATED_LOCK_1, 
DEACTIVATED_LOCK_1,
ACTIVATED_ESCROW_LED,
DEACTIVATED_ESCROW_LED,
ACTIVATED_PRINTER_LIGHT,
DEACTIVATED_PRINTER_LIGHT,
ACTIVATED_MONITOR_POWER,
DEACTIVATED_MONITOR_POWER,
PRESS_MONITOR_BUTTON,
READ_OPT_1, 
READ_OPT_2, 
READ_OPT_3, 
READ_OPT_4, 
READ_OPT_5, 
READ_OPT_6,
READ_OPT_7,
READ_OPT_8,
READ_OPT_9,
READ_OPT_ALL, 
READ_STATUS, 
CLEAR_STATUS,
SAVING_RUN,
SHUTTER_OPEN,
SHUTTER_CLOSE,
READ_TEMP_1,
READ_TEMP_2,
READ_TEMP_ALL,
READ_CARD_1,
NOTIFY_READ_CARD_1,
NOTIFY_READ_DAT_1_2,
DIAGNOCTICS_RUN,
DIAGNOCTICS_STOP,
RESET_CONTROLLER,
READ_STATUS_SHUTTER, 
READ_STATUS_SAVING,
READ_POSITION_SHUTTER,
READ_OPT_10,
ACTIVATED_LOCK_2, 
DEACTIVATED_LOCK_2,
READ_STATUS_RECOVERY_DOOR,
GET_TIME,
SET_TIME,
READ_POWER_STATUS,
ACTIVATED_THERMAL_PRINTER,
DEACTIVATED_THERMAL_PRINTER,
ACTIVATED_POS_TERMINAL,
DEACTIVATED_POS_TERMINAL,
NOTIFY_READ_POWER_STATUS,
NOTIFY_EXTRA_SHUTDOWN,
NOTIFY_PRESS_BUTTON,
SHUTDOWN_CONTROLLER,
ACTIVATED_CARDREADER,
DEACTIVATED_CARDREADER,
ACTIVATED_PC,
DEACTIVATED_PC,
ACTIVATED_RESERVE_POWER,
DEACTIVATED_RESERVE_POWER,
UNKNOWN_COMMAND
};

//States of controller
enum {
READY,
SAVING_IS_RUN,
SAVING_IS_RECOVERING,
SAVING_IS_DONE,
SHUTTER_IS_RUN,
SHUTTER_IS_RECOVERING,
SHUTTER_IS_OPEN,
SHUTTER_IS_CLOSE,
ERROR_MOTOR1_JAM,
ERROR_MOTOR1_SCROLL,
ERROR_MOTOR2_JAM,
ERROR_MOTOR2_SCROLL,
ERROR_MOTOR3_JAM,
ERROR_MOTOR3_SCROLL,
ERROR_MOTOR1_ABSENT,
ERROR_MOTOR2_ABSENT,
ERROR_MOTOR3_ABSENT,
DIAGNOSTICS,
ERROR_SENSOR1_JAM,
ERROR_SENSOR2_JAM,
ERROR_SENSOR3_JAM,
ERROR_SOLENOID1_JAM,
RECOVERY_DOOR_OPEN,
RECOVERY_DOOR_CLOSE
};
