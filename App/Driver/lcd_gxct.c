#include "lcd_gxct.h"

static uint32_t LCD_background_stack; //LEN = 1
//所用字体Y轴占24像素。共十行可用
//所用字体X轴占12像素。共26列可用

//按行显示
void LCD_ShowStringLine(LCD_Line line, const char *p){
		
	uint16_t x = 0,y = line*24;
	const uint8_t  escape='#';

	
	while ((*p <= '~') && (*p >= ' ')||*p==escape)   //判断是不是非法字符!
	{
			if(*p != escape){
				LCD_ShowChar(x, y, *p, 24, 0);
			}
			x += 24 / 2;
			p++;
	}  
}

void LCD_clearLine(LCD_Line line){
    LCD_ShowStringLine(line,"                          ");
}
void LCD_push(uint32_t new_background){
	LCD_background_stack = BACK_COLOR;
	BACK_COLOR = new_background;
}

void LCD_pop(){
	BACK_COLOR = LCD_background_stack;
}