/******************************************************************************

*重要说明！
在.h文件中，#define Immediately时是立即显示当前画面
而如果#define Delay，则只有在执行了LCD_WR_REG(0x0007,0x0173);
之后才会显示，执行一次LCD_WR_REG(0x0007,0x0173)后，所有写入数
据都立即显示。
#define Delay一般用在开机画面的显示，防止显示出全屏图像的刷新过程
******************************************************************************/
#include "LCD.h"
//#include "ff.h"	
#include <stdio.h>
#include "MM_T035.h"
#include "font.h" 
/*
 * 函数名：LCD_FSMC_Config
 * 描述  ：LCD  FSMC 模式配置
 * 输入  ：无
 * 输出  ：无
 * 调用  ：内部调用        
 */
static void LCD_FSMC_Config(void)
{
    FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
    FSMC_NORSRAMTimingInitTypeDef  p; 
    
    
    p.FSMC_AddressSetupTime = 0x02;	 //地址建立时间
    p.FSMC_AddressHoldTime = 0x00;	 //地址保持时间
    p.FSMC_DataSetupTime = 0x05;		 //数据建立时间
    p.FSMC_BusTurnAroundDuration = 0x00;
    p.FSMC_CLKDivision = 0x00;
    p.FSMC_DataLatency = 0x00;

    p.FSMC_AccessMode = FSMC_AccessMode_B;	 // 一般使用模式B来控制LCD
    
    FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;
    FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
    FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_NOR;
    FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
    FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
    FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
    FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
    FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
    FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &p;
    FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &p; 
    
    FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure); 
    
    /* Enable FSMC Bank1_SRAM Bank */
    FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);  
}

volatile static void Delay(__IO u32 nCount)
{	
	volatile int i;
	for(i=0;i<7200;i++)
    for(; nCount != 0; nCount--);
}  
 

 
 //读取个某点的颜色值	 
//x,y:坐标
//返回值:此点的颜色
  
u16 ssd1289_GetPoint(u16 x,u8 y)
{
	 u16 a = 0;
	
	*(__IO u16 *) (Bank1_LCD_C) = 0x4f;	
	*(__IO u16 *) (Bank1_LCD_D) = x;

	*(__IO u16 *) (Bank1_LCD_C) = 0x4e;	
	*(__IO u16 *) (Bank1_LCD_D) = y;

	*(__IO u16 *) (Bank1_LCD_C) = 0x22;

	 a = *(__IO u16 *) (Bank1_LCD_D); 
   return(a);	  
}


volatile static void LCD_Rst(void)
{			
    Clr_Rst;
    Delay(10000);					   
    Set_Rst;		 	 
    Delay(10000);	
}


static void WriteComm(u16 CMD)
{			
	*(__IO u16 *) (Bank1_LCD_C) = CMD;
}



static void WriteData(u16 tem_data)
{			
	*(__IO u16 *) (Bank1_LCD_D) = tem_data;
}


//  ******************************************************************************
//  * @file    main.c
//  * $Author: Du Mingxing $
// * $Revision: 67 $
//  * $Date:: 2016-05-27 10:00 #$
// * @brief:key1按键结构体定义
//  ******************************************************************************
 _lcd_dev lcddev;







void LCD_DrawPoint(u16 x,u16 y)
{
Lcd_ColorBox(y,x,1,1,Blue);
}  	


//画线
//x1,y1:起点坐标
//x2,y2:终点坐标  
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 

	delta_x=x2-x1; //计算坐标增量 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //设置单步方向 
	else if(delta_x==0)incx=0;//垂直线 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//水平线 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//画线输出 
	{  
		LCD_DrawPoint(uRow,uCol);//画点 
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
} 



