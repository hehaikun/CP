#ifndef		_KEYPAD
#define 	_KEYPAD


#include "LCD.h"
//#include "main.h"
#include "ucos_ii.h"
#include "text.h"	
//#include "hummer.h"
#include "delay.h"

 struct key_info
 {
	unsigned char key_type;
	 unsigned char key_num;
 };
struct io_port {
    GPIO_TypeDef *GPIO_x;
    unsigned short GPIO_pin;
};
extern OS_EVENT * keymsg;
static struct io_port key1_output[5] = {
    {GPIOB, GPIO_Pin_1},
    {GPIOB, GPIO_Pin_0},
    {GPIOA, GPIO_Pin_7},
    {GPIOA, GPIO_Pin_6},
    {GPIOA, GPIO_Pin_5}
};

static struct io_port key1_input[5] =
{
    {GPIOA, GPIO_Pin_4},
    {GPIOA, GPIO_Pin_3},
    {GPIOA, GPIO_Pin_2},
    {GPIOA, GPIO_Pin_0},
    {GPIOA, GPIO_Pin_1}
};


static struct io_port key2_output[7] = {
    {GPIOC, GPIO_Pin_8}, 		 //    {GPIOC, GPIO_Pin_6},//	  
    {GPIOB, GPIO_Pin_10},   
    {GPIOB, GPIO_Pin_11}, 
	{GPIOC, GPIO_Pin_9},		//    {GPIOC, GPIO_Pin_7},//    	
    {GPIOC, GPIO_Pin_6},		// 
    {GPIOB, GPIO_Pin_9}	,		//
	{GPIOA, GPIO_Pin_8} 		//

};

static struct io_port key2_input[5] =
{
	{GPIOB, GPIO_Pin_5},//  {GPIOC, GPIO_Pin_9}, //        3
    	
	{GPIOB, GPIO_Pin_7},//    {GPIOB, GPIO_Pin_11},       1  
    {GPIOB, GPIO_Pin_6},    							//4
	{GPIOB, GPIO_Pin_8},//    {GPIOB, GPIO_Pin_10},        2
	
	{GPIOC, GPIO_Pin_7},//                                 0  
};

//邮箱和消息队列
//extern OS_EVENT  *RS232LogQ,*USBMBox,*MessageQ,*ADsem,*ADQ,*key1Box,*key2Box,*BEEPsem;

//extern	volatile u8 price_clear_bz;
//extern	volatile int totalpice[10];
//extern	volatile unsigned char bz_n;
//extern  volatile u8 total_clear_bz;
//extern  volatile u8 price_lock_bz;
//extern  volatile u32 beep_number;
//extern  volatile u8 pice_data[3];		//单价数值
//extern 	volatile u8 price_clear_bz;
//extern 	volatile u32 place_bz;
//extern 	volatile u8 price_lock_bz;
//extern  volatile u32 weight_char_bz;
//extern  volatile u8 num;				//总价计算变量
//extern  volatile u8 total_clear_bz;
//extern 	volatile unsigned char bz_block[10];
//extern 	volatile u32 place_bz;
//extern 	volatile u8 price_lock_bz;
//extern 	volatile unsigned int Acc_totalprice;
//extern	volatile int plu[10];
//extern	volatile int price[10];
//extern	volatile u8 bz_updata;
//extern 	volatile int weight[10];
//extern	char codetest[260];
//extern 	volatile u8 bz_updata;
//extern 	volatile u8 remove_clear_bz;

//extern	volatile u8 price_lock_bz;

//extern  int key_map[50];

//extern 	u8 err_id[3][5];
//------------------------------------------------------------
//void key2_display(u8 anjian);
//void keypad(void);
void Key1_scan_init(void);
void Key2_scan_init(void);
void K_init(void);
void pro_mode(void);
u8 key1_scan(void);
//void key_task(void *pdata);
//void key1_process(u8 key_num);
//------------------------------------------------------------

//extern volatile u32 Zerotracking_data;	 	//零点数值
//extern volatile unsigned char AD_DATA[3];	//重量数值
//extern volatile u32 Acc_bz;
//extern volatile u32 TO_zero_bz; 
//extern volatile u8 remove_cat_bz;
//extern volatile u32 calculate_weight;		//计算重量
//extern volatile u8 PT_stabilize_bz;
//extern volatile u32 cat_weight;				//皮重 

//u8 key1_exchange( u8 anjian);

#endif



