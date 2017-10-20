#include "main.h"
#include "sys.h" 	
#include "delay.h"
#include "usart.h"
#include "hw_config.h"
#include "usbio.h"
#include "usb_lib.h"
#include "w25qxx.h" 
#include "display.h"
#include "LCD.h"
#include "text.h"
#include "spi.h"
#include "includes.h"
#include "keypad.h"
#include "hx710.h"
#include "stmflash.h"
#include "plu_control.h"
#include "os_cfg.h"
#include "ucos_ii.h"

#define piMax 4995

extern const unsigned char gImage_MM_T035[];
unsigned char plu_info[35][30] = {0};
unsigned char plu_gbk[26] = {0};
extern u32 EP1_ReceivedCount;
static u8 err_id[3][5]={
	{237,238,239,240,137,},/*金额过大*/
	{241,242,243,244,137,},/*超过10条*/
	{137,137,137,137,137,},/*    */
};
static u8 bengin_reminder[4][7]={
	{128,129,130,131,132,137,137},		/* 正在启动。  			*/
	{128,129,130,131,132,132,137},		/* 正在启动。。  		*/
	{128,129,130,131,132,132,132},		/* 正在启动。。。 		*/
	{221,222,223,224,225,226,137},		/* 请勿摇晃云秤 		*/
};

int key1_map[50] = {
		-1,-1, 3, 6, 9,-1,-1,-1,-1,-1,
		-1,-1, 2, 5, 8,-1,-1,-1,-1,-1,
		-1, 0, 1, 4, 7,-1,-1,-1,-1,-1,
		-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
		-1,-1,-1,-1,-1,-1,-1,-1,-1,-1
}; 
int key2_map[50] = {
	1 ,2 ,3 ,4 ,5 ,6 ,7 ,-1,-1,-1,
	8 ,9 ,10,11,12,13,14,-1,-1,-1,
	15,16,17,18,19,20,21,-1,-1,-1,
	22,23,24,25,26,27,28,-1,-1,-1,
	29,30,31,32,33,34,35,-1,-1,-1
};
unsigned char usb_recv_buff[64] = {0};
unsigned char usb_send_buff[64] = {0};

u16 kvalue[2];
//struct key_info * key = NULL;

//全局数据
float K = 0.83375;
u8 priceFlag = 0;         //价格清零状态
u8 pointflag = 0; 
u8 recvflag = 0;           //
u8 stability = 0;             //重量稳定标志
u8 err_price_flag = 0;       //当价格输入出现异常的时候
u8 beep_flag = 0;             //
u8 beep_two = 0;
u8 goods_flag = 0;
u8 key2n = 0;                //kay2按下的按键编号

int g_weight;                             	//全局重量值
int weight_real = 0;
unsigned int g_price = 0;                   //全局单价
unsigned int g_total = 0;                   //全局总价
unsigned int g_sumtotal = 0;                //全局累计总价

int adjust = 0;                                     //重量校准值
u8 goods_num = 0;                            //商品数目
u8 addflag = 0;                               //增加商品的flag
u8 clearflag = 0;                              //清空商品flag
u8 removeflag = 0;							//撤销上一商品flag
u8 Dcode = 0;
unsigned int totalprice[10]={0};
unsigned int priceNum[10]={0};
unsigned int weightNum[10]={0};

int g_adjust = 0;                           //重量校准值
int g_peel = 0;                               //去皮调整值

u32 ReceivedCount;
u8 updateflag = 0;
u8 adjustflag = 0;



/////////////////////////UCOSII任务设置///////////////////////////////////
//START 任务
#define START_TASK_PRIO      			10 //开始任务的优先级设置为最低
#define START_STK_SIZE  				128//设置任务堆栈大小
OS_STK START_TASK_STK[START_STK_SIZE];	//任务堆栈	
void start_task(void *pdata);	//任务函数
 			   
