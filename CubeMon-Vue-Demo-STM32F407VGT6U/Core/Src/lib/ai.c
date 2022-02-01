/*
 * ai.c
 *
 *  Created on: Jan 8, 2022
 *      Author: panvicka
 */

#include <main.h>
#include <lib/ai.h>
#include <lib/uti/utility.h>
#include <lib/uti/swo.h>

typedef struct analogInput {
	uint8_t is_initialized;

	uint32_t total;
	uint16_t count;
	uint16_t averaged_12b;
	uint16_t current_12b;

	uint16_t samples_to_average;

	uint16_t mx_voltage;
	uint8_t mx_rewrites;
	uint16_t mx_value;
	liner_fce liner_fce;
} analogInput;

static analogInput analog_inputs[AI_NONE];
uint32_t ADC_data_buffer[(AI_NONE * 2) + 1];

retStatus analog_input_get(uint16_t analog_input, uint16_t data, int32_t *value) {

	if (analog_input >= AI_NONE
			|| analog_inputs[analog_input].is_initialized == 0) {
		return ENODEV;
	}

	struct analogInput *input = &analog_inputs[analog_input];

	switch (data) {

	case ANA_DIO_RAW_AVERAGED:
		*value = (int32_t) input->averaged_12b;
		break;

	case ANA_DIO_RAW_CURRENT:
		*value = (int32_t) input->current_12b;
		break;

	case ANA_DIO_VOLTAGE:
		*value = (int32_t) input->mx_voltage;
		break;

	default:
		return EINVAL;
		break;
	}

	return EOK;

}

retStatus analog_input_init(anaInputs an_input_name, int32_t sample_from,
		liner_fce linearization_function) {
	if (an_input_name >= AI_NONE) {
		swo_print("ai: initialization of invalid input");
		return ENODEV;
	}

	struct analogInput *input = &analog_inputs[an_input_name];

	if (linearization_function != NULL) {
		input->liner_fce = linearization_function;
	} else {
		swo_print("ai: initialization without linearization function");
		return EFAULT;
	}

	if (sample_from > 0 || sample_from < INT16_MAX) {
		input->samples_to_average = sample_from;
	} else {
		swo_print("ai: initialization samples value out of bounds");
		return EINVAL;
	}

	return EOK;
}

void analog_input_handle(void) {

	for (uint8_t i = 0; i < AI_NONE; i++) {

		struct analogInput *input = &analog_inputs[i];
		if (input->is_initialized == 1) {

			if (input->count >= input->samples_to_average) {

				if (input->samples_to_average <= 0) {
					input->samples_to_average = 1;
					swo_print("ai: almost divided by zero!");
				}

				input->averaged_12b = input->total / input->samples_to_average;
				input->count = 0;
				input->total = 0;

				uint32_t aux = 0;

				if (input->liner_fce != NULL) {
					aux = input->liner_fce(input->averaged_12b);
				} else {
					swo_print("ai: null pointer in linearization function");
				}

#ifdef ALLOW_CUBEMX_OVERWRITE
				if (input->mx_rewrites == 0) {
					input->mx_voltage = aux;
				} else {
					input->mx_voltage = input->mx_value;
				}
#else
				input->mx_voltage = aux;
#endif

			} else {
				input->count++;
				input->total = input->total + ADC_data_buffer[i];
				input->current_12b = ADC_data_buffer[i];
			}
		}
	}
}

uint32_t lin_adc_no_scaling_no_corrections(uint32_t adc_value) {
	return adc_value * 330 / 4094;
}

