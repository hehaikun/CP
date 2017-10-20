#ifndef __PLU__H__
#define __PLU__H__
#include <stm32f10x.h>


extern unsigned char plu_info[35][30];
extern u32 EP1_ReceivedCount;


void plu_init(void);
u32 update_plu(unsigned char *data);
unsigned int get_place(u8 type,uint8_t *data);


#endif


