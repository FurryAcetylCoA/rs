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
    //���Խ������ӣ����ȳ���ʹ��dev��ָ�������ӷ�ʽ
    Sens_buffer sens_buffer;
    memset(&sens_buffer,0,sizeof(sens_buffer));

    sens_buffer.address = dev->address;
    sens_buffer.opCode  = 0x06;
    if(dev->inst_sized == 1){
        sens_buffer.withLen.dataLen = 0x04; //���ȹ̶�
        sens_buffer.withLen.dataLo = dev->address;
        fill_crc16(&sens_buffer, 1);
    }else{
        sens_buffer.withNoLen.dataLo = dev->address;
        fill_crc16(&sens_buffer, 0);
    }
    //�������� ����ʽ�ȴ�
    uint8_t successed =1;
    uint32_t txsize = (dev->inst_sized == 0? 8 :9);
    rs485_send((const uint8_t *) &sens_buffer, txsize, 9);
    if(successed){ //�豸�л�Ӧ��˵����ַ��ָ���ʽ��ȷ
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

    //�����ֲᣬֻ������/��ѯ��ַ��ʱ�򣬲Ż��п�����size��������ȡͳһû��
    sens_buffer.withNoLen.dataLo = dev->data2.exist ? 0x2 : 0x1;
    fill_crc16(&sens_buffer,0);

    //�����������ʽ�ȴ�
    if(!1){
        return sens_failed_other;
    }
    uint8_t receivebuffer[12];
    uint8_t *datalen = &receivebuffer[2];
    if(dev->data2.exist && *datalen != 0x04){ //�����еڶ������� ����û�յ�
        _TRAP;
    }
    uint32_t crc = crc16(receivebuffer,*datalen + 2 + 1);
    if(receivebuffer [*datalen + 2 + 1] == *((uint8_t*)&crc + 0) &&\
    receivebuffer [*datalen + 2 + 1 +1] == *((uint8_t*)&crc + 1)){
        //�������������Ǵ����stm32��С����
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
    uint8_t len = (sized == 0? 8 :9) - 2;//�ܳ��ȼ�2������������CRC
    uint32_t crc = crc16((uint8_t*)buf,len);

    if(sized == 1){
        buf->withLen.crc16Hi = *((uint8_t*)&crc + 1);//stm32��С���� ����λ��ǰ
        buf->withLen.crc16Lo = *((uint8_t*)&crc + 0);
    }else{
        buf->withNoLen.crc16Hi = *((uint8_t*)&crc + 1);//stm32��С���� ����λ��ǰ
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
    //�����ȼ��ƫ��һλ�����
    ptr = buf + 1;
    if(size <8){
        _TRAP;
    }
    crc_t crc;
    crc.U = crc16(ptr,8);
    //ע�⣺06ָ��ظ�ʽ���������⣬��Щ��ԭ������8�ֽڣ���Щ�Ƿ���11�ֽ�
    //�ü��һ��
}
//���ջ����յ�һλ�����ͷ ��һλ����һ����ʲô ����������00����Ҳ�в��ǵ�
//����С��������»��յ�û�ж���һλ��
//������ս��������й�
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