//处理任务/
#define Contrl_TASK_PRIO       			5   //设置任务优先级
#define Contrl_STK_SIZE  		   		128  //设置任务堆栈大小
OS_STK Contrl_TASK_STK[Contrl_STK_SIZE];//任务堆栈	
void contrl_task(void *pdata);         //任务函数


//USB任务
#define USB_TASK_PRIO       			3 		//设置任务优先级
#define USB_STK_SIZE  					128		//设置任务堆栈大小
OS_STK USB_TASK_STK[USB_STK_SIZE];					//任务堆栈
void usb_task(void *pdata);						//任务函数


//KEY任务
#define KEY_TASK_PRIO       			4		//设置任务优先级
#define KEY_STK_SIZE  					128		//设置任务堆栈大小
OS_STK KEY_TASK_STK[KEY_STK_SIZE];					//任务堆栈
void key_task(void *pdata);						//任务函数

//send任务
//#define send_TASK_PRIO       			 3		//设置任务优先级
//#define send_STK_SIZE  					128		//设置任务堆栈大小
//OS_STK send_TASK_STK[send_STK_SIZE];					//任务堆栈
//void send_task(void *pdata);						//任务函数

//AD
#define AD_TASK_PRIO            	 	6	//设置任务优先级
#define AD_STK_SIZE  					128		//设置任务堆栈大小
OS_STK AD_TASK_STK[AD_STK_SIZE];					//任务堆栈
void ad_task(void *pdata);						//任务函数

OS_EVENT *keymsg,*USBMBox,*admsg;
OS_TMR *tmr1;
OS_TMR *tmr2;
//-*****************************处理函识声明**********************************************************
void weight_adjust(void);
void key1_handle(u8 key_num);
void key2_handle(u8 key_num);
void usb_in_func(void);


void Beep_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.8
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	//??????
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}

void HDMI_init(void)
  {
	  GPIO_InitTypeDef  GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	//??????
    GPIO_Init(GPIOE, &GPIO_InitStructure);
		PEout(0)=1;
   }
void MOTO_init(void)
  {
	  GPIO_InitTypeDef  GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	//??????
    GPIO_Init(GPIOC, &GPIO_InitStructure);
		PCout(13)=0;
   }
void AUDIO_init(void)
  {
	  
		GPIO_InitTypeDef  GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	//复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure);
		PAout(15)=0;
   }


//-*************************main**************************************************************
 int main(void)
 {	
	u8 stat = 10;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable,ENABLE);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	delay_init();	    //延时函数初始化	  
	uart_init(115200);

	lcd_init();
//	printf("lcd init finish\n");
	Lcd_Initialize();
//	printf("lcd init over\n");

	
	AUDIO_init();
	 PAout(15) = 0;
	 Beep_Init();
//	MOTO_init();
	 printf("hdmi audio moto  init finish\n");
	delay_ms(100);
	
//	W25QXX_Init();
	 font_init();
	Key1_scan_init();
	Key2_scan_init();
	 
	USB_Interrupts_Config();
	Set_USBClock();
	USB_Init();
	
	ADc_Init();
	plu_init();
	stat = W25QXX_ReadSR();
//	printf("stat=%d\n",stat);
	delay_ms(100);
	
	LCD_Fill_Pic(74,164,172,152,gImage_MM_T035);
	delay_ms(100);
	weight_adjust();
	delay_ms(50);
	K_init();
///////////////////////////////////////////////////////
	
	pro_mode();
	
//////////////////////////////////////////////////////
	
	OSInit(); 
 	OSTaskCreate(start_task,(void *)0,(OS_STK *)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO );//创建起始任务
	OSStart();	
		printf("main finish\n");
}
 
//-****************************定时器中断**********************************************************************

