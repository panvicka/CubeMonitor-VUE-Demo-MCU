/*
 * do.h
 *
 *  Created on: Jan 8, 2022
 *      Author: panvicka
 */

#ifndef INC_DO_DO_H_
#define INC_DO_DO_H_

#include <lib/uti/hw_hal_includes.h>
#include <lib/uti/lib_config.h>

#ifdef LIB_DO
#include <prog/init.h>
#else
typedef enum {
	DO_NONE,
}digOutputs;
#endif

retStatus output_init(digOutputs output, uint16_t gpio_pin,
		GPIO_TypeDef* pin_port, dio_states init_state);
retStatus output_toggle(digOutputs output);
retStatus output_set(digOutputs output, dio_states state);
dio_states output_get_state(digOutputs output);

#endif /* INC_DO_DO_H_ */
