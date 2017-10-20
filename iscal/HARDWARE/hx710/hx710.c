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
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	//�����������
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//��������
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
 
	AD_CLK=1;	//ʱ�����ø�
	delay_us(4);//��ʱ2us;
	AD_CLK=0;	//ʱ�����õ�
	delay_us(4);//��ʱ2us;

	ad=(ad<<8)/256;
//	printf("ad source %d\n",ad);

	return ad;
	
}


