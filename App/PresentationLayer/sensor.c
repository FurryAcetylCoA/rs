//
// Created by 11096 on 2022/10/29.
//

#include "sensor.h"
#include "string.h"

typedef __PACKED_STRUCT {
    uint8_t address;
    uint8_t opCode;
    __PACKED_UNION{
        __PACKED_STRUCT{
            uint8_t dataAddHi;
            uint8_t dataAddLo;
            uint8_t dataHi;
            uint8_t dataLo;
            uint8_t crc16Lo;
            uint8_t crc16Hi;
        }withNoLen;
        __PACKED_STRUCT{
            uint8_t dataLen;  //这俩就差这块
            uint8_t dataAddHi;
            uint8_t dataAddLo;
            uint8_t dataHi;
            uint8_t dataLo;
            uint8_t crc16Lo;
            uint8_t crc16Hi;
        }withLen;
    };
}Sens_buffer;

static void fill_crc16(Sens_buffer *buf, uint8_t sized);
static uint32_t crc16(const uint8_t *buf,uint32_t len);

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
        sens_buffer.withLen.dataLo = dev->address; //不知道能不能这样原地踏步
        fill_crc16(&sens_buffer, 1);
    }else{
        sens_buffer.withNoLen.dataLo = dev->address; //不知道能不能这样原地踏步
        fill_crc16(&sens_buffer, 0);
    }
    //发送命令 阻塞式等待
    uint8_t successed =1;
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
* @brief calculate crc checksum in command
* stm32自带的crc外设只能算crc32
* @param buf: command buffer pointer
* @param sized: 1 for withLen and 0 for withNoLen
*/
static uint32_t crc16(const uint8_t *buf,uint32_t len){
    uint32_t c, crc=0xFFFF;
    for(int i=0; i<len; i++)
    {
        c = *((uint8_t*)buf +i ) & 0x00FF;
        crc ^= c;
        for(int j=0; j<8; j++)
        {
            if (crc & 0x0001)
            {
                crc >>= 1;
                crc ^= 0xA001;
            }else {
                crc >>= 1;
            }
        }
    }
    return crc;
}
