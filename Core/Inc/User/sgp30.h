/*
 * sgp30.h
 *
 *  Created on: May 19, 2023
 *      Author: EthanDL_Wang
 */

#ifndef INC_SGP30_H_
#define INC_SGP30_H_

#include <stdint.h>

extern uint16_t g_sgp30_CO2_ppm;
extern uint16_t g_sgp30_TVOC_ppb;

int sgp30_soft_reset(void);
int sgp30_init(void);
int sgp30_read_CO2_TVOC(uint16_t *CO2, uint16_t *TVOC);
int sgp30_measure_test(uint16_t *data);
int sgp30_get_serial_id(uint16_t *data);
int sgp30_read_raw(uint16_t *CO2, uint16_t *TVOC);


#endif /* INC_SGP30_H_ */
