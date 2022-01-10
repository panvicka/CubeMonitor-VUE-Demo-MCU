/*
 * ai.c
 *
 *  Created on: Jan 8, 2022
 *      Author: panvicka
 */

#include <lib/ai.h>
#include <lib/uti/utility.h>

typedef struct {
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

static struct analog_di_mcu analog_inputs[AI_NONE];
uint32_t ADC_data_buffer[(AI_NONE * 2) + 5];

retStatus analog_input_get(uint16_t analog_input, uint16_t data, int32_t *value) {

	if (analog_input >= AI_NONE) {
		return ENODEV;
	}

	switch (data) {

	case ANA_DIO_RAW_AVERAGED:
		*value = (int32_t) analog_inputs[analog_input].averaged_12b;
		break;

	case ANA_DIO_RAW_CURRENT:
		*value = (int32_t) analog_inputs[analog_input].current_12b;
		break;

	case ANA_DIO_VOLTAGE:
		if (analog_inputs[analog_input].mx_rewrites == 1) {
			analog_inputs[analog_input].mx_voltage =
					analog_inputs[analog_input].mx_value;
		}
		*value = (int32_t) analog_inputs[analog_input].mx_voltage;
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

	if (linearization_function != NULL) {
		analog_inputs[an_input_name].liner_fce = linearization_function;
	} else {
		swo_print("ai: initialization without linearization function");
		return EFAULT;
	}

	if (sample_from > 0 || sample_from < INT16_MAX) {
		analog_inputs[an_input_name].samples_to_average = sample_from;
	} else {
		swo_print("ai: initialization samples value out of bounds");
		return EINVAL;
	}

	return EOK;
}

void analog_input_handle(void) {

	for (uint8_t i = 0; i < AI_NONE; i++) {
		if (analog_inputs[i].count >= analog_inputs[i].samples_to_average) {

			// ensure we are not going to be diving by 0
			int16_t average_from = analog_inputs[i].samples_to_average;
			if (average_from <= 0) {
				average_from = 1;
			}

			analog_inputs[i].averaged_12b = analog_inputs[i].total
					/ average_from;
			analog_inputs[i].count = 0;
			analog_inputs[i].total = 0;

			uint32_t aux = 0;

			if (analog_inputs[i].liner_fce != NULL) {
				aux = analog_inputs[i].liner_fce(analog_inputs[i].averaged_12b);
			}

			if (analog_inputs[i].mx_rewrites == 0) {
				analog_inputs[i].mx_voltage = aux;
			} else {
				analog_inputs[i].mx_voltage = analog_inputs[i].mx_value;
			}

		} else {
			analog_inputs[i].count++;
			analog_inputs[i].total = analog_inputs[i].total
					+ ADC_data_buffer[i];
			analog_inputs[i].current_12b = ADC_data_buffer[i];

		}

	}

}

int32_t lin_adc_no_scaling_no_corrections(int32_t adc_value) {
	return adc_value * 330 / 4094;
}

