#include "lcd_gxct.h"

static uint32_t LCD_background_stack; //LEN = 1
//��������Y��ռ24���ء���ʮ�п���
//��������X��ռ12���ء���26�п���

//������ʾ
void LCD_ShowStringLine(LCD_Line line, const char *p){
		
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

void LCD_clearLine(LCD_Line line){ //�Ż���һ�� �ø��ײ�ķ�ʽ�����ʵ����������
    LCD_Fill(0,line*24,320,line*24+24,BACK_COLOR);
}

void LCD_clearLineAll(){
    LCD_clearLine(LINE1);
    LCD_clearLine(LINE2);
    LCD_clearLine(LINE3);
    LCD_clearLine(LINE4);
    LCD_clearLine(LINE5);
    LCD_clearLine(LINE6);
    LCD_clearLine(LINE7);
    LCD_clearLine(LINE8);
    LCD_clearLine(LINE9);
}
//��Сֻ��1��м��ջ
void LCD_push(uint32_t new_background){
	LCD_background_stack = BACK_COLOR;
	BACK_COLOR = new_background;
}

void LCD_pop(){
	BACK_COLOR = LCD_background_stack;
}