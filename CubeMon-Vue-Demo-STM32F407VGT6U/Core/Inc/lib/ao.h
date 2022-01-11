/*
 * ao.h
 *
 *  Created on: Jan 8, 2022
 *      Author: panvicka
 */

#ifndef INC_LIB_AO_H_
#define INC_LIB_AO_H_

#include <lib/uti/hw_hal_includes.h>
#include <lib/uti/utility.h>
#include <lib/uti/lib_config.h>

#ifdef LIB_AO
#include <prog/init.h>
#else
typedef enum {
	AO_NONE
}anaOutputs;
#endif

retStatus analog_output_init(anaOutputs an_output_name, uint32_t channel,
		liner_fce linear_fce);

retStatus analog_output_set(uint16_t an_output_name, uint16_t data_type,
		int32_t value);

uint32_t lin_dac_no_scaling_no_corrections(uint32_t voltage_value);

#endif /* INC_LIB_AO_H_ */
