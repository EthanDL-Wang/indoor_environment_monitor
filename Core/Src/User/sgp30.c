/*
 * sgp30.c
 *
 *  Created on: May 19, 2023
 *      Author: EthanDL_Wang
 */

/**IIC PIN LINK
VCC     ------> VCC
GND     ------> GND
SCL		------> STM32 IIC1 SCL(PB6)
SDA		------> STM32 IIC1 SDA(PB7)
*/

#include <string.h>
#include "sgp30.h"
#include "i2c.h"
#include "cmsis_os.h"

uint16_t g_sgp30_CO2_ppm = 0;
uint16_t g_sgp30_TVOC_ppb = 0;

static uint16_t SGP30_DEV_ADDR_W = 0x58 << 1;
static uint16_t SGP30_DEV_ADDR_R = (0x58 << 1) | 0x1;
static I2C_HandleTypeDef *sgp30_iic_handle = &hi2c1;

extern osMutexId myMutex01_sgp30Handle;

int sgp30_soft_reset(void)
{
	uint8_t buff[2] = {0x00, 0x06};

	if(HAL_I2C_Master_Transmit(sgp30_iic_handle, SGP30_DEV_ADDR_W, buff, sizeof(buff), 100) == HAL_OK)
	{
		return 0;
	}
	HAL_Delay(1000);
	return -1;
}

int sgp30_init(void)
{
	uint8_t buff_cmd_sgp30_iaq_init[2] = {0x20, 0x03};

	if(HAL_I2C_Master_Transmit(sgp30_iic_handle, SGP30_DEV_ADDR_W, buff_cmd_sgp30_iaq_init, sizeof(buff_cmd_sgp30_iaq_init), 100) != HAL_OK)
	{
		return -1;
	}
	HAL_Delay(10);
	return 0;
}

int sgp30_read_CO2_TVOC(uint16_t *CO2, uint16_t *TVOC)
{
	uint8_t buff_cmd_sgp30_measure_iaq[2] = {0x20, 0x08};
	uint8_t recv_buff[8];

	if(HAL_I2C_Master_Transmit(sgp30_iic_handle, SGP30_DEV_ADDR_W, buff_cmd_sgp30_measure_iaq, sizeof(buff_cmd_sgp30_measure_iaq), 100) != HAL_OK)
	{
		return -1;
	}

	HAL_Delay(12);//10ms
	memset(recv_buff, 0, sizeof(recv_buff));
	if(HAL_I2C_Master_Receive(sgp30_iic_handle, SGP30_DEV_ADDR_R, recv_buff, sizeof(recv_buff), 100) != HAL_OK)
	{
		return -1;
	}
	//osMutexWait(myMutex01_sgp30Handle, 100);
	if(CO2 != NULL)
	{
		*CO2 = (recv_buff[0] << 8) | recv_buff[1];
	}

	if(TVOC != NULL)
	{
		*TVOC = (recv_buff[3] << 8) | recv_buff[4];
	}
	//osMutexRelease(myMutex01_sgp30Handle);
	return 0;
}

int sgp30_read_raw(uint16_t *CO2, uint16_t *TVOC)
{
	uint8_t buff_cmd_sgp30_measure_iaq[2] = {0x20, 0x50};
	uint8_t recv_buff[8];

	if(HAL_I2C_Master_Transmit(sgp30_iic_handle, SGP30_DEV_ADDR_W, buff_cmd_sgp30_measure_iaq, sizeof(buff_cmd_sgp30_measure_iaq), 100) != HAL_OK)
	{
		return -1;
	}

	HAL_Delay(10);//10ms
	memset(recv_buff, 0, sizeof(recv_buff));
	if(HAL_I2C_Master_Receive(sgp30_iic_handle, SGP30_DEV_ADDR_R, recv_buff, sizeof(recv_buff), 100) != HAL_OK)
	{
		return -1;
	}
	//osMutexWait(myMutex01_sgp30Handle, 100);
	if(CO2 != NULL)
	{
		*CO2 = (recv_buff[0] << 8) | recv_buff[1];
	}

	if(TVOC != NULL)
	{
		*TVOC = (recv_buff[3] << 8) | recv_buff[4];
	}
	//osMutexRelease(myMutex01_sgp30Handle);
	return 0;
}

int sgp30_measure_test(uint16_t *data)
{
	uint8_t buff_cmd_sgp30_measure_iaq[2] = {0x20, 0x32};
	uint8_t recv_buff[8];

	if(HAL_I2C_Master_Transmit(sgp30_iic_handle, SGP30_DEV_ADDR_W, buff_cmd_sgp30_measure_iaq, sizeof(buff_cmd_sgp30_measure_iaq), 100) != HAL_OK)
	{
		return -1;
	}

	HAL_Delay(10);//10ms
	memset(recv_buff, 0, sizeof(recv_buff));
	if(HAL_I2C_Master_Receive(sgp30_iic_handle, SGP30_DEV_ADDR_R, recv_buff, sizeof(recv_buff), 100) != HAL_OK)
	{
		return -1;
	}
	//osMutexWait(myMutex01_sgp30Handle, 100);
	if(data != NULL)
	{
		*data = (recv_buff[0] << 8) | recv_buff[1];
	}
	//osMutexRelease(myMutex01_sgp30Handle);
	return 0;
}

int sgp30_get_serial_id(uint16_t *data)
{
	uint8_t buff_cmd_sgp30_measure_iaq[2] = {0x36, 0x82};
	uint8_t recv_buff[8];

	if(HAL_I2C_Master_Transmit(sgp30_iic_handle, SGP30_DEV_ADDR_W, buff_cmd_sgp30_measure_iaq, sizeof(buff_cmd_sgp30_measure_iaq), 100) != HAL_OK)
	{
		return -1;
	}

	HAL_Delay(10);
	memset(recv_buff, 0, sizeof(recv_buff));
	if(HAL_I2C_Master_Receive(sgp30_iic_handle, SGP30_DEV_ADDR_R, recv_buff, sizeof(recv_buff), 100) != HAL_OK)
	{
		return -1;
	}
	//osMutexWait(myMutex01_sgp30Handle, 100);
	if(data != NULL)
	{
		*data = (recv_buff[0] << 8) | recv_buff[1];
	}
	//osMutexRelease(myMutex01_sgp30Handle);
	return 0;
}






