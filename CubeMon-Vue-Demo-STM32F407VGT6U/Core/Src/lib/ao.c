/*
 * ao.c
 *
 *  Created on: Jan 8, 2022
 *      Author: panvicka
 */

#include <lib/ao.h>

#ifdef LIB_AO
DAC_HandleTypeDef hdac;
#endif

typedef enum {
	DAC_CHANNEL_STOP = 0, DAC_CHANNEL_RUN = 1,
} dacChannelStatus;

typedef struct {
	uint16_t raw_value;
	uint32_t channel;
	dacChannelStatus channel_status;
	uint16_t mx_voltage;
	liner_fce voltage_to_raw;
} analogOutputDef;

static analogOutputDef analog_outputs[AO_NONE];

retStatus analog_output_init(ao_inputs an_output_name, uint32_t channel,
		liner_fce linear_fce) {

	if (an_output_name >= AO_NONE) {
		swo_print("ao: initialization of invalid input");
		return ENODEV;
	}

	if (channel != DAC_CHANNEL_1 || channel != DAC_CHANNEL_2) {
		swo_print("ao: initialization with invalid DAC channel");
		return ENODEV;
	}

	if (linear_fce == NULL) {
		swo_print("ao: initialization without linearization function");
		return EFAULT;
	}

	analog_outputs[an_output_name].channel = channel;
	analog_outputs[an_output_name].liner_fce = linear_fce;

}

retStatus analog_output_set(uint16_t an_output_name, uint16_t data_type,
		int32_t value) {

	if (an_output_name >= AO_NONE) {
		return ENODEV;
	}

	switch (data_type) {

	case ANA_DIO_RAW_CURRENT:
		return EACCES;
		break;

	case ANA_DIO_VOLTAGE:
		if (value >= 0 && value <= INT16_MAX) {
			_analog_output_set_voltage(an_output_name, (int16_t) value);
		} else {
			return EOVERFLOW;
		}
		break;

	default:
		return EINVAL;
		break;
	}

	return EOK;

}

void static _analog_output_set_raw(anaOutputs an_output_name, int16_t value) {

	if (value == 0) {
		_dac_stop_start_channel(an_output_name, 0);
		analog_outputs[an_output_name].mx_voltage = 0;
		analog_outputs[an_output_name].raw_value = 0;
	} else {
		_dac_stop_start_channel(an_output_name, 1);
		if (analog_outputs[an_output_name].liner_fce != NULL) {
			uint32_t aux = 0;
			if (analog_outputs[an_output_name].voltage_to_raw != NULL) {
				aux = analog_outputs[an_output_name].voltage_to_raw(value);
			} else {
				swo_print("ao: null pointer in linearization function");
				aux = 0;
			}
#ifdef LIB_AO
			HAL_DAC_SetValue(&hdac, analog_outputs[an_output_name].channel,
					DAC_ALIGN_12B_R, (uint16_t) aux);
#endif
		}

	}

}

retStatus static _dac_stop_start_channel(anaOutputs an_output_name,
		dacChannelStatus status) {

	if (status == DAC_CHANNEL_RUN) {
		if (analog_outputs[an_output_name].channel_status == DAC_CHANNEL_STOP) {
#ifdef LIB_AO
			HAL_DAC_Start(&hdac, analog_outputs[an_output_name].channel);
#endif
			analog_outputs[an_output_name].channel_status = DAC_CHANNEL_RUN;
		}

	} else if (status == DAC_CHANNEL_STOP) {
		if (analog_outputs[an_output_name].channel_status == DAC_CHANNEL_RUN) {
#ifdef LIB_AO
			HAL_DAC_Stop(&hdac, analog_outputs[an_output_name].channel);
#endif
			analog_outputs[an_output_name].channel_status = DAC_CHANNEL_STOP;
		}
	} else {
		return ENODEV;
	}

	return EOK;

}

