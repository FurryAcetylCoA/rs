#include <stdio.h>
#include "eeprom_drv.h"
#include "string.h"
#define AT24CXX_Write_ADDR 0xA0
#define AT24CXX_Read_ADDR  0xA1
#define AT24CXX_MAX_SIZE   256 //256�ǿ��õġ�257�Ὺʼ�ؾ�
#define AT24CXX_PAGE_SIZE  8
#define AT24CXX_PAGE_TOTAL (AT24CXX_MAX_SIZE/AT24CXX_PAGE_SIZE)


static void I2C_Mem_Write(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout);
static void I2C_Mem_Read (I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout);
static uint32_t i2c_cold_down; //2022��10��27�գ�Twr��5ms�������κ�����¶������㣬����STM��Ӳ��I2C��ֱ�ӱ���


void EE_write (uint8_t addr,uint8_t *data,uint32_t len){
	if (len == 0) { return; }
	int selectPage_idx  = addr % AT24CXX_PAGE_SIZE;
	int selectPage_rest = AT24CXX_PAGE_SIZE - selectPage_idx;
	if(len <= selectPage_rest){ //һҳ��д��
		I2C_Mem_Write(&hi2c1,AT24CXX_Write_ADDR,addr,I2C_MEMADD_SIZE_8BIT,data,len,0xFF);
	}else{ //һҳд����
		I2C_Mem_Write(&hi2c1,AT24CXX_Write_ADDR,addr,I2C_MEMADD_SIZE_8BIT,data,selectPage_rest,0xFF);

        //����д����д����
		addr += selectPage_rest;
		len  -= selectPage_rest;
		data += selectPage_rest;
		//��ͷд���ˡ�������������ҳд
		int full_page = len / AT24CXX_PAGE_SIZE ; //����������µ����ݲ���һҳ��
		for (int current_page =0;current_page < full_page; current_page++){
                I2C_Mem_Write(&hi2c1,AT24CXX_Write_ADDR,addr,I2C_MEMADD_SIZE_8BIT,data,AT24CXX_PAGE_SIZE,0xFF);

				addr += AT24CXX_PAGE_SIZE;
				len  -= AT24CXX_PAGE_SIZE;
				data += AT24CXX_PAGE_SIZE;
			}
        if( 0 != len){ //�������β��ûд��
            I2C_Mem_Write(&hi2c1,AT24CXX_Write_ADDR,addr,I2C_MEMADD_SIZE_8BIT,data,len,0xFF);
            }
		}
}


void EE_read(uint8_t addr,uint8_t *data,uint32_t len){
     I2C_Mem_Read(&hi2c1,AT24CXX_Read_ADDR,addr,I2C_MEMADD_SIZE_8BIT,data,len,0xFF);;
}


static void I2C_Mem_Write(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout){
    uint32_t  delta = HAL_GetTick() - i2c_cold_down;
    if(delta < 5){ //��ҳд֮�����������ʱ�����5ms(Twr)
        HAL_Delay(5 - delta + 1);
    }
    HAL_StatusTypeDef ret = HAL_I2C_Mem_Write(hi2c,DevAddress,MemAddress,MemAddSize,pData,Size,Timeout);
    i2c_cold_down = HAL_GetTick();
    if(ret != HAL_OK) { //�������ᵽjava���֡�������뱻catch���ϱ���������Ƕ�ô���е���
        _TRAP;
    }
}


static void  I2C_Mem_Read(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout){
    uint32_t  delta = HAL_GetTick() - i2c_cold_down;
    if(delta < 3) { //�ֲ�û˵д�����д֮��Ҫ������٣�������3ms����
        HAL_Delay(3 - delta + 1);
    }
    HAL_StatusTypeDef ret = HAL_I2C_Mem_Read(hi2c,DevAddress,MemAddress,MemAddSize,pData,Size,Timeout);
    i2c_cold_down = HAL_GetTick();
    if(ret != HAL_OK){
        _TRAP;
    }
}