//画一个矩形框 
//x1,y1:起点坐标
//x2,y2:终点坐标  
void LCD_Draw_rectangle(u16 x1, u16 y1, u16 x2, u16 y2)
{
	LCD_DrawLine( x1,  y1,  x1,  y2);
	LCD_DrawLine( x1,  y1,  x2,  y1);
	LCD_DrawLine( x2,  y1,  x2,  y2);
	LCD_DrawLine( x1,  y2,  x2,  y2);
	
}
//快速画点
//x,y:坐标
//color:颜色
void LCD_Fast_DrawPoint(u16 x,u16 y,u16 color)
{	   
	WriteComm(0x2a);   
	WriteData(y>>8);
	WriteData(y&0xff);
	WriteData(y>>8);
	WriteData(y&0xff);

	WriteComm(0x2b);   
	WriteData(x>>8);
	WriteData(x&0xff);
	WriteData(x>>8);
	WriteData(x&0xff);
	
	WriteComm(0x2c);
	
	*(__IO u16 *) (Bank1_LCD_D) =color;


  }
//在指定位置画一个指定大小的圆
//(x,y):中心点
//r    :半径
void LCD_Draw_Circle(u16 x0,u16 y0,u8 r)
{
	int a,b;
	int di;
	a=0;b=r;	  
	di=3-(r<<1);             //判断下个点位置的标志
	while(a<=b)
	{
		LCD_DrawPoint(x0-b,y0-a);             //3           
		LCD_DrawPoint(x0+b,y0-a);             //0           
		LCD_DrawPoint(x0-a,y0+b);             //1       
		LCD_DrawPoint(x0-b,y0-a);             //7           
		LCD_DrawPoint(x0-a,y0-b);             //2             
		LCD_DrawPoint(x0+b,y0+a);             //4               
		LCD_DrawPoint(x0+a,y0-b);             //5
		LCD_DrawPoint(x0+a,y0+b);             //6 
		LCD_DrawPoint(x0-b,y0+a);             
		a++;
		//使用Bresenham算法画圆     
		if(di<0)di +=4*a+6;	  
		else
		{
			di+=10+4*(a-b);   
			b--;
		} 
		LCD_DrawPoint(x0+a,y0+b);
	}
}

//在指定位置显示一个字符
//x,y:起始坐标
//num:要显示的字符:" "--->"~"
//size:字体大小 12/16/24
//mode:叠加方式(1)还是非叠加方式(0)
void ShowChar(u16 x,u16 y,u8 num,u8 size,u8 mode,u16 fornt,u16 back)
{  							  
    u8 temp,t1,t;
	u16 y0=y;
	u8 csize=(size/8+((size%8)?1:0))*(size/2);		//得到字体一个字符对应点阵集所占的字节数	
	//设置窗口		   
	num=num-' ';//得到偏移后的值
	for(t=0;t<csize;t++)
	{   
		if(size==12)temp=asc2_1608[num][t]; 	 	//调用1206字体
		else if(size==16)temp=asc2_1608[num][t];	//调用1608字体
		else if(size==24)temp=asc2_2412[num][t];	//调用2412字体
		else return;								//没有的字库
		for(t1=0;t1<8;t1++)	 
		{			    
			if(temp&0x80)LCD_Fast_DrawPoint(x,y,fornt);
			else if(mode==0)LCD_Fast_DrawPoint(x,y,back);
			temp<<=1;
			y++;
			if(x>=480)return;		//超区域了
			if((y-y0)==size)
			{
				y=y0;
				x++;
				if(x>=480)return;	//超区域了
				break;
			}
		}  	 
	}  	
}   

//在指定位置显示一个字符
//x,y:起始坐标
//num:要显示的字符:" "--->"~"
//size:字体大小 12/16/24
//mode:叠加方式(1)还是非叠加方式(0)
void LCD_ShowChar(u16 x,u16 y,u8 num,u8 size,u8 mode)
{  							  
    u8 temp,t1,t;
	u16 y0=y;
	u8 csize=(size/8+((size%8)?1:0))*(size/2);		//得到字体一个字符对应点阵集所占的字节数	
	//设置窗口		   
	num=num-' ';//得到偏移后的值
	for(t=0;t<csize;t++)
	{   
		if(size==12)temp=asc2_1608[num][t]; 	 	//调用1206字体
		else if(size==16)temp=asc2_1608[num][t];	//调用1608字体
		else if(size==24)temp=asc2_2412[num][t];	//调用2412字体
		else return;								//没有的字库
		for(t1=0;t1<8;t1++)	 
		{			    
			if(temp&0x80)LCD_Fast_DrawPoint(x,y,White);
			else if(mode==0)LCD_Fast_DrawPoint(x,y,Black);
			temp<<=1;
			y++;
			if(x>=lcddev.width)return;		//超区域了
			if((y-y0)==size)
			{
				y=y0;
				x++;
				if(x>=lcddev.width)return;	//超区域了
				break;
			}
		}  	 
	}  	
}  

