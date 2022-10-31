//
// Created by 11096 on 2022/10/31.
//

#include "lcd_server.h"
#include "lcd_gxct.h"
#include "stdio.h"
#define LcdPrint(_LINE_,...) do{sprintf((char*)lcd_buffer,__VA_ARGS__); \
                  LCD_ShowStringLine(_LINE_,lcd_buffer);}while(0)

static char    lcd_buffer[32];

void lcd_server(){
    switch (This.state) {
        case ST_Genesis:
            //does nothing
            break;
        case ST_saint_peter:
            //does nothing
            LCD_clearLine(LINE1);
            LCD_clearLine(LINE2);
            LCD_clearLine(LINE3);
            LcdPrint(LINE2,"Read eeprom for config...%s",(This.config.eeprom_ready ==1?"OK":"Failed"));
            break;
        case ST_Silver_Key:
            LCD_clearLine(LINE1);
            LCD_clearLine(LINE2);
            LCD_clearLine(LINE3);
            LCD_ShowStringLine(LINE2,"Ready for reading");
            LCD_ShowStringLine(LINE3,"Press any key to continue");
    }

}