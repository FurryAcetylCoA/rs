//
// Created by 11096 on 2022/10/31.
//

#include "lcd_server.h"
#include "lcd_gxct.h"
#include "stdio.h"
#include "data.h"

#define LcdPrint(_LINE_,...) do{sprintf((char*)lcd_buffer,__VA_ARGS__); \
                  LCD_ShowStringLine(_LINE_,lcd_buffer);}while(0)

static char    lcd_buffer[32];

static void lcd_server_empyrean();
static void lcd_server_earth();

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
            LCD_ShowStringLine(LINE1,"ST_Silver_key");
            LCD_ShowStringLine(LINE2,"Ready for reading        ");
            LCD_ShowStringLine(LINE3,"Press any key to continue");
            break;
        case ST_Earth:
            lcd_server_earth();
            break;
        case ST_Golden_Key:
            //LCD_clearLine(LINE2);//这里tmd会闪
            //LCD_clearLine(LINE3);
            LCD_ShowStringLine(LINE1,"ST_Golden_key");
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
        LcdPrint(LINE7,"RESET:exit"); //当真？
        LcdPrint(LINE8,"UP/DOWN:select");
        LcdPrint(LINE9,"RIGHT:enter");
    }else if(This.su.ES.es_state == ES_Conform){
        LcdPrint(LINE1,"For %dth device.",This.config.dev_count+1);
        LcdPrint(LINE2,"You select:");
        LCD_push(GREEN);
        LcdPrint(LINE3,"###%s",devDesc[This.su.ES.es_select].name);
        LCD_pop();
        LcdPrint(LINE4,"Data1: %s   Data2: %s","YES",(devDesc[This.su.ES.es_select].data2.exist == 1?"YES":"NO"));
        LcdPrint(LINE6,"Connect ONLY this device");
        LcdPrint(LINE7,"Press RIGHT to register it");
        LcdPrint(LINE8,"Or press LEFT to cancel");
    }else if (This.su.ES.es_state == ES_Programing){
        LcdPrint(LINE1,"Programming the device");
        LcdPrint(LINE2,"With address: %x",This.config.dev_count+1);
        if (This.su.ES.es_programing_step < 1){ return; }
        LcdPrint(LINE3,"Programming ... Done");
        if (This.su.ES.es_programing_step < 2){ return; }
        LcdPrint(LINE4,"Test read result");
        s_data.Print(lcd_buffer,This.config.dev_count);//这种情况下，devcount已经被加过了
        LCD_ShowStringLine(LINE5,lcd_buffer);
        LcdPrint(LINE6,"Press RIGHT if OK");
        LcdPrint(LINE7,"Press UP to try again");
        LcdPrint(LINE8,"Press LEFT to go back");
    }else if (This.su.ES.es_state == ES_Yet_Another){
        LcdPrint(LINE1,"Device %d added",This.config.dev_count);

        LcdPrint(LINE8,"Press UP to add more");
        LcdPrint(LINE9,"Press RIGHT to exit");
    }else if (This.su.ES.es_state == ES_Full){
        LCD_push(MAGENTA);//this supposed to be some kind of red
        LcdPrint(LINE1,"Warning:");
        LCD_pop();
        LcdPrint(LINE1,"######## device list is full!");
        LcdPrint(LINE2,"Maximum of device count is %d",lenof(This.devs));
        LcdPrint(LINE3,"You have to remove some");
        LcdPrint(LINE4,"Before add another");
        LcdPrint(LINE9,"Press any key to continue");
    }

}
/**
// @brief: 显示所有已注册的传感器信息
// 格式为：
//     <传感器名称><':'>
//    <'Data1:'><D1><D1unit> [<'Data2:'><D2><D2unit>]
// 当一屏显示不下时，接受按键翻页(todo:自动翻页？）
 */
static void lcd_server_earth(){
    int i = 0;
    App_dev_desc   *ndev = &This.devs[0];
    Dev_desc const *ddev = &devDesc[0];
    while(i < MAX_DEV_COUNT && i < 6/2){//最高行和最低和次低行分别是标题和底栏
        if(i+This.su.EA.current_top >= This.config.dev_count){ break; }//确保接下来只访问有效传感器
        ndev = &This.devs[i+This.su.EA.current_top];
        ddev = &devDesc[ndev->sens_desc.name_index];

        LcdPrint(i,"%s", ddev->name);
        if(ndev->sens_desc.data2.exist == 0){
            LcdPrint(i+1,"Data1:%ld%s",ndev->data1,ddev->data1_unit);
        }else{
            LcdPrint(i+1,"Data1:%ld%s Data2:%ld%s",ndev->data1,ddev->data1_unit,ndev->data2,ddev->data2_unit);
        }
        i++;
    }
    if(This.config.dev_count > 8){
        LcdPrint(LINE9,"UP/DOWN:roll");
    }

}