#ifndef		_LCD
#define 	_LCD

#include "stm32f10x.h"


//��Ļ��ʼʱ��ʾ��ʽ��ע�⣺��IDelayʱ��ʾ��һ��������������ˢ�µ�
//��ʱ�����ֶ���ˢ�½��������  LCD_WR_REG(0x0007,0x0173);������ʾ
//��Immediatelyʱû�б�ע�͵��ǲ���Ҫ�˹���

/* ѡ��BANK1-BORSRAM1 ���� TFT����ַ��ΧΪ0X60000000~0X63FFFFFF
 * FSMC_A16 ��LCD��DC(�Ĵ���/����ѡ��)��
 * 16 bit => FSMC[24:0]��ӦHADDR[25:1]
 * �Ĵ�������ַ = 0X60000000
 * RAM����ַ = 0X60020000 = 0X60000000+2^16*2 = 0X60000000 + 0X20000 = 0X60020000
 * ��ѡ��ͬ�ĵ�ַ��ʱ����ַҪ���¼��㡣
 */
#define Bank1_LCD_D    ((u32)0x60020000)    //Disp Data ADDR
#define Bank1_LCD_C    ((u32)0x60000000)	   //Disp Reg ADDR

#define Set_Rst GPIOD->BSRR = GPIO_Pin_13;
#define Clr_Rst GPIOD->BRR  = GPIO_Pin_13;

#define Lcd_Light_ON   GPIOC->BSRR = GPIO_Pin_5;
#define Lcd_Light_OFF  GPIOC->BRR  = GPIO_Pin_5;

//  ******************************************************************************
//  * @file   �����ض��� 
//  * $Author: Du Mingxing $
// * $Revision: 67 $
//  * $Date:: 2016-05-27 10:00 #$
// * @brief:
//  ******************************************************************************
#define	LCD_CS_SET  GPIO_SetBits(GPIOD,GPIO_Pin_7)    //Ƭѡ�˿�  		PD7
#define	LCD_RS_SET	GPIO_SetBits(GPIOD,GPIO_Pin_11)    //����/���� 		PD11	   
#define	LCD_WR_SET	GPIO_SetBits(GPIOD,GPIO_Pin_5)   //д����			PD5
#define	LCD_RD_SET	GPIO_SetBits(GPIOD,GPIO_Pin_4)   //������			PD4
								    
#define	LCD_CS_CLR  GPIO_ResetBits(GPIOD,GPIO_Pin_7)    //Ƭѡ�˿�  		PD7
#define	LCD_RS_CLR	GPIO_ResetBits(GPIOD,GPIO_Pin_11)     //����/����		PD11   
#define	LCD_WR_CLR	GPIO_ResetBits(GPIOD,GPIO_Pin_5)     //д����			PD5
#define	LCD_RD_CLR	GPIO_ResetBits(GPIOD,GPIO_Pin_4)    //������			PD4 



//LCD��Ҫ������
typedef struct  
{										    
	u16 width;			//LCD ���
	u16 height;			//LCD �߶�
	u16 id;				//LCD ID
	u8  dir;			//���������������ƣ�0��������1��������	
	u16	wramcmd;		//��ʼдgramָ��
	u16 setxcmd;		//����x����ָ��
	u16  setycmd;		//����y����ָ��	 
}_lcd_dev; 	


extern _lcd_dev lcddev;	
//Lcd��ʼ������ͼ����ƺ���
void Lcd_Configuration(void);
void DataToWrite(u16 data);
void Lcd_Initialize(void);
void LCD_WR_REG(u16 Index,u16 CongfigTemp);
void Lcd_WR_Start(void);
//Lcd�߼����ƺ���
void Lcd_ColorBox(u16 x,u16 y,u16 xLong,u16 yLong,u16 Color);
void DrawPixel(u16 x, u16 y, u16 Color);
void LCD_Fast_DrawPoint(u16 x,u16 y,u16 color);
u16 ssd1289_GetPoint(u16 x,u8 y);
void LCD_Fill_Pic(u16 x, u16 y,u16 pic_H, u16 pic_V, const unsigned char* pic);
void BlockWrite(unsigned int Xstart,unsigned int Xend,unsigned int Ystart,unsigned int Yend);
char Tiky_Button(char *filename,u16 x,u16 y);
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2);
void LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p);
void LCD_ShowChar(u16 x,u16 y,u8 num,u8 size,u8 mode);
void LCD_ShowChar8(u16 x,u16 y,u8 num,u8 size,u8 mode);
void LCD_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 color);
void ShowChar(u16 x,u16 y,u8 num,u8 size,u8 mode,u16 fornt,u16 back);
void ShowChar8(u16 x,u16 y,u8 num,u8 size,u8 mode,u16 fornt,u16 back);
void LCD_ShowString8(u16 x,u16 y,u16 width,u16 height,u8 length,u8 *p);
void LCD_ShowStringNum(u16 x,u16 y,u16 width,u16 height,u8 length,u8 *p);
void LCD_ShowString8_red(u16 x,u16 y,u16 width,u16 height,u8 length,u8 *p);
/*���峣����ɫ*/
// #define red 0x001f
// #define blue 0xf800
// #define green 0x07e0
// #define black 0x0000
// #define white 0xffff
// #define yellow 0x07ff
// #define orange 0x05bf
// #define Chocolate4 0x4451
// #define Grey 0xefbd//��ɫ

#define White          0xFFFF
#define Black          0x0000
#define Blue           0x001F
#define Blue2          0x051F
#define Red            0xF800
#define Magenta        0xF81F
#define Green          0x07E0
#define Cyan           0x7FFF
#define Yellow         0xFFE0

#endif
