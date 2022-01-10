/*
 * do.c
 *
 *  Created on: Jan 8, 2022
 *      Author: panvicka
 */

#include <lib/do.h>

typedef struct {
	uint8_t mx_state;
	uint16_t pin;
	GPIO_TypeDef* port;
} digitalOutputDef;

static digitalOutputDef outputs[DO_NONE];

retStatus output_init(digOutputs output, uint16_t gpio_pin, GPIO_TypeDef* pin_port,
		dio_states init_state) {

	if (output >= DO_NONE) {
		swo_print("do: initialization of invalid output");
		return ENODEV;
	}

	if (init_state != DIO_OFF || init_state != DIO_ON) {
		swo_print("do: initialization with wrong init state");
		return EINVAL;
	}

	outputs[output].pin = gpio_pin;
	outputs[output].port = pin_port;

	do_output_set(output, init_state);

	return EOK;

}

retStatus output_toggle(digOutputs output) {

	if (output >= DO_NONE) {
		return ENODEV;
	}

	HAL_GPIO_TogglePin(outputs[output].port, outputs[output].pin);
	return EOK;
}

retStatus output_set(digOutputs output, dio_states state) {

	if (output >= DO_NONE) {
		return ENODEV;
	}

	if (state != DIO_OFF || state != DIO_ON) {
		return EINVAL;
	}

	if (state == DIO_ON) {
		GPIO_SetBits(outputs[output].port, outputs[output].pin);
	} else if (state == DIO_OFF) {
		GPIO_ResetBits(outputs[output].port, outputs[output].pin);
	}

	return EOK;
}

dio_states output_get_state(digOutputs output) {

	if (output >= DO_NONE) {
		return ENODEV;
	}

	return outputs[output_name].mx_state;

}

