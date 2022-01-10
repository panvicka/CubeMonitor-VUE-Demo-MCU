/*
 * ai.h
 *
 *  Created on: Jan 8, 2022
 *      Author: panvicka
 */

#ifndef INC_LIB_AI_H_
#define INC_LIB_AI_H_

#include <lib/uti/hw_hal_includes.h>
#include <lib/uti/utility.h>
#include <lib/uti/lib_config.h>

#ifdef LIB_AI
#include <prog/init.h>
#else
typedef enum {
	AI_NONE
}anaInputs;
#endif

extern uint32_t ADC_data_buffer[(AI_NONE * 2) + 5];

retStatus analog_input_init(anaInputs an_input_name, int32_t sample_from,
		liner_fce linearization_function);

void analog_input_handle(void);

int32_t linearization_no_scaling_no_corrections(int32_t adc_value);

#endif /* INC_LIB_AI_H_ */
