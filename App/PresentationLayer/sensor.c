//
// Created by 11096 on 2022/10/29.
//

#include "sensor.h"
#include "string.h"
#include "crc16.h"
#include "rs485.h"
#include "stdbool.h"
static void fill_crc16(Sens_buffer *buf, uint8_t sized);
static bool check_06(const uint8_t *buf, uint32_t size);
static bool check_03(const uint8_t *buf, uint32_t size){

}
/**
* @brief get device address using broadcast
* This function will try to communicate with the device using address specified in dev.
* If such communicate can't be establish. It will return with error.
* @param dev: device handle pointer
* @retval sens_success on success; sens_failed_timeout on failure.
*/
sens_ErrCode sens_TryAddr(Sens_dev_desc *dev){
    //尝试建立连接，首先尝试使用dev中指定的连接方式
    Sens_buffer sens_buffer;
    memset(&sens_buffer,0,sizeof(sens_buffer));

    sens_buffer.address = dev->address;
    sens_buffer.opCode  = 0x06;
    if(dev->inst_sized == 1){
        sens_buffer.withLen.dataLen = 0x04; //长度固定
        sens_buffer.withLen.dataLo = dev->address;
        fill_crc16(&sens_buffer, 1);
    }else{
        sens_buffer.withNoLen.dataLo = dev->address;
        fill_crc16(&sens_buffer, 0);
    }
    //发送命令 阻塞式等待
    uint8_t successed =1;
    uint32_t txsize = (dev->inst_sized == 0? 8 :9);
    rs485_send((const uint8_t *) &sens_buffer, txsize, 9);
    if(successed){ //设备有回应，说明地址和指令格式正确
        return sens_success;
    }else{//
        return sens_failed_timeout;
    }


}

sens_ErrCode sens_GetVal(Sens_dev_desc *dev){

    Sens_buffer sens_buffer;
    memset(&sens_buffer,0,sizeof(sens_buffer));

    sens_buffer.address = dev->address;
    sens_buffer.opCode  = 0x03;

    //根据手册，只有设置/查询地址的时候，才会有可能有size。正常读取统一没有
    sens_buffer.withNoLen.dataLo = dev->data2.exist ? 0x2 : 0x1;
    fill_crc16(&sens_buffer,0);

    //发送命令。阻塞式等待
    if(!1){
        return sens_failed_other;
    }
    uint8_t receivebuffer[12];
    uint8_t *datalen = &receivebuffer[2];
    if(dev->data2.exist && *datalen != 0x04){ //明明有第二传感器 但是没收到
        _TRAP;
    }
    uint32_t crc = crc16(receivebuffer,*datalen + 2 + 1);
    if(receivebuffer [*datalen + 2 + 1] == *((uint8_t*)&crc + 0) &&\
    receivebuffer [*datalen + 2 + 1 +1] == *((uint8_t*)&crc + 1)){
        //发过来的数据是大端序。stm32是小端序
        dev->data1_raw = ((uint32_t)receivebuffer[2+1+1]<<16) & \
                         (uint32_t)receivebuffer[2+1];
        if(dev->data2.exist){
            dev->data1_raw = ((uint32_t)receivebuffer[2+1+1+2]<<16) & \
                              (uint32_t)receivebuffer[2+1+2];
        }
    }else{
        return sens_failed_crc;
}
    return sens_success;

}

/**
* @brief fill crc checksum in command
*
* @param buf: command buffer pointer
* @param sized: 1 for withLen and 0 for withNoLen
*/
static void fill_crc16(Sens_buffer *buf, uint8_t sized){
    uint32_t i, j;
    uint8_t len = (sized == 0? 8 :9) - 2;//总长度减2，即不算那俩CRC
    uint32_t crc = crc16((uint8_t*)buf,len);

    if(sized == 1){
        buf->withLen.crc16Hi = *((uint8_t*)&crc + 1);//stm32是小端序 即低位在前
        buf->withLen.crc16Lo = *((uint8_t*)&crc + 0);
    }else{
        buf->withNoLen.crc16Hi = *((uint8_t*)&crc + 1);//stm32是小端序 即低位在前
        buf->withNoLen.crc16Lo = *((uint8_t*)&crc + 0);
    }
}
/**
* @brief check rx data of command 06(address set) from sensors
* Given that chances of 485 receiving an extra byte at the beginning are high
* This function will try to parse it in many ways.
* And unify it if possible (not impl yet)
* @param buf: rx buffer
* @param size: size of rx buffer
* @retval true on valid data; false on invalid data
*/
static bool check_06(const uint8_t *buf, uint32_t size){
    const uint8_t *ptr;
    //首先先检查偏移一位的情况
    ptr = buf + 1;
    if(size <8){
        _TRAP;
    }
    crc_t crc;
    crc.U = crc16(ptr,8);
    //注意：06指令返回格式好像还有问题，有些是原样返回8字节，有些是返回11字节
    //得检查一下
}
//接收会多接收到一位，在最开头 这一位还不一定是什么 大多数情况是00，但也有不是的
//而在小概率情况下会收到没有多余一位的
//这个和终结电阻可能有关
/**
* @brief check rx data of command 03(sensor read) from sensors
* Given that chances of 485 receiving an extra byte at the beginning are high
* This function will try to parse it in many ways.
* And unify it if possible (not impl yet)
* @param buf: rx buffer
* @param size: size of rx buffer
* @retval true on valid data; false on invalid data
*/
static bool check_03(const uint8_t *buf, uint32_t size){

}