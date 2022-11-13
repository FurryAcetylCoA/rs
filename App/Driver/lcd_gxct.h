#ifndef __LCD_GXCT_H__
#define __LCD_GXCT_H__

#include <lcd.h>
////////�ṩGXCT����LCD API////////
//Y:240*X:320
//��������Y��ռ24���ء���ʮ�п���
//��������X��ռ12���ء���26�п���
typedef enum{
	LINE1 =0,
	LINE2 =1,
	LINE3 =2,
	LINE4 =3,
	LINE5 =4,
	LINE6 =5,
	LINE7 =6,
	LINE8 =7,
	LINE9 =8,
	LINE10=9
}LCD_Line;

void LCD_ShowStringLine(LCD_Line line, const char *p);
void LCD_ShowStringLineEx(LCD_Line line, char *p);
void LCD_clearLine(LCD_Line line);
void LCD_clearLineAll();
void LCD_push(uint32_t new_background);
void LCD_pop();
#endif  //__LCD_GXCT_H__