void tmr1_callback(OS_TMR *ptmr,void *p_arg)
{
	u8 err; 
	HDMI_init();
	PEout(0)=0;//HDMI
	PAout(15)=1;
	OSTmrStop(tmr1,OS_TMR_OPT_NONE,0,&err);
}
void tmr2_callback(OS_TMR *ptmr,void *p_arg)
{
	u8 err; 
	PAout(15)=1;
	OSTmrStop(tmr2,OS_TMR_OPT_NONE,0,&err);
}

	  
//开始任务
void start_task(void *pdata)
{
    OS_CPU_SR cpu_sr=0;
	pdata = pdata;
	
	keymsg = OSMboxCreate((void *)0);                                 //创建空邮箱
	USBMBox = OSMboxCreate((void *)0);
	admsg = OSMboxCreate((void *)0);                                 //创建空邮箱
  	OS_ENTER_CRITICAL();			                                  //进入临界区(无法被中断打断)    
 	OSTaskCreate(contrl_task,(void *)0,(OS_STK*)&Contrl_TASK_STK[Contrl_STK_SIZE-1],Contrl_TASK_PRIO);						   
 	OSTaskCreate(usb_task,(void *)0,(OS_STK*)&USB_TASK_STK[USB_STK_SIZE-1],USB_TASK_PRIO);	
	OSTaskCreate(key_task,(void *)0,(OS_STK*)&KEY_TASK_STK[KEY_STK_SIZE-1],KEY_TASK_PRIO);	
//	OSTaskCreate(send_task,(void *)0,(OS_STK*)&send_TASK_STK[send_STK_SIZE-1],send_TASK_PRIO);
	OSTaskCreate(ad_task,(void *)0,(OS_STK*)&AD_TASK_STK[AD_STK_SIZE-1],AD_TASK_PRIO);
	OSTaskSuspend(START_TASK_PRIO);	//挂起起始任务.
	OS_EXIT_CRITICAL();				//退出临界区(可以被中断打断)
}