void ShowChar8(u16 x,u16 y,u8 num,u8 size,u8 mode,u16 fornt,u16 back)
{  							  
  u8 temp,t1,t;
//	u16 m1,m2;
	u16 x0=x;
	u16 y0=y+8;
	u16 temp1,temp2;
	int totalnum;
	temp1=x;temp2=y;
	if(size==12)
		totalnum=72;
	if(size==24)
		totalnum=144;	
		BlockWrite(temp1,temp1+24-1,temp2,temp2+size*2-1);
	for(t=0;t<totalnum;t++)
	{   
		if(size==12)temp=asc2_24xx[num][t]; 	 	//调用1206字体
		else if(size==24)temp=asc2_2448[num][t];	//调用2448字体
		else return;
		y=y0;								//没有的字库
		for(t1=0;t1<8;t1++)
		{			    
			if(temp&0x80)LCD_Fast_DrawPoint(x,y,fornt);
//			if(temp&0x80)print_data[x-x0][y-y0]=fornt;
			else if(mode==0)LCD_Fast_DrawPoint(x,y,back);
//			else if(mode==0)print_data[x-x0][y-y0]=back;
			temp<<=1;
            y--;//y--;	// y++;
			if(t1==7)
			{
				x++;
				if(x-x0==24){
					
//								
//		for (m1 = y0-8; m1 < y0; m1++)	
//	{
//	for (m2 = 0; m2 < 24; m2++)
//	{

//		*(__IO u16 *) (Bank1_LCD_D)  = print_data[m2][m1];
//	} 
//}	
					
				x=x0;
				y0+=8;
				}
			}
		}  	 
	} 

} 
void LCD_ShowChar8(u16 x,u16 y,u8 num,u8 size,u8 mode)
{  							  
    u8 temp,t1,t;
	u16 x0=x;
	u16 y0=y;
	int totalnum;
	if(size==12)
		totalnum=72;
	if(size==24)
		totalnum=144;	
	for(t=0;t<totalnum;t++)
	{   
		if(size==12)temp=asc2_24xx[num][t]; 	 	//调用1206字体
		else if(size==24)temp=asc2_2448[num][t];	//调用2448字体
		else return;
		y=y0;								//没有的字库
		for(t1=0;t1<8;t1++)
		{			    
			if(temp&0x80)LCD_Fast_DrawPoint(x,y,White);
			else if(mode==0)LCD_Fast_DrawPoint(x,y,Black);
			temp<<=1;
            y--;//y--;	// y++;
			if(t1==7)
			{
				x++;
				if(x-x0==24){
				x=x0;
				y0+=8;
				}
			}
		}  	 
	}  	
	
	

} 


