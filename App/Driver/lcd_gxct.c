#include "lcd_gxct.h"


//��������Y��ռ24���ء���ʮ�п���
//��������X��ռ12���ء���26�п���

//������ʾ
void LCD_ShowStringLine(LCD_Line line,const uint8_t *p){
		
	uint16_t x = 0,y = line*24;
	const uint8_t  escape='#';

	
	while ((*p <= '~') && (*p >= ' ')||*p==escape)   //�ж��ǲ��ǷǷ��ַ�!
	{
			if(*p != escape){
				LCD_ShowChar(x, y, *p, 24, 0);
			}
			x += 24 / 2;
			p++;
	}  
}