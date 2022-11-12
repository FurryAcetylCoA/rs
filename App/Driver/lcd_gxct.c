#include "lcd_gxct.h"
#include "HanZi.h"
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

//与LCD_ShowStringLine类似。但支持中文，不支持escape符
void LCD_ShowStringLineEx(LCD_Line line, char *p){

    uint16_t x = 0,y = line*24;
    const uint8_t  escape='#';


    uint16_t isAscii = 1;

    while( * p != 0){
        if (isAscii){
            if (*p > 0x80){
                isAscii = 0;
                //break;
            }else{
                LCD_ShowChar(x,y,*p,24,0);
                p++;
                x+=24/2;
            }
        }else{
            isAscii = 1;
            Show_Font(x,y,p,24,0);
            p+=2;
            x+=24;
        }

    }
}

void LCD_clearLine(LCD_Line line){ //优化了一下 用更底层的方式（填充实心区域）来做
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
//大小只有1的屑堆栈
void LCD_push(uint32_t new_background){
	LCD_background_stack = BACK_COLOR;
	BACK_COLOR = new_background;
}

void LCD_pop(){
	BACK_COLOR = LCD_background_stack;
}