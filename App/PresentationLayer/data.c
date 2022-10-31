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
* @brief 根据当前轮询id号
* todo:目前是阻塞，我想写成回调，但是没想好怎么写
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
* @brief 向下层要求某个传感器的数据
* todo:目前是阻塞，我想写成回调，但是没想好怎么写
* @param current_poll: 需要查询的id号
* @retval None.
*/
static void sdata_Poll(uint32_t current_poll){
    sens_GetVal(&This.devs[current_poll].sens_desc);
    int32_t data1_temp,data2_temp;

    /////处理data1_raw/////
    ///符号控制///
    if(This.devs[current_poll].sens_desc.data1.is_signed == 1){
        //对于有符号数，要解码（todo：int32是不是自己就能解了。得试试）
        _TRAP;
        if((uint16_t)This.devs[current_poll].sens_desc.data1_raw >= 0x8000 ) {
            data1_temp = 0xFFFF - (uint16_t) This.devs[current_poll].sens_desc.data1_raw + 1;
            data1_temp = -1 * data1_temp;
        }
    }else{
            data1_temp = (uint16_t)This.devs[current_poll].sens_desc.data1_raw;
    }//符号控制结束
    ///乘除系数///
    if(This.devs[current_poll].sens_desc.data1.mult_or_div == 1){//todo:我忘记哪个是mult了
        data1_temp *= This.devs[current_poll].sens_desc.data1.factor;
    }else{
        data1_temp /= This.devs[current_poll].sens_desc.data1.factor;
    }//乘除系数结束
    /////处理data2_raw/////
    ///存在性判断///
    if(This.devs[current_poll].sens_desc.data2.exist){
        ///符号控制///
        if(This.devs[current_poll].sens_desc.data2.is_signed == 1){
            if((uint16_t)This.devs[current_poll].sens_desc.data1_raw >= 0x8000 ) {
                data2_temp = 0xFFFF - (uint16_t) This.devs[current_poll].sens_desc.data2_raw + 1;
                data2_temp = -1 * data2_temp;
            }
        }else{
            data2_temp = (uint16_t)This.devs[current_poll].sens_desc.data2_raw;
        }//符号控制结束
        ///系数///
        if(This.devs[current_poll].sens_desc.data2.factor != 1){
            data2_temp /= This.devs[current_poll].sens_desc.data2.factor;
        }
        This.devs[current_poll].data2 = data2_temp;
    }
    This.devs[current_poll].data1 = data1_temp;


}

/**
* @brief 阻塞式轮询所有注册的传感器，向下层要求某个传感器的数据
* 通常是第一次读取时调用
*
* @retval None.
*/
static void sdata_PollAll(){
    for (int i = 0; i < This.config.dev_count; ++i) {
        sdata_Poll(i);
    }
}