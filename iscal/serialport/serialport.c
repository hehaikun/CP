#include "serialport.h"
#include "stm32f10x_usart.h"
#include "usb_regs.h"
#include "usbio.h"
#include "keypad.h"
#include "usart.h"
#include "delay.h"

int MessageType;
uint8_t RxBuffer1[64];
int RxCounter1;
uint8_t PressN=0xff;
int ReCheck=0;
//------------------------------------------------------------ serial port 
void sendmessage(u8 Res) {
smstart:
    if(MessageType == 0) 
	{												//empty
        if(Res==0x57||Res==0x53||Res==0x50||Res==0x77||Res==0x59) 
		{
            MessageType = 0x57;
            RxBuffer1[RxCounter1] = Res;
            RxCounter1 = 1;
        }
    }
    else if(MessageType == 0x57) 					//STABLE ÎÈ¶¨µÄ
	{	
        if(RxCounter1 < 5) 							//RxCounter1>0&&
		{						
            RxBuffer1[RxCounter1++] = Res;
            if(RxCounter1==2) 
			{
//                HEARTBEAT_LED = LED_ON;
                if(Res!=0xff) 
				{
                    if(RxBuffer1[0]==0x57||RxBuffer1[0]==0x53||Res==0x59)
                        PressN=Res;
                }
            } else if(RxCounter1==5) 
			
			{
                //MessageType=0;
                MessageType = 0x01;
                if(ReCheck) 
				{
                    MessageType=0;
                    RxCounter1=0;
                    ReCheck=0;
                } else 
				{
                    if(PressN!=0xff) {
                        //HEARTBEAT_LED = LED_ON;
                        USART_SendData(USART1,0x12);
                    } else {
                        //HEARTBEAT_LED = LED_ON;
                        USB_SendData(ENDP1,RxBuffer1,5);
//                        HEARTBEAT_LED = LED_OFF;
                    }
                }

//				RxBuffer1[8]=0xf1;
//				RxBuffer1[40]=0xf1;
                //RxCounter1=0;
                //OSQPost(USBSentMsg
                //USB_SendData(ENDP1,RxBuffer1,64);

            }
        }
    }
    else if (MessageType==0x01) {

        if(RxCounter1==5) {
            if(Res==0x01) {
                RxBuffer1[5] = 0x01;
                MessageType=0x02;
                RxCounter1=6;
            } else {
                //if(Res==0x57||Res==0x53){
                MessageType=0;
                RxCounter1=0;
                ReCheck=1;
                goto smstart;
                //}
            }
        } else {

        }
    }
    else if (MessageType==0x02) {
        RxBuffer1[RxCounter1++] = Res;
        if(RxCounter1==42) {
            MessageType=0;
            RxCounter1=0;
            if((PressN!=0xff)&&(RxBuffer1[1]==0xff)) {
                RxBuffer1[1]=PressN;
                PressN=0xff;
            }
            USB_SendData(ENDP1,RxBuffer1,43);
//            HEARTBEAT_LED = LED_OFF;
            //USB_SendData(ENDP1,RxBuffer1,43);
            //OSQPost(MessageQ,RxBuffer1);
        }
    }



//	if(UartLengthCount==1){
//		if(MessageType == 0x01 && Res == 0x30){
//		 	USB_SendData(ENDP1,"I LOVE Y",8);
//		}
//	}else if(UartLengthCount==0&&Res==0x57){
//		UartLengthCount=6;
//    }

    //USB_SendData(ENDP1,Res,1);
}

//----------------------------------------------------------------
void rs232_log_task(void *pdata)
{
    void *lmsg;
    char *msg;
    INT8U err;

    pdata = pdata;
    while (1) {
        lmsg = OSQPend(RS232LogQ,0,&err);
        if (err == OS_ERR_NONE) {
            msg = lmsg;
            uart1_send_(msg,strlen(msg));
        }
        delay_ms(100);
    }
}