//m^n函数
//返回值:m^n次方.
u32 LCD_Pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}	 
//显示数字,高位为0,还是显示
//x,y:起点坐标
//num:数值(0~999999999);	 
//len:长度(即要显示的位数)
//size:字体大小
//mode:
//[7]:0,不填充;1,填充0.
//[6:1]:保留
//[0]:0,非叠加显示;1,叠加显示.
void LCD_ShowxNum(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode)
{  
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/LCD_Pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				if(mode&0X80)LCD_ShowChar(x+(size/2)*t,y,'0',size,mode&0X01);  
				else LCD_ShowChar(x+(size/2)*t,y,' ',size,mode&0X01);  
 				continue;
			}else enshow=1; 
		 	 
		}
	 	LCD_ShowChar(x+(size/2)*t,y,temp+'0',size,mode&0X01); 
	}
} 
//显示字符串
//x,y:起点坐标
//width,height:区域大小  
//size:字体大小
//*p:字符串起始地址		  
void LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p)
{         
	u8 x0=x;
	width+=x;
	height+=y;
    while((*p<='~')&&(*p>=' '))//判断是不是非法字符!
    {       
        if(x>=width){x=x0;y+=size;}
        if(y>=height)break;//退出
        LCD_ShowChar(x,y,*p,size,0);
        x+=size/2;
        p++;
    }  
}
//显示字符串
//x,y:起点坐标
//width,height:区域大小  
//size:字体大小
//*p:字符串起始地址	
void LCD_ShowString8(u16 x,u16 y,u16 width,u16 height,u8 length,u8 *p)
{         
	u8 x0=x;
	u8 i;
	width+=x;
	height+=y;
      for(i=0;i<length;i++)
	      {
        if(x>=width){x=x0;y+=24;}
        if(y>=height)break;//退出
        ShowChar8(x,y,*p,12,0,White,Black);
        x+=24;
        p++;
			}
} 

void LCD_ShowStringNum(u16 x,u16 y,u16 width,u16 height,u8 length,u8 *p)
{         
	u8 x0=x;
	u8 i;
	width+=x;
	height+=y;
      for(i=0;i<length;i++)
	      {
        if(x>=width){x=x0;y+=24;}
        if(y>=height)break;//退出
        ShowChar8(x,y,*p,24,0,White,Black);
        x+=24;
        p++;
			}
} 
/**********************************************
	Lcd display ! red 
 */
void LCD_ShowString8_red(u16 x,u16 y,u16 width,u16 height,u8 length,u8 *p)
{         
	u8 x0=x;
	u8 i;
	width+=x;
	height+=y;
  for(i=0;i<length;i++)
	   {
        if(x>=width){x=x0;y+=24;}
        if(y>=height)break;//退出
        ShowChar8(x,y,*p,12,0,Red,Black);
        x+=24;
        p++;
			}
} 	

/**********************************************
	Lcd初始化函数
 */
