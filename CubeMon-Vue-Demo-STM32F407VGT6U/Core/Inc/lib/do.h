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
#include <lib/uti/utility.h>

#ifdef LIB_DO
#include <prog/init.h>
#else
typedef enum digOutputs {
	DO_NONE,
}digOutputs;
#endif

typedef struct digitalOutputInitData {
	uint16_t pin;
	GPIO_TypeDef *port;
	GPIO_PinState init_state;
} digitalOutputInitData;

retStatus output_init(digOutputs output,
		digitalOutputInitData digital_output_init_data);

retStatus output_toggle(digOutputs output);
retStatus output_set(digOutputs output_name, GPIO_PinState state);
dioStates output_get_state(digOutputs output);

__weak retStatus output_set_hw_HAL(digOutputs output_name, GPIO_PinState state);

#endif /* INC_DO_DO_H_ */
