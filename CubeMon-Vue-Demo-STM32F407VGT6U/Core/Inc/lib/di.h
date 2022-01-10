/*
 * di.h
 *
 *  Created on: Jan 8, 2022
 *      Author: panvicka
 */

#ifndef INC_DI_DI_H_
#define INC_DI_DI_H_

#include <lib/uti/hw_hal_includes.h>
#include <lib/uti/utility.h>
#include <lib/uti/lib_config.h>

#ifdef LIB_DI
#include <prog/init.h>
#else
typedef enum {
	DI_NONE,
}digInputs;
#endif

typedef enum {
	INPUT_DATA_CURRENT_VALUE,
	INPUT_DATA_DEBOUNCED_VALUE,
	INPUT_DATA_RISING_EDGE,
	INPUT_DATA_FALLING_EDGE,
	INPUT_DATA_DEBOUNC_TIME,
} digitalInputDateTypes;

retStatus input_get(uint32_t input, uint16_t data, int32_t *value);
retStatus input_set(uint32_t input, uint16_t data, int32_t value);

retStatus input_init(digInputs input_name, uint16_t gpio_pin,
		GPIO_TypeDef *pin_port, uint16_t threshold);

dio_states input_state_now(digInputs input_name);
dio_states input_state_debounced(digInputs input_name);

void input_handle(void);

#endif /* INC_DI_DI_H_ */
