//
// Created by 11096 on 2022/10/29.
//

#include "sensor.h"
#include "string.h"
#include "crc16.h"
#include "rs485.h"
#include "stdbool.h"
static void fill_crc16(Sens_buffer *buf, uint8_t sized);
static bool check_03(uint8_t **buf, uint32_t size);


//使用广播方式设置设备地址
//这种情况下，dev中只有inst_sized是有意义的
sens_ErrCode sens_SetAddr(Sens_dev_desc *dev,uint8_t addr){
    Sens_buffer sens_buffer;
    memset(&sens_buffer,0,sizeof(sens_buffer));

    sens_buffer.address = 0xFE; //broadcast
    sens_buffer.opCode  = 0x06;

    if (dev->inst_sized){
        sens_buffer.withLen.dataLo = addr;
        fill_crc16(&sens_buffer,1);
    }else{
        sens_buffer.withNoLen.dataLo = addr;
        fill_crc16(&sens_buffer,0);
    }
    uint8_t rxbuf[12]={0};//准备接收缓冲区
    uint32_t txsize = (dev->inst_sized == 0? 8 :9);
    rs485_send((const uint8_t *) &sens_buffer, rxbuf, txsize, 11);
    HAL_Delay(2);
    rs485_send((const uint8_t *) &sens_buffer, rxbuf, txsize, 11);
    //简单粗暴发两遍，确保成功

    //06指令的返回格式差异非常大，如果考虑到第一位可能的乱码，那么就会更复杂，
    //所以这里使用一个不标准的方式大概验证一下返回数据的有效性
    //根据观察，几种返回格式共同的特点是：
    //  1）都有0x06    2）都有连续3个0x00接一个新地址
    /*int step=0;
    for (uint8_t* p=rxbuf;p < (rxbuf+sizeof(rxbuf));p++) {
        switch (step) {
            case 0:
                if (*p == 0x06){step++;} break;
            case 1:
                if (*p == 0x00){step++;} break;
            case 2:
            case 3:
                if (*p == 0x00){
                    step++;
                }else{
                    step =2;
                }
                break;
            case 4:
                if (*p == addr){
                    step++;
                }else{
                    step = -1;
                }
                break;
            default:
                break;
        }
    }//for
    if(step != 5){ //没有完成所有匹配，说明命令失败
        return sens_failed_other;
    }*/
    //以上纯属扯淡，这玩意的返回值根本跟手册对不上，但确确实实是改成功了
    dev->address=addr;

    return sens_success;
}

sens_ErrCode sens_GetVal(Sens_dev_desc *dev){

    Sens_buffer sens_buffer;
    memset(&sens_buffer,0,sizeof(sens_buffer));

    sens_buffer.address = dev->address;
    sens_buffer.opCode  = 0x03;

    //根据手册，只有设置/查询地址的时候，才会有可能有size。正常读取统一没有
    //所以这里选取withNoLen
    sens_buffer.withNoLen.dataLo = dev->data2.exist ? 0x2 : 0x1;
    fill_crc16(&sens_buffer,0);
    uint8_t rxbuf[12];//准备接收缓冲区
    uint8_t *pprxbuf =  &rxbuf[0];
    //返回值长度应该是3+请求数据数量*2+crc
    //发送命令。阻塞式等待
    rs485_send((const uint8_t *) &sens_buffer, rxbuf, 8, 3 + sens_buffer.withNoLen.dataLo * 2 + 2);

    if (check_03((uint8_t **) &pprxbuf, 3 + sens_buffer.withNoLen.dataLo * 2 + 2) != true){
        //这个函数会试图让pprxbuf指向rxbuf有效部分的开头
        _TRAP;
        //有时候会进到这里，得加一个带延迟的重试
        return sens_failed_crc;
    }

    uint8_t *datalen = &(pprxbuf[2]);
    if(dev->data2.exist && *datalen != 0x04){ //明明有第二传感器 但是没收到
        _TRAP;
    }

    dev->data1_raw = ((uint32_t)pprxbuf[2 + 1 ] << 8) | \
                        (uint32_t)pprxbuf[2 + 1 +1];
    if (dev->data1.factor != 1){
        if(dev->data1.mult_or_div ==1){ //1：除法
            dev->data1_raw /= dev->data1.factor;
        }else {
            dev->data1_raw *= dev->data1.factor;
        }
    }
    if(dev->data2.exist){
            dev->data2_raw = ((uint32_t)pprxbuf[2 + 1 + 2] << 8) | \
                        (uint32_t)pprxbuf[2 + 1 + 1 + 2];
            if (dev->data2.factor != 1){ //第二数据只有除法
                dev->data2_raw /= dev->data2.factor;
            }
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


//接收会多接收到一位，在最开头 这一位还不一定是什么 大多数情况是00，但也有不是的
//而在小概率情况下会收到没有多余一位的
//这个和终结电阻可能有关
/**
* @brief check rx data of command 03(sensor read) from sensors
* Given that chances of 485 receiving an extra byte at the beginning are high
* This function will try to parse it in many ways. (Using crc checksum)
* And unify it if possible
* @param buf: a pointer that point to rx buffer (C没有传引用这里简直扭曲得要死）
* @param size: expected size of rx buffer (include checksum and header)
* @retval true: successfully unify; false: unable to unify rx buffer
*/
static bool check_03(uint8_t **buf, uint32_t size){
    //先处理开头有乱码的情况
    //编码说明：buf用之前一定要先解一次指针
    crc_t  crc;
    crc.U = crc16(*buf+1,size - 2);//从第二位开始计算crc。然后和预期的位比较
    if (crc.Lo == (*buf+1)[size - 2] && crc.Hi == (*buf+1)[size - 1]){
        //crc确实符合，这说明确实是开头有一个乱码，并且后面都正常
        *buf= (*buf+1); //把rx指针往后推 让调用方可以直接处理
        return true;
    }
    //再处理开头直接就是数据的情况
    crc.U = crc16(*buf,size - 2);
    if (crc.Lo == (*buf+1)[size - 2] && crc.Hi == (*buf+1)[size - 1]){
        //crc确实符合，这说明确实是开头就是数据，并且后面都正常
        //不调整rx指针
        return true;
    }
    return false;//无法识别的情况

}