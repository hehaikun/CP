#include "keypad.h"
#include "sys.h"
#include "usart.h"
#include "LCD.h"
#include "hx710.h"
#include "stmflash.h"
//*******************************************
//volatile  u8 key1_num = 255;
//  ******************************************************************************

extern float K;
extern int adjust;
extern u16 kvalue[2];


//**********************************************************************************************************************



void Key1_scan_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    unsigned char i;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB, ENABLE);
    /* 键盘扫描输出线 输出低电平 */  /* PB9 PB8 PB7 PB6 PB5 PG15 输出*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5  | GPIO_Pin_6| GPIO_Pin_7 ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;

    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);


	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1| GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;

    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* 键盘扫描输入线 键被按时输入低电平 放开输入高电平 */  /* PG14 PG13 PG11 PG9 PD7 PD6 输入*/

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0| GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3| GPIO_Pin_4;

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    for(i = 0; i < 5; i++)
    {
        GPIO_SetBits(key1_output[i].GPIO_x, key1_output[i].GPIO_pin);
    }

}
//------------------------------

void Key2_scan_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    unsigned char i;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC, ENABLE);
 //   /键盘扫描输出线 输出低电平 /  / PB9 PB8 PB7 PB6 PB5 PG15 输出/
		 GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8 ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;

    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	
    GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_9|GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;

    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	
	  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6| GPIO_Pin_8| GPIO_Pin_9 ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;

    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

//    /键盘扫描输入线 键被按时输入低电平 放开输入高电平 /  / PG14 PG13 PG11 PG9 PD7 PD6 输入/

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6| GPIO_Pin_7| GPIO_Pin_8 ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);


    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);


    for(i = 0; i < 7; i++)
    {
        GPIO_SetBits(key2_output[i].GPIO_x, key2_output[i].GPIO_pin);
    }

}

void pro_mode(void)
{
	int weight;
	int weight_d;
	u16 * kaddr;
	GPIO_ResetBits(key1_output[0].GPIO_x, key1_output[0].GPIO_pin);
	GPIO_ResetBits(key1_output[4].GPIO_x, key1_output[4].GPIO_pin);
	delay_ms(1);
	if(GPIO_ReadInputDataBit(key1_input[0].GPIO_x, key1_input[0].GPIO_pin) == 0)
	{
		delay_ms(2);
		if(GPIO_ReadInputDataBit(key1_input[0].GPIO_x, key1_input[0].GPIO_pin) == 0)
		{
			if(GPIO_ReadInputDataBit(key1_input[4].GPIO_x, key1_input[4].GPIO_pin) == 0)
			{
				delay_ms(2);
				if(GPIO_ReadInputDataBit(key1_input[4].GPIO_x, key1_input[4].GPIO_pin) == 0)
				{
					Lcd_ColorBox(1,1,320,500,Black);//清屏	
					while(GPIO_ReadInputDataBit(key1_input[0].GPIO_x, key1_input[0].GPIO_pin) == 0);
					while(1)
					{
						Show_Str(100,50,200,50,"请放置10Kg砝码进行校准系数",24,0);
						
						weight = (GetAdValue() - adjust);
						if(weight > 1000000)
						{
							K = 1000000.00 / (float)weight;
							
							delay_ms(20);
							weight_d = (int)((float)(GetAdValue() - adjust) * K);
							if( ((weight_d - 1000000) > -250) &&((weight_d - 1000000) < 250))
							{
								Show_Str(100,150,200,50,"校准成功    ",24,0);
							
								kaddr = (u16 *)&K;
								STMFLASH_Write(K_BASE,kaddr,2);	
							}
							else
							{
								Show_Str(100,150,200,50,"正在重新校准",24,0);	
							}
						}
						else
						{
							Show_Str(100,150,200,50,"砝码重量不足",24,0);
						}
						
						if(GPIO_ReadInputDataBit(key1_input[0].GPIO_x, key1_input[0].GPIO_pin) == 0)
						{
							delay_ms(2);
							if(GPIO_ReadInputDataBit(key1_input[0].GPIO_x, key1_input[0].GPIO_pin) == 0)
								break;
						}
					}
				}
			}
		}
						
	}
}

void K_init(void)
{
	u16 kk[2];
	STMFLASH_Read(K_BASE,kk,2);
	K = *(float*)kk;
}


/*
void key_task(void *pdata)
{
	unsigned char i = 0,j = 0;
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
							printf("keypad >>> 1num %d\n",key.key_num);           //发送按键值 调试使用串口打印 
							OSMboxPost(keymsg,(void *)&key);
							
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
							printf("keypad >>> 2num %d\n",key.key_num);           //发送按键值 调试使用串口打印 
							OSMboxPost(keymsg,(void *)&key);
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
	}
}


*/


