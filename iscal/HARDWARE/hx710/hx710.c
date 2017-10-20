#include "hx710.h"
#include "includes.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
extern OS_EVENT * admsg;

void ADc_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	//复用推挽输出
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//浮空输入
    GPIO_Init(GPIOC, &GPIO_InitStructure);
	printf("adc init end\n");
}



int GetAdValue(void)
{	
	int ad;
	int i;
	AD_CLK=0;
	ad=0;
	while(AD_BIT){delay_us(4);}
	for (i=0;i<24;i++)
	{ 
		ad=ad<<1;
		AD_CLK=1;
		delay_us(4);		
		AD_CLK=0;
		delay_us(4);
		ad|=AD_BIT;
	}
 
	AD_CLK=1;	//时钟线置高
	delay_us(4);//延时2us;
	AD_CLK=0;	//时钟线置低
	delay_us(4);//延时2us;

	ad=(ad<<8)/256;
//	printf("ad source %d\n",ad);

	return ad;
	
}


