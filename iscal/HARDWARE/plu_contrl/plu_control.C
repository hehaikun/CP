#include "plu_control.h"
#include "stmflash.h" 
#include "text.h"	
#include "usart.h"
#include "delay.h"



                   
void plu_init()
{
	unsigned int i;
	u32 offset;
	for(i=0;i<35;i++)
	{	
		offset = i*30;	
		STMFLASH_Read(PLU_ADDR+offset,(u16 *)plu_info[i],15);
	}	
}

u32 update_plu(unsigned char *data)
{
	u8 temp[30] = {0};
	unsigned int i;
	u32 offset;
	unsigned int place;
	
	if((data[0]==0)&&(data[1]==0)&&(data[2]==0x01)&&(data[3]==0))
	{
		printf("update begin\n");
		place = get_place(0x11,data);
		if(place == 0)return 0;
		offset=data[place+2+3];
		offset=offset<<8;				
		offset|=data[place+2+2];
		offset=offset<<8;
		offset|=data[place+2+1];
		offset=offset<<8;
		offset|=data[place+2];

		if(offset==0) return 0;//按键值不再范围内
		if(offset>35) return 0;//按键值不再范围内		
		
			
		
		place = get_place(0x0f,data);
		if(place == 0)return 0;
		for(i=0; i<4;i++)
		{
			temp[i] = data[place+2+i];                  //将plu的编号存储起来
		}
		
		place = get_place(0x12,data);
		if(place == 0)return 0;
		for(i=0; i<data[place+1];i++)
		{
			temp[i+4] = data[place+2+i];                //存plu的品名
		}
		for(i=0;i<30;i++)
		{
			plu_info[offset-1][i] = temp[i];            //赋值给全局变量
		}
		STMFLASH_Write(PLU_ADDR+(offset-1)*30,(u16*)temp,15);
		delay_ms(50);
		return 1;
		
	}
	
	
	
}

unsigned int get_place(u8 type,uint8_t *data)
{
	unsigned int i;
//	unsigned int l;
//	unsigned int m;
//	u8 temp[64];
									
	for ( i = 4; i  < 64; ) 
	{
		if (data[i]==0) 
		return 0;							   
		else
		{
			if(data[i]==type)                                                //只是将数据的位置提取出来
			{
//				for ( m = 0; m  < data[i+1]; m++) //打印提取的信息条
//				temp[m]=data[i+2+m];
									
				return i;
			}
			else
				i=data[i+1]+2+i;
		 }
		 if(i==64)
							 
		 return 0;
            
        }	
	
}

// SPI2_Flash_Read(temp1, (+100)*1024,40);
//	uart1_send_(temp,64);//测试使用		
//-**********************************************************************************************




