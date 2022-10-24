#include "eeprom.h"
#include "string.h"

#define AT24CXX_Write_ADDR 0xA0
#define AT24CXX_Read_ADDR  0xA1
#define AT24CXX_MAX_SIZE   256
#define AT24CXX_PAGE_SIZE  8
#define AT24CXX_PAGE_TOTAL (AT24CXX_MAX_SIZE/AT24CXX_PAGE_SIZE)

static int  EE_write(uint8_t addr,uint8_t *data,uint32_t len);
static int  EE_read (uint8_t addr,uint8_t *data,uint32_t len);

void EE_init(){
	uint8_t buffer;
	
	EE_read(250,&buffer,1);
	if(buffer == 0x32){
		E2PRom_data white_data;
		memset(&white_data,0,sizeof(E2PRom_data));
		EE_write(0,(uint8_t*)&white_data,sizeof(E2PRom_data));
		buffer = 0x32;
		EE_write(250,&buffer,1);
	}
	
}


int EE_check(){  
	int ret =1;
	
	uint8_t string[]={"AcetylCoA"__TIME__};
	uint8_t string_echo[sizeof(string)];           //测试EEPROM是否可以正常读写，防止待会读出脏数据
	EE_write(251,string,sizeof(string));
	EE_read (251,string_echo,sizeof(string));
	ret = ret && (0== strcmp((char*)string,(char*)string_echo));
	
	ret = ret && (sizeof(uint32_t) == sizeof(E2PRom_dev));   //测试断言
	return ~ret;
}
static int EE_write (uint8_t addr,uint8_t *data,uint32_t len){
		
	if (0 == len) { return -1; }
	int res = HAL_OK;
	int selectPage_idx  = addr % AT24CXX_PAGE_SIZE;
	int selectPage_rest = AT24CXX_PAGE_SIZE - selectPage_idx;
	
	if(len <= selectPage_rest){
		res = HAL_I2C_Mem_Write(&hi2c1,AT24CXX_Write_ADDR,addr,I2C_MEMADD_SIZE_8BIT,data,len,0xFFF);
		if(HAL_OK != res) {return res;}
		
		HAL_Delay(5);
		
	}else{ //一页写不下
		res = HAL_I2C_Mem_Write(&hi2c1,AT24CXX_Write_ADDR,addr,I2C_MEMADD_SIZE_8BIT,data,selectPage_rest,0xFFF);
		//先能写多少写多少
		addr += selectPage_rest;
		len  -= selectPage_rest;
		data += selectPage_rest;
		HAL_Delay(4);
		//按整个页写
		int full_page = len / AT24CXX_PAGE_SIZE ; //如果这里余下的数据不够一页，
		for (int current_page =0;current_page < full_page; current_page++){
				res = HAL_I2C_Mem_Write(&hi2c1,AT24CXX_Write_ADDR,addr,I2C_MEMADD_SIZE_8BIT,data,AT24CXX_PAGE_SIZE,0xFFF);
				if(HAL_OK != res) {return res;}
				
				HAL_Delay(4);
				addr += AT24CXX_PAGE_SIZE;
				len  -= AT24CXX_PAGE_SIZE;
				data += AT24CXX_PAGE_SIZE;
			}
			if( 0 != len){
				res = HAL_I2C_Mem_Write(&hi2c1,AT24CXX_Write_ADDR,addr,I2C_MEMADD_SIZE_8BIT,data,len,0xFFF);
				if ( HAL_OK != res){return res;}
			}
		}
	return HAL_OK;
}


static int EE_read(uint8_t addr,uint8_t *data,uint32_t len){
	int res =  HAL_I2C_Mem_Read(&hi2c1,AT24CXX_Read_ADDR,addr,I2C_MEMADD_SIZE_8BIT,data,len,0xFFF);
	return res;
}