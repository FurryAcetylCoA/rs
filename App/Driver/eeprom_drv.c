#include <stdio.h>
#include "eeprom_drv.h"
#include "string.h"
#define AT24CXX_Write_ADDR 0xA0
#define AT24CXX_Read_ADDR  0xA1
#define AT24CXX_MAX_SIZE   256 //256是可用的。257会开始回卷
#define AT24CXX_PAGE_SIZE  8
#define AT24CXX_PAGE_TOTAL (AT24CXX_MAX_SIZE/AT24CXX_PAGE_SIZE)


static void I2C_Mem_Write(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout);
static void I2C_Mem_Read (I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout);
static uint32_t i2c_cold_down; //2022年10月27日：Twr的5ms必须在任何情况下都被满足，否则STM的硬件I2C会直接报错


void EE_write (uint8_t addr,uint8_t *data,uint32_t len){
	if (len == 0) { return; }
	int selectPage_idx  = addr % AT24CXX_PAGE_SIZE;
	int selectPage_rest = AT24CXX_PAGE_SIZE - selectPage_idx;
	if(len <= selectPage_rest){ //一页能写完
		I2C_Mem_Write(&hi2c1,AT24CXX_Write_ADDR,addr,I2C_MEMADD_SIZE_8BIT,data,len,0xFF);
	}else{ //一页写不下
		I2C_Mem_Write(&hi2c1,AT24CXX_Write_ADDR,addr,I2C_MEMADD_SIZE_8BIT,data,selectPage_rest,0xFF);

        //先能写多少写多少
		addr += selectPage_rest;
		len  -= selectPage_rest;
		data += selectPage_rest;
		//零头写完了。接下来按整个页写
		int full_page = len / AT24CXX_PAGE_SIZE ; //如果这里余下的数据不够一页，
		for (int current_page =0;current_page < full_page; current_page++){
                I2C_Mem_Write(&hi2c1,AT24CXX_Write_ADDR,addr,I2C_MEMADD_SIZE_8BIT,data,AT24CXX_PAGE_SIZE,0xFF);

				addr += AT24CXX_PAGE_SIZE;
				len  -= AT24CXX_PAGE_SIZE;
				data += AT24CXX_PAGE_SIZE;
			}
        if( 0 != len){ //如果还有尾巴没写完
            I2C_Mem_Write(&hi2c1,AT24CXX_Write_ADDR,addr,I2C_MEMADD_SIZE_8BIT,data,len,0xFF);
            }
		}
}


void EE_read(uint8_t addr,uint8_t *data,uint32_t len){
     I2C_Mem_Read(&hi2c1,AT24CXX_Read_ADDR,addr,I2C_MEMADD_SIZE_8BIT,data,len,0xFF);;
}


static void I2C_Mem_Write(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout){
    uint32_t  delta = HAL_GetTick() - i2c_cold_down;
    if(delta < 5){ //跨页写之间必须满足间隔时间大于5ms(Twr)
        HAL_Delay(5 - delta + 1);
    }
    HAL_StatusTypeDef ret = HAL_I2C_Mem_Write(hi2c,DevAddress,MemAddress,MemAddSize,pData,Size,Timeout);
    i2c_cold_down = HAL_GetTick();
    if(ret != HAL_OK) { //我这才体会到java那种“错误必须被catch或上报”的设计是多么地有道理
        _TRAP;
    }
}


static void  I2C_Mem_Read(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout){
    uint32_t  delta = HAL_GetTick() - i2c_cold_down;
    if(delta < 3) { //手册没说写读与读写之间要间隔多少，这里间隔3ms好了
        HAL_Delay(3 - delta + 1);
    }
    HAL_StatusTypeDef ret = HAL_I2C_Mem_Read(hi2c,DevAddress,MemAddress,MemAddSize,pData,Size,Timeout);
    i2c_cold_down = HAL_GetTick();
    if(ret != HAL_OK){
        _TRAP;
    }
}

