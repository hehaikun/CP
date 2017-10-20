#ifndef __HX710_H
#define __HX710_H


#include "includes.h"
#include "includes.h"
#include "delay.h"
#include "sys.h"
#define AD_CLK  PCout(0)	
#define AD_BIT PCin(1)
int GetAdValue(void);
void ADc_Init(void);
//void AD_task(void *pdata);



#endif
