/*
 * sht3x.h
 *
 *  Created on: 2023年4月7日
 *      Author: EthanDL_Wang
 */
#ifndef __SHT3X_H_
#define __SHT3X_H_

extern void sht3x_soft_reset(void);
extern int sht3x_get_temp_and_humi(double *temperature, double *humidity);

#endif

