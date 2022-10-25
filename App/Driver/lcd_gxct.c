#include "lcd_gxct.h"


//所用字体Y轴占24像素。共十行可用
//所用字体X轴占12像素。共26列可用

//按行显示
void LCD_ShowStringLine(LCD_Line line,const uint8_t *p){
		
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