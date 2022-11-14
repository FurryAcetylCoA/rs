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
            LcdPrint(LINE2,"��ȡ eeprom...%s",(This.config.eeprom_ready ==1?"�ɹ�":"ʧ��"));
            HAL_Delay(1000);//�����һ��Ӧ��û��ϵ
            LCD_clearLine(LINE1);
            LCD_clearLine(LINE2);
            LCD_clearLine(LINE3);
            break;
        case ST_Silver_Key:
            LCD_ShowStringLineEx(LINE1,"ST_Silver_key");
            LCD_ShowStringLineEx(LINE2,"ϵͳ׼�����  ");
            LCD_ShowStringLineEx(LINE3,"���������ʼ��ȡ");

            LCD_ShowStringLine(LINE7,"UP: Register more device");
            LCD_ShowStringLine(LINE8,"DOWN: Remove all device !");
            LCD_ShowStringLine(LINE8,"OTHER:continue reading");
            break;
        case ST_Earth:
            lcd_server_earth();
            break;
        case ST_Golden_Key:
            //LCD_clearLine(LINE2);//����tmd����
            //LCD_clearLine(LINE3);
            LCD_ShowStringLineEx(LINE1,"ST_Golden_key");
            LCD_ShowStringLineEx(LINE3,"û����ע���豸   ");
            LCD_ShowStringLineEx(LINE4,"���������ʼע�� ");
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
        LcdPrint(LINE1,"��ע��%d���豸",This.config.dev_count);
        LcdPrint(LINE2,"����ע��%d���豸",This.config.dev_count+1);
        LcdPrint(LINE3,"��ѡ���豸����:");
        if(This.su.ES.es_select_changed == 1){
            This.su.ES.es_select_changed = 0;
            LCD_clearLine(LINE4);
        }
        LCD_push(GREEN);
        LcdPrint(LINE4,"###%s",devDesc[This.su.ES.es_select].name);
        LCD_pop();
        LcdPrint(LINE7,"RESET:�˳�"); //���棿
        LcdPrint(LINE8,"UP/DOWN:ѡ��");
        LcdPrint(LINE9,"RIGHT:ȷ��");
    }else if(This.su.ES.es_state == ES_Conform){
        LcdPrint(LINE1,"�����豸 %d ��ѡ����",This.config.dev_count+1);
        LCD_push(GREEN);
        LcdPrint(LINE2,"###%s",devDesc[This.su.ES.es_select].name);
        LCD_pop();
        LcdPrint(LINE3,"����λ1: %s ",devDesc[This.su.ES.es_select].data1_display_name);
        if(devDesc[This.su.ES.es_select].data2.exist == 1){
            LcdPrint(LINE4,"����λ2: %s ",devDesc[This.su.ES.es_select].data2_display_name);
        }else{
            LcdPrint(LINE4,"����λ2: %s ","��");
        }
        LcdPrint(LINE6,"��ֻ��������豸");
        LcdPrint(LINE7,"���� RIGHT ȷ��");
        LcdPrint(LINE8,"���� LEFT ����һҳ");
    }else if (This.su.ES.es_state == ES_Programing){
        LcdPrint(LINE1,"д��������ַ");
        LcdPrint(LINE2,"Ŀ���ַ: %x",This.config.dev_count+1);
        if (This.su.ES.es_programing_step < 1){ return; }
        LcdPrint(LINE3,"д��������ַ... ���");
        if (This.su.ES.es_programing_step < 2){ return; }
        LcdPrint(LINE4,"��������");
        s_data.Print(lcd_buffer,This.config.dev_count);//��������£�devcount�Ѿ����ӹ���
        LCD_ShowStringLine(LINE5,lcd_buffer);
        LcdPrint(LINE6,"�� RIGHT ȷ��");
        LcdPrint(LINE7,"�� UP ����");
        LcdPrint(LINE8,"�� LEFT ����һҳ");
    }else if (This.su.ES.es_state == ES_Yet_Another){
        LcdPrint(LINE1,"�豸 %d �����",This.config.dev_count);

        LcdPrint(LINE8,"�� UP �������");
        LcdPrint(LINE9,"�� RIGHT �˳�");
    }else if (This.su.ES.es_state == ES_Full){
        LCD_push(MAGENTA);//this supposed to be some kind of red
        LcdPrint(LINE1,"��ע��:");
        LCD_pop();
        LcdPrint(LINE1,"######## �豸�б�����!");
        LcdPrint(LINE2,"���֧��%d���豸",lenof(This.devs));
        LcdPrint(LINE3,"�����������");
        LcdPrint(LINE4,"�������Ƴ�һЩ�豸");
        LcdPrint(LINE9,"�� ����� ����");
    }

}


/**
// @brief: ��ʾ������ע��Ĵ�������Ϣ
// ��ʽΪ��
//     <����������><':'>
//    <'Data1:'><D1><D1unit> [<'Data2:'><D2><D2unit>]
// ��һ����ʾ����ʱ�����ܰ�����ҳ(todo:�Զ���ҳ����
 */
static void lcd_server_earth(){
    int i = 0;
    App_dev_desc   *ndev = &This.devs[0];
    Dev_desc const *ddev = &devDesc[0];
    while(i < MAX_DEV_COUNT && i < 6/2){//����к���ͺʹε��зֱ��Ǳ���͵���
        if(i+This.su.EA.current_top >= This.config.dev_count){ break; }//ȷ��������ֻ������Ч������
        ndev = &This.devs[i+This.su.EA.current_top];
        ddev = &devDesc[ndev->sens_desc.name_index];

        LcdPrint(i,"%s", ddev->name);
        if(ndev->sens_desc.data2.exist == 0){
            LcdPrint(i+1,"%s:%ld%s",ddev->data1_display_name,ndev->data1,ddev->data1_unit);
        }else{ //todo:����ܿ���һ����ʾ�����ˡ���Ҫ�������
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
 * @brief: �����ڲ�ͬģʽ�л�ʱ���޸ĵ���������
 * ֮��������������������������Ϊ���������Ҫ��������ʾ�Ķ����������
 * lcd_server�ᵼ����Ļ��˸
 * @param: next_state: ����ת�Ƶ�״̬
 ***/
void lcd_state_go(States next_state){
    LCD_clearLineAll();
    LCD_push(BLUE);
    switch (next_state) {
        case ST_Empyrean:
            LCD_clearLine(LINE10);
            LCD_ShowStringLineEx(LINE10,"�豸ע�����");
            break;
        case ST_Earth:
            LCD_clearLine(LINE10);
            LCD_ShowStringLineEx(LINE10,"���ݶ�ȡҳ��");
            break;
        case ST_Genesis:
            LCD_clearLine(LINE10);
            LCD_ShowStringLineEx(LINE10,"            ����ʱ��:"__TIME__);
        default:
            break;
    }
    LCD_pop();
}