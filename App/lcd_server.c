//
// Created by 11096 on 2022/10/31.
//

#include "lcd_server.h"
#include "lcd_gxct.h"
#include "stdio.h"
#define LcdPrint(_LINE_,...) do{sprintf((char*)lcd_buffer,__VA_ARGS__); \
                  LCD_ShowStringLine(_LINE_,lcd_buffer);}while(0)

static char    lcd_buffer[32];

static void lcd_server_empyrean();

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
            LcdPrint(LINE2,"Reading eeprom...%s",(This.config.eeprom_ready ==1?"OK":"Failed"));
            HAL_Delay(1000);//这里等一会应该没关系
            LCD_clearLine(LINE1);
            LCD_clearLine(LINE2);
            LCD_clearLine(LINE3);
            break;
        case ST_Silver_Key:
            LCD_ShowStringLine(LINE2,"Ready for reading        ");
            LCD_ShowStringLine(LINE3,"Press any key to continue");
            break;
        case ST_Earth:
            break;
        case ST_Golden_Key:
            //LCD_clearLine(LINE2);//这里tmd会闪
            //LCD_clearLine(LINE3);
            LCD_ShowStringLine(LINE3,"Ready for registering     ");
            LCD_ShowStringLine(LINE4,"Press any key to continue ");
            break;
        case ST_Empyrean:
            lcd_server_empyrean();
            break;
        default:
            _TRAP;
    }

}

static void lcd_server_empyrean(){
    if(This.su.ES.es_state == ES_Devname) {
        LcdPrint(LINE1,"%d device registered",This.config.dev_count);
        LcdPrint(LINE2,"Now registering %d device",This.config.dev_count+1);
        LcdPrint(LINE3,"Please select device type:");
        if(This.su.ES.es_select_changed == 1){
            This.su.ES.es_select_changed = 0;
            LCD_clearLine(LINE4);
        }
        LCD_push(GREEN);
        LcdPrint(LINE4,"###%s",devDesc[This.su.ES.es_select].name);
        LCD_pop();

        LcdPrint(LINE8,"Up/Down:select");
        LcdPrint(LINE9,"Right:enter");
    }else if(This.su.ES.es_state == ES_Conform){
        LcdPrint(LINE1,"For %dth device.",This.config.dev_count+1);
        LcdPrint(LINE2,"You select:");
        LCD_push(GREEN);
        LcdPrint(LINE3,"###%s",devDesc[This.su.ES.es_select].name);
        LCD_pop();
        LcdPrint(LINE4,"Data1: %s   Data2: %s","YES",(devDesc[This.su.ES.es_select].data2.exist == 1?"YES":"NO"));
        LcdPrint(LINE6,"Connect ONLY this device");
        LcdPrint(LINE7,"Press right to register it");
        LcdPrint(LINE8,"Or press left to cancel");
    }else if (This.su.ES.es_state == ES_Programing){
        LcdPrint(LINE1,"Programming the device");
        LcdPrint(LINE2,"With address: %x",This.config.dev_count+1);
        if (This.su.ES.es_programing_step < 1){ return; }
        LcdPrint(LINE3,"Programming ... Done");
        if (This.su.ES.es_programing_step < 2){ return; }
        LcdPrint(LINE4,"Testing");
        LcdPrint(LINE5,"D1:%ld D2:%ld",This.devs[This.config.dev_count].data1,This.devs[This.config.dev_count].data2);

    }

}