void Lcd_Initialize(void)
{	

u16 a;
LCD_FSMC_Config();
lcddev.width=480;
lcddev.height=320;
Delay(1000);
LCD_Rst();

WriteComm(0xD3);
	
	
a = *(__IO u16 *) (Bank1_LCD_D); Delay(5000);
a = *(__IO u16 *) (Bank1_LCD_D); Delay(5000);
a = *(__IO u16 *) (Bank1_LCD_D); Delay(5000);
a = *(__IO u16 *) (Bank1_LCD_D); Delay(5000);
a = *(__IO u16 *) (Bank1_LCD_D); Delay(5000);
a = *(__IO u16 *) (Bank1_LCD_D); Delay(5000);

// // //************* Start Initial Sequence **********// 

WriteComm(0xC0);Delay(1000);
WriteData(0x0F);Delay(1000);
WriteData(0x0F);Delay(1000);
WriteComm(0xC1);Delay(1000);
WriteData(0x41);Delay(1000);

WriteComm(0XC5);Delay(1000);
WriteData(0x00);Delay(1000);
WriteData(0x3A);Delay(1000);
WriteComm(0xB1);Delay(1000);
WriteData(0xD0);Delay(1000);
WriteData(0x11);Delay(1000);


WriteComm(0xB6);Delay(1000);
WriteData(0x02);Delay(1000);
WriteData(0x22);Delay(1000);

WriteComm(0xB7);Delay(1000);
WriteData(0xC6);Delay(1000);
WriteComm(0xE0);Delay(1000);
WriteData(0x00);Delay(1000);
WriteData(0x0B);Delay(1000);
WriteData(0x10);Delay(1000);
WriteData(0x03);Delay(1000);
WriteData(0x10);Delay(1000);
WriteData(0x08);Delay(1000);

WriteData(0x33);Delay(1000);
WriteData(0x89);Delay(1000);
WriteData(0x48);Delay(1000);
WriteData(0x07);Delay(1000);
WriteData(0x0E);Delay(1000);
WriteData(0x0C);Delay(1000);
WriteData(0x28);Delay(1000);
WriteData(0x2D);Delay(1000);
WriteData(0x0F);Delay(1000);

WriteComm(0XE1);Delay(1000);
WriteData(0x00);Delay(1000);
WriteData(0x12);Delay(1000);
WriteData(0x17);Delay(1000);
WriteData(0x03);Delay(1000);
WriteData(0x11);Delay(1000);
WriteData(0x08);Delay(1000);
WriteData(0x37);Delay(1000);
WriteData(0x67);Delay(1000);
WriteData(0x4C);Delay(1000);
WriteData(0x07);Delay(1000);
WriteData(0x0F);Delay(1000);
WriteData(0x0C);Delay(1000);
WriteData(0x2F);Delay(1000);
WriteData(0x34);Delay(1000);
WriteData(0x0F);Delay(1000);

WriteComm(0XF2);Delay(1000);
WriteData(0x18);Delay(1000);
WriteData(0xA3);Delay(1000);
WriteData(0x12);Delay(1000);
WriteData(0x02);Delay(1000);
WriteData(0xB2);Delay(1000);
WriteData(0x12);Delay(1000);
WriteData(0xFF);Delay(1000);
WriteData(0x10);Delay(1000);
WriteData(0x00);Delay(1000);

WriteComm(0XF7);Delay(1000);
WriteData(0xA9);Delay(1000);
WriteData(0x91);Delay(1000);
WriteData(0x2D);Delay(1000);
WriteData(0x0A);Delay(1000);
WriteData(0x4F);Delay(1000);

WriteComm(0XF8);Delay(1000);
WriteData(0x21);Delay(1000);
WriteData(0x04);Delay(1000);

WriteComm(0x36);Delay(1000);
WriteData(0x48);Delay(1000);

WriteComm(0x3A);Delay(1000);
WriteData(0x66);Delay(1000);

WriteComm(0xF9);Delay(1000);
WriteData(0x00);Delay(1000);
WriteData(0x08);Delay(1000);

WriteComm(0xF4);Delay(1000);
WriteData(0x00);Delay(1000);
WriteData(0x00);Delay(1000);
WriteData(0x08);Delay(1000);
WriteData(0x91);Delay(1000);
WriteData(0x04);Delay(1000);

WriteComm(0x21);Delay(1000);
WriteComm(0x11);Delay(1000);

WriteComm(0x29);Delay(1000);
WriteComm(0x3A); Delay(1000);
WriteData(0x55);Delay(1000);
WriteComm(0x21);Delay(1000);
//Lcd_Light_ON;
Lcd_ColorBox(0,0,320,480,Black);

printf("lcd init over\n");
// while(1);
}
/******************************************
函数名：Lcd写命令函数
功能：向Lcd指定位置写入应有命令或数据
入口参数：Index 要寻址的寄存器地址
          ConfigTemp 写入的数据或命令值
******************************************/
void LCD_WR_REG(u16 Index,u16 CongfigTemp)
{
	*(__IO u16 *) (Bank1_LCD_C) = Index;	
	*(__IO u16 *) (Bank1_LCD_D) = CongfigTemp;
}
/************************************************
函数名：Lcd写开始函数
功能：控制Lcd控制引脚 执行写操作
************************************************/
void Lcd_WR_Start(void)
{
*(__IO u16 *) (Bank1_LCD_C) = 0x2C;
}
/**********************************************
函数名：Lcd块选函数
功能：选定Lcd上指定的矩形区域

注意：xStart、yStart、Xend、Yend随着屏幕的旋转而改变，位置是矩形框的四个角

入口参数：xStart x方向的起始点
          ySrart y方向的起始点
          Xend   y方向的终止点
          Yend   y方向的终止点
返回值：无
***********************************************/
void BlockWrite(unsigned int Xstart,unsigned int Xend,unsigned int Ystart,unsigned int Yend) 
{
	//HX8352-C

	WriteComm(0x2a);   
	WriteData(Xstart>>8);
	WriteData(Xstart&0xff);
	WriteData(Xend>>8);
	WriteData(Xend&0xff);

	WriteComm(0x2b);   
	WriteData(Ystart>>8);
	WriteData(Ystart&0xff);
	WriteData(Yend>>8);
	WriteData(Yend&0xff);


	WriteComm(0x2c);
}
/**********************************************
函数名：Lcd块选函数
功能：选定Lcd上指定的矩形区域

注意：xStart和 yStart随着屏幕的旋转而改变，位置是矩形框的四个角

入口参数：xStart x方向的起始点
          ySrart y方向的终止点
          xLong 要选定矩形的x方向长度
          yLong  要选定矩形的y方向长度
返回值：无
***********************************************/
void Lcd_ColorBox(u16 xStart,u16 yStart,u16 xLong,u16 yLong,u16 Color)
{
	u32 temp;

	BlockWrite(xStart,xStart+xLong-1,yStart,yStart+yLong-1);
	for (temp=0; temp<xLong*yLong; temp++)
	{
		*(__IO u16 *) (Bank1_LCD_D) = Color;
	}
}
//在指定区域内填充单个颜色
//(sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)   
//color:要填充的颜色
void LCD_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 color)
{
	u16 height,width;
	u16 i,j;
	width=ex-sx+1; 			//得到填充的宽度
	height=ey-sy+1;			//高度
	
	 	for(i=0;i<height;i++)
	{
		for(j=0;j<width;j++)
		LCD_Fast_DrawPoint(sx+j,sy+i,color);//写入数据 
	}	 
	
	
	
}

