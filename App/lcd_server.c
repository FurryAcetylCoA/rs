//
// Created by 11096 on 2022/10/31.
//

#include "lcd_server.h"
#include "lcd_gxct.h"
#include "stdio.h"
#include "data.h"

#define LcdPrint(_LINE_,...) do{sprintf((char*)lcd_buffer,__VA_ARGS__); \
                  LCD_ShowStringLineEx(_LINE_,lcd_buffer);}while(0)

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
            LcdPrint(LINE2,"读取 eeprom...%s",(This.config.eeprom_ready ==1?"成功":"失败"));
            HAL_Delay(1000);//这里等一会应该没关系
            LCD_clearLine(LINE1);
            LCD_clearLine(LINE2);
            LCD_clearLine(LINE3);
            break;
        case ST_Silver_Key:
            LCD_ShowStringLineEx(LINE1,"ST_Silver_key");
            LCD_ShowStringLineEx(LINE2,"系统准备完成  ");
            LCD_ShowStringLineEx(LINE3,"按任意键开始读取");

            LCD_ShowStringLine(LINE7,"UP: Register more device");
            LCD_ShowStringLine(LINE8,"DOWN: Remove all device !");
            LCD_ShowStringLine(LINE8,"OTHER:continue reading");
            break;
        case ST_Earth:
            lcd_server_earth();
            break;
        case ST_Golden_Key:
            //LCD_clearLine(LINE2);//这里tmd会闪
            //LCD_clearLine(LINE3);
            LCD_ShowStringLineEx(LINE1,"ST_Golden_key");
            LCD_ShowStringLineEx(LINE3,"没有已注册设备   ");
            LCD_ShowStringLineEx(LINE4,"按任意键开始注册 ");
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
        LcdPrint(LINE1,"已注册%d个设备",This.config.dev_count);
        LcdPrint(LINE2,"现在注册%d号设备",This.config.dev_count+1);
        LcdPrint(LINE3,"请选择设备类型:");
        if(This.su.ES.es_select_changed == 1){
            This.su.ES.es_select_changed = 0;
            LCD_clearLine(LINE4);
        }
        LCD_push(GREEN);
        LcdPrint(LINE4,"###%s",devDesc[This.su.ES.es_select].name);
        LCD_pop();
        LcdPrint(LINE7,"RESET:退出"); //当真？
        LcdPrint(LINE8,"UP/DOWN:选择");
        LcdPrint(LINE9,"RIGHT:确认");
    }else if(This.su.ES.es_state == ES_Conform){
        LcdPrint(LINE1,"对于设备 %d 你选择了",This.config.dev_count+1);
        LCD_push(GREEN);
        LcdPrint(LINE2,"###%s",devDesc[This.su.ES.es_select].name);
        LCD_pop();
        LcdPrint(LINE3,"数据位1: %s ",devDesc[This.su.ES.es_select].data1_display_name);
        if(devDesc[This.su.ES.es_select].data2.exist == 1){
            LcdPrint(LINE4,"数据位2: %s ",devDesc[This.su.ES.es_select].data2_display_name);
        }else{
            LcdPrint(LINE4,"数据位2: %s ","无");
        }
        LcdPrint(LINE6,"请只连接这个设备");
        LcdPrint(LINE7,"按下 RIGHT 确认");
        LcdPrint(LINE8,"按下 LEFT 回上一页");
    }else if (This.su.ES.es_state == ES_Programing){
        LcdPrint(LINE1,"写入器件地址");
        LcdPrint(LINE2,"目标地址: %x",This.config.dev_count+1);
        if (This.su.ES.es_programing_step < 1){ return; }
        LcdPrint(LINE3,"写入器件地址... 完成");
        if (This.su.ES.es_programing_step < 2){ return; }
        LcdPrint(LINE4,"读数测试");
        s_data.Print(lcd_buffer,This.config.dev_count);//这种情况下，devcount已经被加过了
        LCD_ShowStringLine(LINE5,lcd_buffer);
        LcdPrint(LINE6,"按 RIGHT 确认");
        LcdPrint(LINE7,"按 UP 重试");
        LcdPrint(LINE8,"按 LEFT 回上一页");
    }else if (This.su.ES.es_state == ES_Yet_Another){
        LcdPrint(LINE1,"设备 %d 已添加",This.config.dev_count);

        LcdPrint(LINE8,"按 UP 继续添加");
        LcdPrint(LINE9,"按 RIGHT 退出");
    }else if (This.su.ES.es_state == ES_Full){
        LCD_push(MAGENTA);//this supposed to be some kind of red
        LcdPrint(LINE1,"请注意:");
        LCD_pop();
        LcdPrint(LINE1,"######## 设备列表已满!");
        LcdPrint(LINE2,"最多支持%d个设备",lenof(This.devs));
        LcdPrint(LINE3,"如果想继续添加");
        LcdPrint(LINE4,"必须先移除一些设备");
        LcdPrint(LINE9,"按 任意键 继续");
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
            LcdPrint(i+1,"%s:%ld%s",ddev->data1_display_name,ndev->data1,ddev->data1_unit);
        }else{ //todo:这里很可能一行显示不下了。需要重新设计
            LcdPrint(i+1,"%s:%ld%s %s:%ld%s",ddev->data1_display_name,ndev->data1,ddev->data1_unit,\
                                             ddev->data2_display_name,ndev->data2,ddev->data2_unit);
        }
        i++;
    }
    if(This.config.dev_count > 8){
        LcdPrint(LINE9,"UP/DOWN:roll");
    }

}


/***
 * @brief: 用于在不同模式切换时，修改底栏的内容
 * 之所以这个函数单独拎出来，是因为如果这种需要清行再显示的东西如果放在
 * lcd_server会导致屏幕闪烁
 * @param: next_state: 即将转移的状态
 ***/
void lcd_state_go(States next_state){
    LCD_clearLineAll();
    LCD_push(BLUE);
    switch (next_state) {
        case ST_Empyrean:
            LCD_clearLine(LINE10);
            LCD_ShowStringLineEx(LINE10,"设备注册界面");
            break;
        case ST_Earth:
            LCD_clearLine(LINE10);
            LCD_ShowStringLineEx(LINE10,"数据读取页面");
            break;
        case ST_Genesis:
            LCD_clearLine(LINE10);
            LCD_ShowStringLineEx(LINE10,"            构建时间:"__TIME__);
        default:
            break;
    }
    LCD_pop();
}