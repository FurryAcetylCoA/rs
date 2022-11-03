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
    //��������ѡȡwithNoLen
    sens_buffer.withNoLen.dataLo = dev->data2.exist ? 0x2 : 0x1;
    fill_crc16(&sens_buffer,0);
    uint8_t rxbuf[12];//׼�����ջ�����
    uint8_t *pprxbuf =  &rxbuf[0];
    //����ֵ����Ӧ����3+������������*2+crc
    //�����������ʽ�ȴ�
    rs485_send((const uint8_t *) &sens_buffer, rxbuf, 8, 3 + sens_buffer.withNoLen.dataLo * 2 + 2);

    if (check_03((uint8_t **) &pprxbuf, 3 + sens_buffer.withNoLen.dataLo * 2 + 2) != true){
        //�����������ͼ��pprxbufָ��rxbuf��Ч���ֵĿ�ͷ
        _TRAP;
        //��ʱ����������ü�һ�����ӳٵ�����
        return sens_failed_crc;
    }

    uint8_t *datalen = &(pprxbuf[2]);
    if(dev->data2.exist && *datalen != 0x04){ //�����еڶ������� ����û�յ�
        _TRAP;
    }

    dev->data1_raw = ((uint32_t)pprxbuf[2 + 1 ] << 8) | \
                        (uint32_t)pprxbuf[2 + 1 +1];
    if (dev->data1.factor != 1){
        if(dev->data1.mult_or_div ==1){ //1������
            dev->data1_raw /= dev->data1.factor;
        }else {
            dev->data1_raw *= dev->data1.factor;
        }
    }
    if(dev->data2.exist){
            dev->data2_raw = ((uint32_t)pprxbuf[2 + 1 + 2] << 8) | \
                        (uint32_t)pprxbuf[2 + 1 + 1 + 2];
            if (dev->data2.factor != 1){ //�ڶ�����ֻ�г���
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

//���ջ����յ�һλ�����ͷ ��һλ����һ����ʲô ����������00����Ҳ�в��ǵ�
//����С��������»��յ�û�ж���һλ��
//������ս��������й�
/**
* @brief check rx data of command 03(sensor read) from sensors
* Given that chances of 485 receiving an extra byte at the beginning are high
* This function will try to parse it in many ways. (Using crc checksum)
* And unify it if possible
* @param buf: a pointer that point to rx buffer (Cû�д����������ֱŤ����Ҫ����
* @param size: expected size of rx buffer (include checksum and header)
* @retval true: successfully unify; false: unable to unify rx buffer
*/
static bool check_03(uint8_t **buf, uint32_t size){
    //�ȴ���ͷ����������
    crc_t  crc;
    crc.U = crc16(*buf+1,size - 2);//�ӵڶ�λ��ʼ����crc��Ȼ���Ԥ�ڵ�λ�Ƚ�
    if (crc.Lo == (*buf+1)[size - 2] && crc.Hi == (*buf+1)[size - 1]){
        //crcȷʵ���ϣ���˵��ȷʵ�ǿ�ͷ��һ�����룬���Һ��涼����
        *buf= (*buf+1); //��rxָ�������� �õ��÷�����ֱ�Ӵ���
        return true;
    }
    //�ٴ���ͷֱ�Ӿ������ݵ����
    crc.U = crc16(*buf,size - 2);
    if (crc.Lo == (*buf+1)[size - 2] && crc.Hi == (*buf+1)[size - 1]){
        //crcȷʵ���ϣ���˵��ȷʵ�ǿ�ͷ�������ݣ����Һ��涼����
        //������rxָ��
        return true;
    }
    return false;//�޷�ʶ������

}