//display任务
void contrl_task(void *pdata)
{	 

	u8 weightflag = 0;
	u8 more = 0;
	unsigned int i = 0, j = 0,l = 0;
	unsigned char m = 0;
	unsigned char fat = 0;
	u8 err;
	u8 totalerr[8] = {11,11,11,11,13,10,13,13};
		
	Lcd_ColorBox(1,1,320,500,Black);//清屏	
	tmr1=OSTmrCreate(600,10,OS_TMR_OPT_ONE_SHOT,(OS_TMR_CALLBACK)tmr1_callback,0,"tmr1",&err);//创建定时器1
	OSTmrStart(tmr1,&err);
	tmr2=OSTmrCreate(700,10,OS_TMR_OPT_ONE_SHOT,(OS_TMR_CALLBACK)tmr2_callback,0,"tmr2",&err);//创建定时器2
	OSTmrStart(tmr2,&err);
	delay_ms(100);
	printf("contrl task finish\n");	
	while(1)
	{							
		
		OSMboxPend(admsg,0,&err);
		
		if(g_weight > 25000)
		{
			if(more < 3)
			{
				more ++;
			}
			else
			{
				Lcd_ColorBox(1,1,320,500,Black);//清屏
				Show_Str(100,50,200,75,"重量超出量程，请放置小于25千克的物品！",24,0);
				fat = 1;
				PCout(2) = 1;
		
			}
			
		}
		
		else
		{
			more = 0;
			if(fat == 1)
			{
				fat =0;
				PCout(2)=0;
				Lcd_ColorBox(1,1,320,500,Black);//清屏
			}
			
			if((priceFlag == 1) && (stability == 1)) 
			{
				g_price   = 0;
				pointflag = 0;
				priceFlag = 0;
			}
			pi_dispaly(g_peel);
			
			if(g_weight >= 0)
			{
				g_total = (((unsigned int)g_weight * g_price) + 500)/1000;			
			}
			else
				g_total = 0;
	//this is usb send code
			
			for(m=0;m<64;m++)
			{
				usb_send_buff[m]=0;
			}
				
				printf("send flag 1\n");
				usb_send_buff[0]=0;
				usb_send_buff[1] = 1;
				usb_send_buff[2]=0;
				usb_send_buff[3]=0;
				
				usb_send_buff[4] = 8;
				usb_send_buff[5] = 4;
				usb_send_buff[6] = g_weight & 0xff;
				usb_send_buff[7] = (g_weight >> 8) & 0xff;
				usb_send_buff[8] = (g_weight >> 16) & 0xff;
				usb_send_buff[9] = (g_weight >> 24) & 0xff;
				usb_send_buff[10] = 0;		
				USB_SendData(ENDP1,usb_send_buff,64);
			
		
			if(addflag == 1)
			{
				addflag = 0;
				printf("send flag 2\n");
				for(m=0;m<64;m++)
				{
				usb_send_buff[m]=0;
				}
				usb_send_buff[0] = 0;
				usb_send_buff[1] = 2;
				usb_send_buff[2] = 0;
				usb_send_buff[3] = 0;
				
				usb_send_buff[4] = 8;      //weight
				usb_send_buff[5] = 4;
				usb_send_buff[6] = weightNum[goods_num] & 0xff;
				usb_send_buff[7] = (weightNum[goods_num] >> 8) & 0xff;
				usb_send_buff[8] = (weightNum[goods_num] >> 16) & 0xff;
				usb_send_buff[9] = (weightNum[goods_num] >> 24) & 0xff;
				
				usb_send_buff[10] = 15;    //plu code
				usb_send_buff[11] = 4;
				usb_send_buff[12] = plu_info[key2n-1][0];
				usb_send_buff[13] = plu_info[key2n-1][1];
				usb_send_buff[14] = plu_info[key2n-1][2];
				usb_send_buff[15] = plu_info[key2n-1][3];
				
				usb_send_buff[16] = 13;     //price
				usb_send_buff[17] = 4;
				usb_send_buff[18] = priceNum[goods_num] & 0xff;
				usb_send_buff[19] = (priceNum[goods_num] >> 8)  & 0xff;
				usb_send_buff[20] = (priceNum[goods_num] >> 16) & 0xff;
				usb_send_buff[21] = (priceNum[goods_num] >> 24) & 0xff;
				
				usb_send_buff[22] = 14;      //total price
				usb_send_buff[23] = 4;
				usb_send_buff[24] = totalprice[goods_num] & 0xff;
				usb_send_buff[25] = (totalprice[goods_num] >> 8)  & 0xff;
				usb_send_buff[26] = (totalprice[goods_num] >> 16) & 0xff;
				usb_send_buff[27] = (totalprice[goods_num] >> 24) & 0xff;
				
				usb_send_buff[28] = 16;     //sum total price
				usb_send_buff[29] = 4;
				usb_send_buff[30] = g_sumtotal & 0xff;
				usb_send_buff[31] = (g_sumtotal >> 8)  & 0xff;
				usb_send_buff[32] = (g_sumtotal >> 16) & 0xff;
				usb_send_buff[33] = (g_sumtotal >> 24) & 0xff;
				
				usb_send_buff[34] = 10;
				usb_send_buff[35] = 1;
				usb_send_buff[36] = goods_num;
				usb_send_buff[37] = 0;		
				USB_SendData(ENDP1,usb_send_buff,64);

				printf("add goods \n");
			}
			if(clearflag == 1)
			{
				clearflag = 0;
				
				for(m=0;m<64;m++)
				{
					usb_send_buff[m]=0;
				}
				usb_send_buff[0]=0;
				usb_send_buff[1]=4;
				usb_send_buff[2]=0;
				usb_send_buff[3]=0;	
				
				usb_send_buff[4]=0;
				delay_ms(10);
				USB_SendData(ENDP1,usb_send_buff,64);
				
				printf("clear goods\n");
			}
			if(removeflag == 1)
			{
				removeflag = 0;
				priceFlag = 0;
				for(m=0;m<64;m++)
				{
					usb_send_buff[m]=0;
				}
				
				usb_send_buff[0] = 0;
				usb_send_buff[1] = 3;
				usb_send_buff[2] = 0;
				usb_send_buff[3] = 0;
				
				usb_send_buff[4] = 0x0a;
				usb_send_buff[5] = 1;
				usb_send_buff[6] = goods_num;
				usb_send_buff[7] = 0;
				delay_ms(10);
				USB_SendData(ENDP1,usb_send_buff,64);
				goods_num --;
				
				printf("remove goods\n");
				
			}
			if(Dcode == 1)
			{
				Dcode = 0;
				
				for(m=0;m<64;m++)
				{
					usb_send_buff[m]=0;
				}
				usb_send_buff[0]=0;
				usb_send_buff[1]=8;
				usb_send_buff[2]=0;
				usb_send_buff[3]=0;	
				
				usb_send_buff[4]=0;
				delay_ms(10);
				USB_SendData(ENDP1,usb_send_buff,64);
				printf("code\n");
			}
			
	// usb send code over

			
			
			
			if((stability == 0) && (g_weight > 0))
			{
				LCD_ShowChar8(10,130,135,12,0);   //显示称重稳定标志
			}
			else
			{
				Lcd_ColorBox(120,10,30,30,Black);//清除称重稳定标志 
			}
	//-********************异常提示
			if((err_price_flag == 1) || (goods_flag == 1))
			{
				i++;
				if(err_price_flag == 1)
				{
					LCD_ShowString8_red(100,50,73,25,5,err_id[0]);   //价格过大的提示
				}
				if(goods_flag == 1)
				{
					LCD_ShowString8_red(100,50,73,25,5,err_id[1]);   //超出数目的提示
				}
				
				if(i > 3)
				{
					i = 0;
					err_price_flag = 0;
					goods_flag =0;
					LCD_ShowString8_red(100,50,73,25,5,err_id[2]);   //清除提示
				}
			}
			
			weight_dis(g_weight,&weightflag);
			price_dis(g_price);				
			sumtotal_dis(g_sumtotal);
			
			if(priceFlag == 1)
			{
				LCD_ShowChar8(10,225,127,12,0);      //价格锁定
				total_dis(g_total);
			}
			else
			{
				Lcd_ColorBox(215,10,30,30,Black);
				Show_Str(100,80,120,25,"          ",24,0);
				LCD_ShowStringNum(232,225,170,25,8,totalerr);
			}
			
			
					
			LCD_ShowChar8(150,5,125,12,0);    //电池显示
			LCD_ShowChar8(50,5,126,12,0);     //'kg'显示	
			l++;
			if(pointflag == 0)
			{			
				if(l <= 3)
				{
					LCD_Fill(377,175,392,179,White);
					LCD_Fill(401,175,416,179,Black);
				}
				else
				{
					LCD_Fill(377,175,392,179,Black);
					LCD_Fill(401,175,416,179,Black);
				}
			}
			if(pointflag == 1)
			{
				if(l<=3)
				{
					LCD_Fill(377,175,392,179,Black);
					LCD_Fill(401,175,416,179,White);
				}
				else
				{
					LCD_Fill(377,175,392,179,Black);
					LCD_Fill(401,175,416,179,Black);
				}
			}
			if(l==6)l=0;
		}
	}
}
//-********************contrl 函数 结束****************************************************************


