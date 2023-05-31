/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include "i2c.h"
#include "sgp30.h"
#include "st7735.h"
#include "rtc.h"
#include "sht3x.h"
#include "bmp280.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
BMP280_HandleTypedef bmp280;
/* USER CODE END Variables */
osThreadId defaultTaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 256);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* USER CODE BEGIN StartDefaultTask */

	char buff[16];
	uint16_t CO2_ppm, TVOC_ppb;
	double temp, humi;
	uint16_t text_color, background_color;
	float pressure, temperature;

	bmp280_init_default_params(&bmp280.params);
	bmp280.addr = BMP280_I2C_ADDRESS_0;
	bmp280.i2c = &hi2c1;

	sgp30_init();
	HAL_Delay(100);
	bmp280_init(&bmp280, &bmp280.params);
	ST7735_Init();


	background_color = ST7735_WHITE;
	ST7735_FillScreen(background_color);

	/* Infinite loop */
	for(;;)
	{
		sht3x_get_temp_and_humi(&temp, &humi);
		sgp30_read_CO2_TVOC(&CO2_ppm, &TVOC_ppb);
		bmp280_read_float(&bmp280, &temperature, &pressure, NULL);

		//temperature
		if(temp <= 18){
			text_color = ST7735_BLUE;
		} else if(temp > 18 && temp <= 28){
			text_color = ST7735_GREEN;
		} else if(temp > 28 && temp <= 35){
			text_color = ST7735_YELLOW;
		} else if(temp > 35){
			text_color = ST7735_RED;
		}
		memset(buff, 0, sizeof(buff));
		snprintf(buff, sizeof(buff), "%.2f", temp);
		ST7735_FillRectangle(0, 0*16, 128, 1*16, ST7735_WHITE);
		ST7735_Draw_Chinese_16X16_by_ethan(0*8, 0*16, g_chinese_front_16X16, 0, text_color, ST7735_WHITE);
		ST7735_Draw_Chinese_16X16_by_ethan(2*8, 0*16, g_chinese_front_16X16, 3, text_color, ST7735_WHITE);
		ST7735_Draw_Char_8X16_by_ethan(4*8, 0, ':', text_color, ST7735_WHITE);
		ST7735_Draw_String_8X16_by_ethan(5*8, 0*16, buff, strlen(buff), text_color, ST7735_WHITE);
		ST7735_Draw_Temperature_Symbol_16X16((5+strlen(buff))*8, 0*16, text_color, ST7735_WHITE);

		//humidity
		background_color = ST7735_WHITE;
		if(humi <= 45){
			text_color = ST7735_BLUE;
		} else if(humi > 45 && humi <= 65){
			text_color = ST7735_GREEN;
			//background_color = ST7735_YELLOW;
		} else if(humi > 65 && humi <= 80){
			text_color = ST7735_YELLOW;
		} else if(humi > 80){
			text_color = ST7735_RED;
		}
		memset(buff, 0, sizeof(buff));
		snprintf(buff, sizeof(buff), "%.2f", humi);
		ST7735_FillRectangle(0, 1*16, 128, 2*16, background_color);
		ST7735_Draw_Chinese_16X16_by_ethan(0*8, 1*16, g_chinese_front_16X16, 1, text_color, background_color);
		ST7735_Draw_Chinese_16X16_by_ethan(2*8, 1*16, g_chinese_front_16X16, 3, text_color, background_color);
		ST7735_Draw_Char_8X16_by_ethan(4*8, 1*16, ':', text_color, background_color);
		ST7735_Draw_String_8X16_by_ethan(5*8, 1*16, buff, strlen(buff), text_color, background_color);
		ST7735_Draw_Char_8X16_by_ethan((5+strlen(buff))*8, 1*16, '%', text_color, background_color);


		//CO2
		background_color = ST7735_WHITE;
		if(CO2_ppm <= 500){
			text_color = ST7735_GREEN;
			//background_color = ST7735_ORANGE;
		} else if(CO2_ppm > 500 && CO2_ppm <= 1000){
			text_color = ST7735_YELLOW;
		} else if(CO2_ppm > 1000 && CO2_ppm <= 2000){
			text_color = ST7735_ORANGE;
		} else if(CO2_ppm > 2000){
			text_color = ST7735_RED;
		}
		memset(buff, 0, sizeof(buff));
		snprintf(buff, sizeof(buff), "%d", CO2_ppm);
		ST7735_FillRectangle(0, 2*16, 128, 3*16, background_color);
		ST7735_Draw_String_8X16_by_ethan(0, 2*16, " CO2:", strlen(" CO2:"), text_color, background_color);
		ST7735_Draw_String_8X16_by_ethan(5*8, 2*16, buff, strlen(buff), text_color, background_color);
		ST7735_Draw_String_8X16_by_ethan((5+strlen(buff))*8, 2*16, "ppm", strlen("ppm"), text_color, background_color);

		//TVOC
		if(TVOC_ppb <= 50){
			text_color = ST7735_GREEN;
		} else if(TVOC_ppb > 50 && TVOC_ppb <= 100){
			text_color = ST7735_YELLOW;
		} else if(TVOC_ppb > 100 && TVOC_ppb <= 500){
			text_color = ST7735_ORANGE;
		} else if(TVOC_ppb > 500){
			text_color = ST7735_RED;
		}
		memset(buff, 0, sizeof(buff));
		snprintf(buff, sizeof(buff), "%d", TVOC_ppb);
		ST7735_FillRectangle(0, 3*16, 128, 4*16, ST7735_WHITE);
		ST7735_Draw_String_8X16_by_ethan(0, 3*16, "TVOC:", strlen("TVOC:"), text_color, ST7735_WHITE);
		ST7735_Draw_String_8X16_by_ethan(5*8, 3*16, buff, strlen(buff), text_color, ST7735_WHITE);
		ST7735_Draw_String_8X16_by_ethan((5+strlen(buff))*8, 3*16, "ppb", strlen("ppb"), text_color, ST7735_WHITE);

		//pressure
		text_color = ST7735_GREEN;
		pressure /= 1000;
		memset(buff, 0, sizeof(buff));
		snprintf(buff, sizeof(buff), "%.3f", pressure);
		ST7735_FillRectangle(0, 4*16, 128, 5*16, ST7735_WHITE);
		ST7735_Draw_Chinese_16X16_by_ethan(0*8, 4*16, g_chinese_front_16X16, 4, text_color, ST7735_WHITE);
		ST7735_Draw_Chinese_16X16_by_ethan(2*8, 4*16, g_chinese_front_16X16, 5, text_color, ST7735_WHITE);
		ST7735_Draw_Char_8X16_by_ethan(4*8, 4*16, ':', text_color, ST7735_WHITE);
		ST7735_Draw_String_8X16_by_ethan(5*8, 4*16, buff, strlen(buff), text_color, ST7735_WHITE);
		ST7735_Draw_String_8X16_by_ethan((5+strlen(buff))*8, 4*16, "KPa", strlen("KPa"), text_color, ST7735_WHITE);

		//time
		RTC_TimeTypeDef data_time;
		HAL_RTC_GetTime(&hrtc, &data_time, RTC_FORMAT_BIN);
		memset(buff, 0, sizeof(buff));
		snprintf(buff, sizeof(buff), "%d:%d:%d", data_time.Hours, data_time.Minutes, data_time.Seconds);
		ST7735_FillRectangle(0, 5*16, 128, 6*16, ST7735_WHITE);
		ST7735_Draw_String_8X16_by_ethan(0*8, 16*5, "uptime:", strlen("uptime:"), text_color, ST7735_WHITE);
		ST7735_Draw_String_8X16_by_ethan((0+strlen("uptime:"))*8, 16*5, buff, strlen(buff), text_color, ST7735_WHITE);

		HAL_Delay(2000);
	}
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

