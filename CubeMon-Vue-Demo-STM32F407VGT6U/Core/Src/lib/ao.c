/*
 * ao.c
 *
 *  Created on: Jan 8, 2022
 *      Author: panvicka
 */

#include <lib/ao.h>
#include <lib/uti/swo.h>

#ifdef DAC
DAC_HandleTypeDef hdac;
#endif

typedef enum dacChannelStatus {
	DAC_CHANNEL_STOP = 0, DAC_CHANNEL_RUN = 1,
} dacChannelStatus;

typedef struct analogOutputDef {
	uint8_t is_initialized;
	liner_fce voltage_to_raw_fce;
	uint16_t raw_value;
	uint32_t channel;
	dacChannelStatus channel_status;
	uint16_t mx_voltage;
} analogOutputDef;

static analogOutputDef analog_outputs[AO_NONE];

void static _analog_output_set_voltage(anaOutputs an_output_name, int16_t value);
retStatus static _analog_output_stop_start_channel(anaOutputs an_output_name,
		dacChannelStatus status);

retStatus analog_output_init(anaOutputs an_output_name, uint32_t channel,
		liner_fce linear_fce) {

	if (an_output_name >= AO_NONE) {
		swo_print("ao: initialization of invalid input");
		return ENODEV;
	}

	if (!IS_DAC_CHANNEL(channel)) {
		swo_print("ao: initialization with invalid DAC channel");
		return ENODEV;
	}

	if (linear_fce == NULL) {
		swo_print("ao: initialization without linearization function");
		return EFAULT;
	}

	analog_outputs[an_output_name].is_initialized = 1;
	analog_outputs[an_output_name].channel = channel;
	analog_outputs[an_output_name].voltage_to_raw_fce = linear_fce;

	return EOK;

}

retStatus analog_output_set(uint16_t an_output_name, uint16_t data_type,
		int32_t value) {

	if (an_output_name >= AO_NONE
			|| analog_outputs[an_output_name].is_initialized == 0) {
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

void static _analog_output_set_voltage(anaOutputs an_output_name, int16_t value) {

	struct analogOutputDef *output = &analog_outputs[an_output_name];

	if (value == 0) {
		_analog_output_stop_start_channel(an_output_name, DAC_CHANNEL_STOP);
		output->mx_voltage = 0;
		output->raw_value = 0;
	} else {
		_analog_output_stop_start_channel(an_output_name, DAC_CHANNEL_RUN);
		if (output->voltage_to_raw_fce != NULL) {
			uint32_t aux = output->voltage_to_raw_fce(value);
#ifdef DAC
			HAL_DAC_SetValue(&hdac, output->channel,
			DAC_ALIGN_12B_R, (uint16_t) aux);
#endif
		} else {
			swo_print("ao: null pointer in linearization function");

		}

	}

}

retStatus static _analog_output_stop_start_channel(anaOutputs an_output_name,
		dacChannelStatus status) {

	struct analogOutputDef *output = &analog_outputs[an_output_name];

	if (status == DAC_CHANNEL_RUN) {
		if (output->channel_status == DAC_CHANNEL_STOP) {
#ifdef DAC
			HAL_DAC_Start(&hdac, output->channel);
#endif
			output->channel_status = DAC_CHANNEL_RUN;
		}

	} else if (status == DAC_CHANNEL_STOP) {
		if (output->channel_status == DAC_CHANNEL_RUN) {
#ifdef DAC
			HAL_DAC_Stop(&hdac, output->channel);
#endif
			output->channel_status = DAC_CHANNEL_STOP;
		}
	} else {
		return ENODEV;
	}

	return EOK;

}

uint32_t lin_dac_no_scaling_no_corrections(uint32_t voltage_value) {

	return voltage_value * 330 / 4094;

}