//USB任务
void usb_task(void *pdata)
{	
	unsigned char err;
    while (1)
	{
		printf("usb task\n");
		OSMboxPend(USBMBox,0,&err);
         if(EP1_ReceivedCount >= 1)
		 {
			usb_in_func();
		 }	
		 else 
			 continue;

	}
}



void ad_task(void *pdata)
{
//	int weight[5] = {0};
	int weight_new = 0;
	int weight_tmp = 0;
	

	
	while(1)
	{
		
		weight_tmp = GetAdValue();
		
		weight_new = (weight_tmp - adjust);		//
		weight_new = ((int)((float)weight_new * K))/100 - g_peel;                                //去皮操作 
//		printf("source:%d\n",weight_new);		//此处增加显示稳定标志
		
		if((weight_new <= 3)&&(weight_new >= -3))
		{
			weight_new = 0;
		}
		
		  
		if(((weight_new-weight_real) > 4) || ((weight_new-weight_real) < -4))
		{
			weight_real = weight_new;  
			stability = 1;				//不稳定 为1  稳定为0
		}
		else
			stability = 0;
	
		if(weight_real >= 0)
			g_weight =((weight_real+2)/5)*5;
		else
			g_weight = -((((-weight_real)+2)/5)*5);
		OSMboxPost(admsg,(void *)g_weight);
					
		delay_ms(100);
	}
}

