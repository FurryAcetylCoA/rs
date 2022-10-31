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
            uint8_t dataLen;  //�����Ͳ����
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
        sens_buffer.withLen.dataLo = dev->address; //��֪���ܲ�������ԭ��̤��
        fill_crc16(&sens_buffer, 1);
    }else{
        sens_buffer.withNoLen.dataLo = dev->address; //��֪���ܲ�������ԭ��̤��
        fill_crc16(&sens_buffer, 0);
    }
    //�������� ����ʽ�ȴ�
    uint8_t successed =1;
    if(successed){ //�豸�л�Ӧ��˵����ַ��ָ���ʽ��ȷ
        return sens_success;
    }else{//
        return sens_failed_timeout;
    }


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
    uint32_t c, crc=0xFFFF;
    for(i=0; i<len; i++)
    {
        c = *((uint8_t*)buf +i ) & 0x00FF;
        crc ^= c;
        for(j=0; j<8; j++)
        {
            if (crc & 0x0001)
            {
                crc>>=1;
                crc^=0xA001;
            }else crc>>=1;
        }
    }
    if(sized == 1){
        buf->withLen.crc16Hi = *((uint8_t*)&crc + 1);//stm32��С���� ����λ��ǰ
        buf->withLen.crc16Lo = *((uint8_t*)&crc + 0);
    }else{
        buf->withNoLen.crc16Hi = *((uint8_t*)&crc + 1);//stm32��С���� ����λ��ǰ
        buf->withNoLen.crc16Lo = *((uint8_t*)&crc + 0);
    }
}

