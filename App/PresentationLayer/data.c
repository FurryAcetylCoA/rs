//
// Created by 11096 on 2022/10/31.
//

#include "data.h"
#include "sensor.h"
#include "stdio.h"


static void sdata_Poll(uint32_t current_poll);
static void sdata_PollOne(uint8_t num);
static void sdata_Poll_Pub();
static void sdata_PollAll();

static uint32_t current_poll_pub;

S_Data s_data={
        .Poll = sdata_Poll_Pub,
        .Pollall = sdata_PollAll,
        .PollOne = sdata_PollOne
};

/**
* @brief ����ָ����id�ţ���ȡ��Ӧ�Ĵ�����
* @param num:ID�ţ���0��ʼ
* @retval None.
*/
static void sdata_PollOne(uint8_t num){
    sdata_Poll(num);
}


/**
* @brief ���ݵ�ǰ��ѯid��
*
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
* @param current_poll: ��Ҫ��ѯ��id��
* @retval None.
*/
static void sdata_Poll(uint32_t current_poll){
    sens_ErrCode ret= sens_GetVal(&This.devs[current_poll].sens_desc);
    if (ret == sens_failed_crc){ //ż���ᷢ���������飬��һ��������һ��
        HAL_Delay(800);
        ret= sens_GetVal(&This.devs[current_poll].sens_desc);
        if(ret == sens_failed_crc){
            This.devs[current_poll].errCode = sens_failed_crc;
            return;
        }
    }
    float data1_temp = 0,data2_temp = 0;
    This.devs[current_poll].errCode = sens_success;
    /////����data1_raw/////
    ///���ſ���///
    if(This.devs[current_poll].sens_desc.data1.is_signed == 1){
        //�����з�������Ҫ����
        if((uint16_t)This.devs[current_poll].sens_desc.data1_raw >= 0x8000 ) {
            data1_temp = (float)0xFFFF - (float) This.devs[current_poll].sens_desc.data1_raw + 1;
            data1_temp = -1 * data1_temp;
        }else{
            data1_temp = (float)This.devs[current_poll].sens_desc.data1_raw;
        }
    }else{
            data1_temp = (float)This.devs[current_poll].sens_desc.data1_raw;
    }//���ſ��ƽ���
    ///�˳�ϵ��///
    if(This.devs[current_poll].sens_desc.data1.mult_or_div == 0){
        data1_temp *= (float)This.devs[current_poll].sens_desc.data1.factor;
    }else{
        data1_temp /= (float)This.devs[current_poll].sens_desc.data1.factor;
    }//�˳�ϵ������
    This.devs[current_poll].data1 = data1_temp;
    /////����data2_raw/////
    ///�������ж�///
    if(This.devs[current_poll].sens_desc.data2.exist){
        ///���ſ���///
        if(This.devs[current_poll].sens_desc.data2.is_signed == 1){
            if((float)This.devs[current_poll].sens_desc.data2_raw >= 0x8000 ) {
                data2_temp = (float)0xFFFF - (float) This.devs[current_poll].sens_desc.data2_raw + 1;
                data2_temp = -1 * data2_temp;
            }else{
                data2_temp = (float)This.devs[current_poll].sens_desc.data2_raw;
            }
        }else{
            data2_temp = (float)This.devs[current_poll].sens_desc.data2_raw;
        }//���ſ��ƽ���
        ///ϵ��///
        if(This.devs[current_poll].sens_desc.data2.factor != 1){
            data2_temp /= (float)This.devs[current_poll].sens_desc.data2.factor;
        }
        This.devs[current_poll].data2 = data2_temp;
    }

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