void key_task(void *pdata)
{
	unsigned char i = 0,j = 0,m=0;
	struct key_info key;      //哪个键盘 和按键号
	unsigned int key1_flag[5] = {0};     //右侧键盘 按键状态标志位
	unsigned int key2_flag[7] = {0};    //左侧键盘 按键状态标志位
	
	
	while(1)
	{
		//-********************************************键盘一的扫描*******************************************
	//	printf("keykeyukye\n");
		for(i = 0; i < 5; i++)  
		{
			GPIO_ResetBits(key1_output[i].GPIO_x, key1_output[i].GPIO_pin);
			delay_ms(5);
			for(j = 0; j < 5; j++) 
			{
				if(GPIO_ReadInputDataBit(key1_input[j].GPIO_x, key1_input[j].GPIO_pin) == 0)
				{
					delay_ms(2);
					if(GPIO_ReadInputDataBit(key1_input[j].GPIO_x, key1_input[j].GPIO_pin) == 0)
					{
						key.key_type = 1;
						key.key_num = i*10 + j;
						if((key1_flag[i] & ((0x1)<<j)) == 0)              //有按键按下 判断之前状态
						{
	//						printf("keypad >>> 1num %d\n",key.key_num);           //发送按键值 调试使用串口打印 
							beep_flag = 1;
							key1_handle(key.key_num);
//							OSMboxPost(keymsg,(void *)&key);
							
						}
						key1_flag[i] |= ((0x1) << j);                       //标记按下状态
					
					}
				}
				else
					key1_flag[i] &= ~((0x1)<<j);	                     //取消按下状态						
			}
			GPIO_SetBits(key1_output[i].GPIO_x, key1_output[i].GPIO_pin);
	
		}
		
		//-**************************************以下是键盘2扫描***************************************************
		
		for(i = 0; i < 7; i++)  
		{
			GPIO_ResetBits(key2_output[i].GPIO_x, key2_output[i].GPIO_pin);
			delay_ms(5);
			for(j = 0; j < 5; j++) 
			{
				if(GPIO_ReadInputDataBit(key2_input[j].GPIO_x, key2_input[j].GPIO_pin) == 0)
				{
					delay_ms(2);
					if(GPIO_ReadInputDataBit(key2_input[j].GPIO_x, key2_input[j].GPIO_pin) == 0)
					{
						key.key_type = 2;
						key.key_num = j*10 + i;
						if((key2_flag[i] & ((0x1)<<j)) == 0)              //有按键按下 判断之前状态
						{
	//						printf("keypad >>> 2num %d\n",key.key_num);           //发送按键值 调试使用串口打印 
							beep_flag = 1;
							key2_handle(key.key_num);
			//				OSMboxPost(keymsg,(void *)&key);
						}
						key2_flag[i] |= ((0x1)<<j);                       //标记按下状态
					
					}
				}
				else
					key2_flag[i] &= ~((0x1)<<j);	                     //取消按下状态						
			}
			GPIO_SetBits(key2_output[i].GPIO_x, key2_output[i].GPIO_pin);
	
		}
		
		
	delay_ms(20);
	if(beep_flag == 1)
			{
				m++;
				PCout(2)=1;
				if(m > 1)
				{
					m = 0;
					beep_flag = 0;
					PCout(2)=0;
				}
				
			}
			if(beep_two == 1)
			{
				m++;
				PCout(2)=1;
				if(m == 1)
				{
					PCout(2)=0;
				}
				if(m == 2)
				{
					PCout(2)=1;
				}
				if(m == 3)
				{
					m=0;
					beep_two = 0;
					PCout(2)=0;
				}
				
			}
	}
}



