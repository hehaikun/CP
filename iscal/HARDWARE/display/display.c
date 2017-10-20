#include "display.h"
#include "LCD.h"
#include "usart.h"
#include "delay.h"
#include "text.h"


//volatile u32 mem_place_bz;
volatile u32 corsur_enable;
extern u8 goods_num;
/*
 * 函数名：LCD_GPIO_Config
 * 描述  ：根据FSMC配置LCD的I/O
 * 输入  ：无
 * 输出  ：无
 * 调用  ：内部调用
 */
void LCD_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Enable the FSMC Clock */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);

    /* config lcd gpio clock base on FSMC */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE , ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    /* config tft rst gpio */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    /* config tft back_light gpio base on the PT4101 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 ;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    /* config tft data lines base on FSMC
     * data lines,FSMC-D0~D15: PD 14 15 0 1,PE 7 8 9 10 11 12 13 14 15,PD 8 9 10
     */
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_8 | GPIO_Pin_9 |
                                  GPIO_Pin_10 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 |
                                  GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 |
                                  GPIO_Pin_15;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    /* config tft control lines base on FSMC
     * PD4-FSMC_NOE  :LCD-RD
    * PD5-FSMC_NWE  :LCD-WR
     * PD7-FSMC_NE1  :LCD-CS
    * PD11-FSMC_A16 :LCD-DC
     */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 ;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    /* tft control gpio init */

    GPIO_SetBits(GPIOD, GPIO_Pin_13);		 // RST = 1
    GPIO_SetBits(GPIOD, GPIO_Pin_4);		 // RD = 1
    GPIO_SetBits(GPIOD, GPIO_Pin_5);		 // WR = 1
    GPIO_SetBits(GPIOD, GPIO_Pin_7);		 //	CS = 1

}
/*******************************************************************************
	所有STM32的芯片都提供对Flash的保护，防止对Flash非法访问 --- 写保护和读保护

*/
void  lcd_init(void)
{
	printf("lcd init\n");
	LCD_GPIO_Config();
    // DispLogo();		/* 显示例程Logo */
}

//-*******************************************************************

void weight_dis(int weight,u8 *weightflag)
{
	u8 weightstr[16]  = {0};

	Show_Str(430,35,200,50,"kg",24,0);	
	if(weight < 0)		
	{			
		weight = -weight;			
		weightstr[0] = 13;			
		*weightflag = 1;
	}
	else
	{
		*weightflag = 0;
		weightstr[0] = weight/10000;
		if(weightstr[0] == 0)	
			weightstr[0] = 11;
	}
	weightstr[1] = weight%10000/1000;
	weightstr[2] = 10;
	weightstr[3] = weight%1000/100;
	weightstr[4] = weight%100/10;
	weightstr[5] = weight%10;

	LCD_ShowStringNum(280,30,170,25,6,weightstr);

	delay_ms(10);
}
//-*******************************************************************
void price_dis(unsigned int price)
{
	u8 pricestr[16]  = {0};
	
	

	Show_Str(430,150,100,25,"元",24,0);
	Show_Str(430,250,100,25,"元",24,0);	
	
	pricestr[0] =price/10000;
	if(pricestr[0] == 0)	
		pricestr[0] = 11;
	
	pricestr[1] = price%10000/1000;	
		if((pricestr[0] == 11) && (pricestr[1] == 0))	
			pricestr[1] = 11;
	pricestr[2] = price%1000/100;
	pricestr[3] = 10;
	pricestr[4] = price%100/10;
	pricestr[5] = price%10;
	LCD_ShowStringNum(280,125,170,25,6,pricestr);

}
void total_dis(unsigned int total)
{
	u8 totalstr[16]  = {0};
	totalstr[0] =total/1000000;
	if(totalstr[0] == 0)	
		totalstr[0] = 11;
	
	totalstr[1] = total%1000000/100000;	
	if((totalstr[0] == 11) && (totalstr[1] == 0))	
		totalstr[1] = 11;
	totalstr[2] = total%100000/10000;	
	if((totalstr[1] == 11) && (totalstr[2] == 0))	
		totalstr[2] = 11;
	totalstr[3] = total%10000/1000;	
	if((totalstr[2] == 11) && (totalstr[3] == 0))	
		totalstr[3] = 11;	
	totalstr[4] = total%1000/100;
	totalstr[5] = 10;
	totalstr[6] = total%100/10;
	totalstr[7] = total%10;
	LCD_ShowStringNum(232,225,170,25,8,totalstr);
}

void sumtotal_dis(unsigned int sumtotal)
{
	u8 i;
	u8 sumtotalstr[16]  = {0};
	if(goods_num == 0)
	{
		for(i=0;i<8;i++)
		{
			sumtotalstr[i] = 11;
		}
	}
	else
	{
		sumtotalstr[0] =sumtotal/1000000;
		if(sumtotalstr[0] == 0)	
			sumtotalstr[0] = 11;
		
		sumtotalstr[1] = sumtotal%1000000/100000;	
		if((sumtotalstr[0] == 11) && (sumtotalstr[1] == 0))	
			sumtotalstr[1] = 11;
		sumtotalstr[2] = sumtotal%100000/10000;	
		if((sumtotalstr[1] == 11) && (sumtotalstr[2] == 0))	
			sumtotalstr[2] = 11;
		sumtotalstr[3] = sumtotal%10000/1000;	
		if((sumtotalstr[2] == 11) && (sumtotalstr[3] == 0))	
			sumtotalstr[3] = 11;	
		sumtotalstr[4] = sumtotal%1000/100;
		sumtotalstr[5] = 10;
		sumtotalstr[6] = sumtotal%100/10;
		sumtotalstr[7] = sumtotal%10;
	}
	LCD_ShowStringNum(40,225,170,25,8,sumtotalstr);
	
}

//-*********************************************************************************
void pi_dispaly(int pi)    //去皮的显示函数
{	
	u8 pistr[10] = {0};
//	u8 dis[10] = 
	u8 hz[2] = {156,157};
	static u8 flag = 0;
	
	if(pi > 0)
	{		
		pi = ((pi+2)/5)*5;
		pistr[0] = '-';				
		pistr[1] = pi/1000+48;
		pistr[2] = '.';
		pistr[3] = pi%1000/100+48;
		pistr[4] = pi%100/10+48;
		pistr[5] = pi%10 +48;
		pistr[6] = '\0';

		LCD_ShowString8(100,15,170,25,2,hz);
		Show_Str(160,15,200,25,pistr,24,0);
		flag = 1;
	}
	else
	{
		if(flag == 1)
		Lcd_ColorBox(15,100,35,140,Black);//清屏	
		flag = 0;
	}

}