/******************************************
函数名：Lcd图像填充100*100
功能：向Lcd指定位置填充图像
入口参数：Index 要寻址的寄存器地址
          ConfigTemp 写入的数据或命令值
******************************************/
void LCD_Fill_Pic(u16 x, u16 y,u16 pic_H, u16 pic_V, const unsigned char* pic)
{
  unsigned long i;
  unsigned int j;

// 	WriteComm(0x36); //Set_address_mode
// 	WriteData(0x0a); //横屏，从左下角开始，从左到右，从下到上
	BlockWrite(x,x+pic_H-1,y,y+pic_V-1);
	for (i = 0; i < pic_H*pic_V*2; i+=2)
	{
		j=pic[i];
		j=j<<8;
		j=j+pic[i+1];
		*(__IO u16 *) (Bank1_LCD_D) = j;
	}
// 	WriteComm(0x36); //Set_address_mode
// 	WriteData(0xaa);
}

void DrawPixel(u16 x, u16 y, u16 Color)
{
	BlockWrite(x,x+1,y,y+1);
	*(__IO u16 *) (Bank1_LCD_D) = Color;
}


typedef __packed struct
{
	u8  pic_head[2];				//1
	u16 pic_size_l;			    //2
	u16 pic_size_h;			    //3
	u16 pic_nc1;				    //4
	u16 pic_nc2;				    //5
	u16 pic_data_address_l;	    //6
	u16 pic_data_address_h;		//7	
	u16 pic_message_head_len_l;	//8
	u16 pic_message_head_len_h;	//9
	u16 pic_w_l;					//10
	u16 pic_w_h;				    //11
	u16 pic_h_l;				    //12
	u16 pic_h_h;				    //13	
	u16 pic_bit;				    //14
	u16 pic_dip;				    //15
	u16 pic_zip_l;			    //16
	u16 pic_zip_h;			    //17
	u16 pic_data_size_l;		    //18
	u16 pic_data_size_h;		    //19
	u16 pic_dipx_l;			    //20
	u16 pic_dipx_h;			    //21	
	u16 pic_dipy_l;			    //22
	u16 pic_dipy_h;			    //23
	u16 pic_color_index_l;	    //24
	u16 pic_color_index_h;	    //25
	u16 pic_other_l;			    //26
	u16 pic_other_h;			    //27
	u16 pic_color_p01;		    //28
	u16 pic_color_p02;		    //29
	u16 pic_color_p03;		    //30
	u16 pic_color_p04;		    //31
	u16 pic_color_p05;		    //32
	u16 pic_color_p06;		    //33
	u16 pic_color_p07;		    //34
	u16 pic_color_p08;			//35			
}BMP_HEAD;

BMP_HEAD bmp;

 

