/*
 * di.h
 *
 *  Created on: Jan 8, 2022
 *      Author: panvicka
 */

#ifndef INC_DI_DI_H_
#define INC_DI_DI_H_

#include <stm32f4xx_hal.h>
#include <prog/init.h>
#include <lib/uti/utility.h>

typedef enum {
	INPUT_DATA_CURRENT_VALUE,
	INPUT_DATA_DEBOUNCED_VALUE,
	INPUT_DATA_RISING_EDGE,
	INPUT_DATA_FALLING_EDGE,
	INPUT_DATA_DEBOUNC_TIME,
} input_dat_types;

typedef enum {
	INPUT_OFF = 0, INPUT_ON = 1,
} input_states;

ret_state input_get(uint32_t input, uint16_t data, int32_t *value);
ret_state input_set(uint32_t input, uint16_t data, int32_t value);

ret_state input_init(di_inputs input_name, uint16_t gpio_pin,
		GPIO_TypeDef *pin_port, uint16_t threshold);

input_states input_state_now(di_inputs input_name);
input_states input_state_debounced(di_inputs input_name);

void input_handle(void);

#endif /* INC_DI_DI_H_ */
