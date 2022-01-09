/*
 * run_time.h
 *
 *  Created on: Jan 9, 2022
 *      Author: panvicka
 */

#ifndef INC_LIB_RUN_TIME_H_
#define INC_LIB_RUN_TIME_H_

#include <lib/uti/utility.h>

ret_state run_time_get(uint32_t time, uint16_t data, int32_t *value);
ret_state run_time_set(uint32_t time, uint16_t data, int32_t value);

#endif /* INC_LIB_RUN_TIME_H_ */
