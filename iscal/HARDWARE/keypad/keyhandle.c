
#include "keypad.h"
//*******************************************
volatile u32 Zerotracking_data;	 	//零点数值
volatile unsigned char AD_DATA[3];	//重量数值
volatile u32 Acc_bz;
volatile u32 TO_zero_bz; 
volatile u8 remove_cat_bz;
volatile u32 calculate_weight;		//计算重量
volatile u8 PT_stabilize_bz;
volatile u32 cat_weight;			//皮重 
//--------------------------------------------------	 