//-*******************************************************************
void weight_adjust()
{
	u8 i = 0;
	int adj[5] = {0};
	int max=0,min=0;
	
	for(i=0;i<5;i++)
	{
		LCD_ShowString8(164,260,170,25,7,bengin_reminder[0]);
		delay_ms(100);	
		LCD_ShowString8(164,260,170,25,7,bengin_reminder[1]);
		delay_ms(100);
		LCD_ShowString8(164,260,170,25,7,bengin_reminder[2]);
		delay_ms(100);		
		adj[i] = GetAdValue();
//		printf(" in weight adjust\n");
	}
	max = min = adj[0];
	for(i=0;i<5;i++)
	{
		max = max >= adj[i] ? max : adj[i];
		min = min <= adj[i] ? min : adj[i];
	}
	if((max - min) <= 250)
	{
		adjust = (adj[0]+adj[1]+adj[2]+adj[3]+adj[4])/5;
	}
	else
	{		
		while(1)Show_Str(45,30,200,50,"晃动异常请重启！",24,0);
	}
}

//-***************************  KEY1   *****************************************
void key1_handle(u8 key_num)
{
	unsigned char i = 0;
	int adj[5] = {0};
	if(key1_map[key_num] != -1)
	{
		if(priceFlag == 0)
		{
			if(pointflag == 0)
			{
				if(g_price >= 10000)
				{
					err_price_flag = 1;
					
				}
				else
					g_price = g_price *10 + key1_map[key_num]*10;
			}
			else
			{
				
					g_price = g_price - (g_price%10) + key1_map[key_num];
				
					
			
			}
		}
	}
	else
	{
		switch(key_num)
		{
			case 1:
				
					Dcode = 1;
				break;
			case 10:
					if((g_peel + weight_real) > piMax)
						break;
					g_peel = g_peel + weight_real;
					if(g_peel < 5)
						g_peel = 0;
				break;
			case 11:
				pointflag = 1;
				break;
			case 20:            //归零
				if(g_peel > 0)
					break;
				else
				{			
					if((g_weight > 500) || (g_weight == 0))
						break;
					else
					{
						for(i=0;i<5;i++)
						{
							adj[i] = GetAdValue();
						}
						adjust = (adj[0]+adj[1]+adj[2]+adj[3]+adj[4])/5;
					}
				}
				break;
				
			case 34:
				goods_num = 0;
				g_sumtotal = 0;
				g_price = 0;
				priceFlag = 0;
				pointflag = 0;			
				clearflag = 1;
				break;
			case 43:           //回删 按原路回删
	//			printf("begin 43");
				if(pointflag == 1)
				{
					g_price = ((g_price)/10)*10;
					pointflag = 0;
				}
				else
				{
					g_price = ((g_price)/100)*10;
				}
				break;
			case 44:      //价格归零/撤销商品
				if((g_price != 0) &&(priceFlag != 1))
				{
					g_price = 0;
				}				
				else if(goods_num > 0)
				{	
					removeflag = 1;
					g_price = 0;
					g_sumtotal = g_sumtotal - totalprice[goods_num];

				}
				pointflag = 0;
				break;
			default:
				break;
		}
	}
	
}

