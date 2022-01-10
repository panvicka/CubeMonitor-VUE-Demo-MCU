/**
 * @file ao.h
 * @author panvicka
 * @date 8.1.2022
 * @brief analog output library
 *
 * To use this library define flag LIB_AO in the lib_config.h file and create an enum in init.h file
 * with the outputs you would like to use.
 * You then have to initialize each input with \ref input_init function.
 * @code
 *	typedef enum anaOutputs {
 *	MY_ANALOG_OUTPUT_1,
 *	MY_ANALOG_OUTPUT_2,
 *	MY_ANALOG_OUTPUT_3,
 *	AO_NONE
 *	} anaOutputs;
 * @endcode
 * @note Do not forget to include the AO_NONE at the end of the enum listing.
 */

#ifndef INC_LIB_AO_H_
#define INC_LIB_AO_H_

#include <lib/uti/hw_hal_includes.h>
#include <lib/uti/utility.h>
#include <lib/uti/lib_config.h>

#ifdef LIB_AO
#include <prog/init.h>
#else
/**
 * If the library is not used (@b LIB_AO is not defined as symbol) this dummy enum will be created
 * to prevent compilation errors
 */
typedef enum anaOutputs {
	AO_NONE
}anaOutputs;
#endif

retStatus analog_output_init(anaOutputs an_output_name, uint32_t channel,
		liner_fce linear_fce);

retStatus analog_output_set(uint16_t an_output_name, uint16_t data_type,
		int32_t value);

uint32_t lin_dac_no_scaling_no_corrections(uint32_t voltage_value);

#endif /* INC_LIB_AO_H_ */
