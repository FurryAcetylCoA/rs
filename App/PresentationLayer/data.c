//
// Created by 11096 on 2022/10/31.
//

#include "data.h"
#include "sensor.h"


static void sdata_Poll(uint32_t current_poll);
static void sdata_Poll_Pub();
static void sdata_PollAll();

static uint32_t current_poll_pub;

S_Data s_data={
        .Poll = sdata_Poll_Pub,
        .Pollall = sdata_PollAll
};

/**
* @brief ���ݵ�ǰ��ѯid��
* todo:Ŀǰ������������д�ɻص�������û�����ôд
*
* @retval None.
*/
static void sdata_Poll_Pub(){
    sdata_Poll(current_poll_pub);

    current_poll_pub++;
    if(current_poll_pub >= This.config.dev_count){
        current_poll_pub = 0;
    }
}

/**
* @brief ���²�Ҫ��ĳ��������������
* todo:Ŀǰ������������д�ɻص�������û�����ôд
* @param current_poll: ��Ҫ��ѯ��id��
* @retval None.
*/
static void sdata_Poll(uint32_t current_poll){
    sens_GetVal(&This.devs[current_poll].sens_desc);
    int32_t data1_temp,data2_temp;

    /////����data1_raw/////
    ///���ſ���///
    if(This.devs[current_poll].sens_desc.data1.is_signed == 1){
        //�����з�������Ҫ���루todo��int32�ǲ����Լ����ܽ��ˡ������ԣ�
        _TRAP;
        if((uint16_t)This.devs[current_poll].sens_desc.data1_raw >= 0x8000 ) {
            data1_temp = 0xFFFF - (uint16_t) This.devs[current_poll].sens_desc.data1_raw + 1;
            data1_temp = -1 * data1_temp;
        }
    }else{
            data1_temp = (uint16_t)This.devs[current_poll].sens_desc.data1_raw;
    }//���ſ��ƽ���
    ///�˳�ϵ��///
    if(This.devs[current_poll].sens_desc.data1.mult_or_div == 1){//todo:�������ĸ���mult��
        data1_temp *= This.devs[current_poll].sens_desc.data1.factor;
    }else{
        data1_temp /= This.devs[current_poll].sens_desc.data1.factor;
    }//�˳�ϵ������
    /////����data2_raw/////
    ///�������ж�///
    if(This.devs[current_poll].sens_desc.data2.exist){
        ///���ſ���///
        if(This.devs[current_poll].sens_desc.data2.is_signed == 1){
            if((uint16_t)This.devs[current_poll].sens_desc.data1_raw >= 0x8000 ) {
                data2_temp = 0xFFFF - (uint16_t) This.devs[current_poll].sens_desc.data2_raw + 1;
                data2_temp = -1 * data2_temp;
            }
        }else{
            data2_temp = (uint16_t)This.devs[current_poll].sens_desc.data2_raw;
        }//���ſ��ƽ���
        ///ϵ��///
        if(This.devs[current_poll].sens_desc.data2.factor != 1){
            data2_temp /= This.devs[current_poll].sens_desc.data2.factor;
        }
        This.devs[current_poll].data2 = data2_temp;
    }
    This.devs[current_poll].data1 = data1_temp;


}

/**
* @brief ����ʽ��ѯ����ע��Ĵ����������²�Ҫ��ĳ��������������
* ͨ���ǵ�һ�ζ�ȡʱ����
*
* @retval None.
*/
static void sdata_PollAll(){
    for (int i = 0; i < This.config.dev_count; ++i) {
        sdata_Poll(i);
    }
}