//-********************************KEY  2*******************************************
void key2_handle(u8 key_num)
{
	unsigned char i = 0;
	if(key2_map[key_num] != -1)
	{
		Show_Str(100,80,120,96,"          ",24,0);
		if(plu_info[key2_map[key_num]-1][0] == 255)
			if(plu_info[key2_map[key_num]-1][1] == 255)
				if(plu_info[key2_map[key_num]-1][2] == 255)
					if(plu_info[key2_map[key_num]-1][3] == 255)
					{
						beep_two = 1;
						return;
					}
		
		if(plu_info[key2_map[key_num]-1][0] == 0)
			if(plu_info[key2_map[key_num]-1][1] == 0)
				if(plu_info[key2_map[key_num]-1][2] == 0)
					if(plu_info[key2_map[key_num]-1][3] == 0)
					{
						beep_two = 1;
						return;
					}			
		
		
		if(goods_num < 9)
		{
			if(priceFlag == 0)                     //价格没有锁定 
			{
				priceFlag = 1;
				
				goods_num ++;
				totalprice[goods_num] = g_total;
				priceNum[goods_num] = g_price;
				weightNum[goods_num] = g_weight;
				g_sumtotal += totalprice[goods_num];
			}	
			key2n = key2_map[key_num];
			addflag = 1;
			for(i=0;i<26;i++)
			{
				plu_gbk[i] = plu_info[key2_map[key_num]-1][i+4];
			}
			Show_Str(100,80,120,25,plu_gbk,24,0);
		}
		else
			goods_flag = 1;                 //超出商品数目		
	}
}





void usb_in_func()
{
	unsigned char   mBuf[64] = {0};
	u32 offset = 0;
	u16 temp[15] = {0};
    USB_GetData(ENDP1,mBuf,EP1_ReceivedCount);
//	printf("de dao shu jv\n");
	if (EP1_ReceivedCount) 
	{
		delay_ms(10);
		if((mBuf[0]==0)&&(mBuf[1]==0)&&(mBuf[2]==0x02)&&(mBuf[3]==0))
		{			
			updateflag = 1;	
			printf("begin signal\n");
			OSTaskSuspend(Contrl_TASK_PRIO);
			for(offset=0;offset<35;offset++)
			{
				STMFLASH_Write(PLU_ADDR+offset*30,temp,15);
			}
			mBuf[0]=0;
			mBuf[1]=5;
			mBuf[2]=0;
			mBuf[3]=0;
			mBuf[4]=0x14;
			mBuf[5]=0x01;
			mBuf[6]=0x01;
			USB_SendData(ENDP1,mBuf,64);//发送反馈消息	
		}
		if((mBuf[0]==0)&&(mBuf[1]==0)&&(mBuf[2]==0x03)&&(mBuf[3]==0))
		{			
			updateflag = 0;	
			printf("end signal\n");
			mBuf[0]=0;
			mBuf[1]=6;
			mBuf[2]=0;
			mBuf[3]=0;
			mBuf[4]=0x14;
			mBuf[5]=0x01;
			mBuf[6]=0x01;
			USB_SendData(ENDP1,mBuf,64);//发送反馈消息
			OSTaskResume(Contrl_TASK_PRIO);
//			clearflag = 1;
			
		}
		if((mBuf[0]==0)&&(mBuf[1]==0)&&(mBuf[2]==0x01)&&(mBuf[3]==0))
		{
			printf("pan duan success \n");
			if(update_plu(mBuf))
			{
				mBuf[0]=0;
				mBuf[1]=5;
				mBuf[2]=0;
				mBuf[3]=0;
				mBuf[4]=0x14;
				mBuf[5]=0x01;
				mBuf[6]=0x01;
//				mBuf[7]=0x00;
				USB_SendData(ENDP1,mBuf,64);
			}
			else
			{
				mBuf[0]=0;
				mBuf[1]=5;
				mBuf[2]=0;
				mBuf[3]=0;
				mBuf[4]=0x14;
				mBuf[5]=0x01;
				mBuf[6]=0x01;
				mBuf[7]=0x00;
				USB_SendData(ENDP1,mBuf,64);
			}			
		}
	}
}


