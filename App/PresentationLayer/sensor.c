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


//ʹ�ù㲥��ʽ�����豸��ַ
//��������£�dev��ֻ��inst_sized���������
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
    uint8_t rxbuf[12]={0};//׼�����ջ�����
    uint32_t txsize = (dev->inst_sized == 0? 8 :9);
    rs485_send((const uint8_t *) &sens_buffer, rxbuf, txsize, 11);
    HAL_Delay(2);
    rs485_send((const uint8_t *) &sens_buffer, rxbuf, txsize, 11);
    //�򵥴ֱ������飬ȷ���ɹ�

    //06ָ��ķ��ظ�ʽ����ǳ���������ǵ���һλ���ܵ����룬��ô�ͻ�����ӣ�
    //��������ʹ��һ������׼�ķ�ʽ�����֤һ�·������ݵ���Ч��
    //���ݹ۲죬���ַ��ظ�ʽ��ͬ���ص��ǣ�
    //  1������0x06    2����������3��0x00��һ���µ�ַ
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
    if(step != 5){ //û���������ƥ�䣬˵������ʧ��
        return sens_failed_other;
    }*/
    //���ϴ���������������ķ���ֵ�������ֲ�Բ��ϣ���ȷȷʵʵ�Ǹĳɹ���
    dev->address=addr;

    return sens_success;
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
    //����˵����buf��֮ǰһ��Ҫ�Ƚ�һ��ָ��
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