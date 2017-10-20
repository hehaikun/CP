#ifdef		_SERIALPORT
#define 	_SERIALPORT

extern int MessageType;

extern uint8_t RxBuffer1[64];

extern int RxCounter1;

extern uint8_t PressN;
extern int ReCheck;

void sendmessage(u8 Res);
void rs232_log_task(void *pdata);

#endif

