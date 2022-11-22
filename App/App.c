//
// Created by 11096 on 2022/10/26.
//

#include "App.h"
#include "string.h"
#include "stdio.h"
#include "tictok.h"
#include "data.h"
#include "lcd_gxct.h"
#include "sensor.h"
#include "lcd_server.h"
#include "HanZi.h"
#include "onenet.h"

//所有支持的传感器
const Dev_desc devDesc[]={                  //为了方便字库的操作。用双引号代替°
        {.name="Air Temp&Humidity",.inst_sized=0, .data2.exist=1,
        .data1_display_name="温度" , .data2_display_name="湿度",.data1_name="Temperature",.data2_name="Humidity",
        .data1_unit="\"C"  ,.data1.factor=10, .data1.is_signed=1,.data1.mult_or_div=1,
        .data2_unit="%RH",  .data2.factor=10, .data2.is_signed=1 },

        {.name="CO2"              ,.inst_sized=0 ,.data2.exist=0,
        .data1_display_name="二氧化碳浓度",.data1_name="",
        .data1_unit="ppm"  ,.data1.factor=1  ,.data1.is_signed=0,.data1.mult_or_div=1},

        {.name="Soil Conductance" ,.inst_sized=0,.data2.exist=0,
        .data1_display_name="土壤电导率",.data1_name="",
        .data1_unit="mS/cm",.data1.factor=100,.data1.is_signed=1,.data1.mult_or_div=1},

        {.name="PH"            ,.inst_sized=1,.data2.exist=0,
        .data1_display_name="PH值" ,.data1_name="",
        .data1_unit="pH",.data1.factor=100,.data1.is_signed=0,.data1.mult_or_div=1 },

        {.name="Illuminance" ,.inst_sized=0,.data2.exist=0,
         .data1_display_name="光强",.data1_name="",
         .data1_unit="Lux",.data1.factor=10,.data1.is_signed=0,.data1.mult_or_div=0}

};//请只在最后添加新传感器。否则会影响已有传感器



static void ST_Empyrean_Program_dev();
static void App_init();
static void State_go(States next_state);
static void State_server(void);
static void on_error (const char*);



App_info This={
        .init     = App_init,
        .state_go = State_go,
        .state_server = State_server,
        .on_error = on_error
};


static void App_init(){
    This.total_dev=sizeof(devDesc)/sizeof (devDesc[0]);//必须要在这个文件里取，因为siezof是编译期确定，而extern 是链接期确定
    tictok.Init();
    LCD_Init(GRAYBLUE);
    HanZi_init();

    This.state_go(ST_Genesis);
}

/**
* @brief 状态机的组合逻辑输出（即在某状态时持续发生的操作）
* 自动转移会写在这里
* @retval None.
*/
static void State_server(){
    switch (This.state) {
        case ST_saint_peter:
            //eeprom读取成功+有设备注册->silver_key
            if(This.config.eeprom_ready == 1){
                if(This.config.dev_count != 0 ){
                    State_go(ST_Silver_Key);
                }else{
                    State_go(ST_Golden_Key);
                }
            }break;
        case ST_Empyrean:
            if (This.su.ES.es_state == ES_Programing){
                ST_Empyrean_Program_dev();
            }
            break;
        default:
            break;
    }
}

/**
* @brief 操作This状态机转移状态
* 一次性操作发生在这里
* @param next_state: next state
* @retval None.
*/
static void State_go(States next_state){
    //状态转移
    if(This.state == ST_Limbo){
        return; //limbo 状态不允许移出
    }
    lcd_state_go(next_state);

    switch (next_state) {
        case ST_Genesis:
            This.state=ST_Genesis;
            break;
        case ST_Silver_Key:
            EE_Load(&This);
            This.state=ST_Silver_Key;
            LCD_clearLineAll();
            break;
        case ST_saint_peter:
            EE_Load(&This);//读入EEPROM配置
            This.state=ST_saint_peter;
            break;
        case ST_Earth:
            //填充所有数据，然后注册数据中间件轮询到时钟中心
            //onenet事件注册在这里
            //如果将来会离开earth。请记得要注销onenet
            memset(&This.su,0,sizeof(This.su));

            s_data.Pollall();
            if(This.config.dev_count <=2) {
                tictok.Add(s_data.Poll, 1000, false);
            }else{
                tictok.Add(s_data.Poll,300,false);
            }

            This.onenet_poll_tictok_ID=tictok.Add(onenet.Poll,10000,false);
            onenet.Pollall();

            This.state=ST_Earth;
            LCD_clearLineAll();
            break;
        case ST_Golden_Key:
            //进入设备注册
            This.state=ST_Golden_Key;
            break;
        case ST_Empyrean:
            EE_Load(&This);
            memset(&This.su, 0, sizeof(This.su));
            if(This.config.dev_count>=lenof(This.devs)){
                This.su.ES.es_state = ES_Full;
            }
            This.state=ST_Empyrean;
            break;
        case ST_Limbo:
            tictok.Wipe();
            This.state=ST_Limbo;
            break;
        default:
            _TRAP;

    }
}

//与注册功能相关的过程
//根据当前的选择，填充一个设备描述符
//然后根据当前步骤数，逐一进行：
//   0）描述符产生与sens_SetAddr调用
//   1）尝试通过s_data.PollOne读取
//   2）读取成功 ——> 修改dev_count & 写入EEPROM
//控制步骤数的变量在es_programing_step中
static void ST_Empyrean_Program_dev(){
    App_dev_desc   *ndev = &This.devs[This.config.dev_count];
    switch (This.su.ES.es_programing_step) {
        case 0:{

            Dev_desc const *ddev = &devDesc[This.su.ES.es_select];
            ndev->sens_desc.name_index = This.su.ES.es_select;
            ndev->sens_desc.inst_sized = ddev->inst_sized;
            //memcpy(ndev->name,ddev->name, strlen((const char*)ddev->name));
            ndev->sens_desc.data1.factor      = ddev->data1.factor;
            ndev->sens_desc.data1.is_signed   = ddev->data1.is_signed;
            ndev->sens_desc.data1.mult_or_div = ddev->data1.mult_or_div;
            if (ddev->data2.exist == 1){
                ndev->sens_desc.data2.exist     = 1;
                ndev->sens_desc.data2.factor    = ddev->data2.factor;
                ndev->sens_desc.data2.is_signed = ddev->data2.is_signed;
            }
            sens_SetAddr(&ndev->sens_desc,This.config.dev_count+1);
        }   This.su.ES.es_programing_step++;
            break;
        case 1:
            s_data.PollOne(This.config.dev_count +1 -1);
            This.su.ES.es_programing_step++;
            break;
        case 2:
            //nothing
            break;
        default:
            break;

    }
}



static void on_error (const char* err){
    This.state_go(ST_Limbo);
    LCD_push(RED);
    char buffer[26];
    snprintf((char*)buffer,25,"ERR:%s     ",err);
    LCD_ShowStringLine(LINE10, buffer);
    LCD_pop();
}
