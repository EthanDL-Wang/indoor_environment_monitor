#include <stdio.h>
#include <string.h>
#include "sht3x.h"
#include "stm32f1xx_hal.h"
#include "cmsis_os.h"
#include "i2c.h"

uint16_t SHT3X_DEV_ADDR_W = 0x44 << 1;
uint16_t SHT3X_DEV_ADDR_R = (0x44 << 1) + 1;

void sht3x_soft_reset(void)
{

	uint8_t tData[2] = {0x30, 0xa2};

	HAL_I2C_Master_Transmit(&hi2c1, SHT3X_DEV_ADDR_W, tData, sizeof(tData), 0x1000);
	
}

int sht3x_get_temp_and_humi(double *temperature, double *humidity)
{
	HAL_StatusTypeDef res;
	uint8_t tData[2] = {0x2c, 0x06};
	uint8_t rData[8];
	uint16_t tempData;
	uint16_t humiData;
	double test = 0;

	//high repetitive rate, clock stretching enable
	res = HAL_I2C_Master_Transmit(&hi2c1, SHT3X_DEV_ADDR_W, tData, sizeof(tData), 0x1000);
	if(res != HAL_OK){
		return -1;
	}
	
	memset(rData, 0, sizeof(rData));
	res = HAL_I2C_Master_Receive(&hi2c1, SHT3X_DEV_ADDR_R, rData, sizeof(rData), 0x1000);
	if(res != HAL_OK){
		return -1;
	}
	
	//temperature 
	tempData = rData[0]<<8 | rData[1];
	test = (double)tempData;
	test *= 175;
	test /= 65535;
	test -= 45;
	*temperature = test;
	
	//humidity
	humiData = rData[3]<<8 | rData[4];
	test = (double)humiData;
	test *= 100;
	test /= 65535;
	*humidity = test;

	return 0;

}
