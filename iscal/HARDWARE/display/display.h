#ifndef		_DISPLAY
#define 	_DISPLAY





void display(void);
extern void lcd_init(void);

extern void LCD_GPIO_Config(void);

//------------------------------------
void Cursor_task(void *pdata);
void lcd_display_accprice(void);
void lcd_display(unsigned char *ad_data);
void weight_dis(int weight,unsigned char *weightflag);
void price_dis(unsigned int price);
void total_dis(unsigned int total);
void sumtotal_dis(unsigned int sumtotal);
void pi_dispaly(int pi);




#endif


