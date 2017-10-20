#ifndef __SWITCH_OPS_H
#define __SWITCH_OPS_H

void init_switch(void);
int open_switch(unsigned char index);
int close_switch(unsigned char index);

unsigned char get_input_status(unsigned char index);
unsigned char get_light_zouyou_status(void);
unsigned char get_light_shangxia_status(void);

unsigned char get_feiye_fuzi_status(void);
unsigned char get_qingxiye_fuzi_status(void);
unsigned char get_xishiye_fuzi_status(void);

unsigned char get_start_key_status(void);   //获取 启动按键 状态
unsigned char get_xckg_ybh_status(void);    //获取 样本盒 行程开关 状态
unsigned char get_xckg_sjk_status(void);    //获取 试剂卡 行程开关 状态

unsigned char get_shijika_key_status(void);
unsigned char get_qingxi_key_status(void);
unsigned char get_start_key_status(void);

unsigned char get_go_status(unsigned char index);  //获取 光耦 状态 
unsigned char get_all_go_status(unsigned char go_type,unsigned char *buf);

void check_go(unsigned char go_type);

#endif
