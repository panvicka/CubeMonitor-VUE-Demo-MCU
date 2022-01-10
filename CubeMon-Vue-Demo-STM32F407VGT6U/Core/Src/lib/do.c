/**
 * @file do.c
 * @author panvicka
 * @date 8.1.2022
 * @brief digital outputs with set/get and toggle functions
 */

#include <lib/do.h>
#include <lib/uti/swo.h>

typedef struct digitalOutputDef {
	uint8_t is_initialized;
	dioStates mx_state;
	digitalOutputInitData inits;
} digitalOutputDef;

static digitalOutputDef outputs[DO_NONE];

retStatus output_init(digOutputs output_name,
		digitalOutputInitData digital_output_init_data) {

	if (output_name >= DO_NONE) {
		swo_print("do: initialization of invalid output");
		return ENODEV;
	}

	if (!IS_GPIO_PIN_ACTION(digital_output_init_data.init_state)) {
		swo_print("do: initialization with invalid state");
		return ENODEV;
	}

	if (!IS_GPIO_PIN(digital_output_init_data.pin)) {
		swo_print("do: initialization with invalid pin");
		return ENODEV;
	}

	struct digitalOutputDef *output = &outputs[output_name];

	output->is_initialized = 1;
	output->inits.pin = digital_output_init_data.pin;
	output->inits.port = digital_output_init_data.port;
	output->inits.init_state = digital_output_init_data.init_state;

	output_set(output_name, digital_output_init_data.init_state);

	return EOK;

}

retStatus output_toggle(digOutputs output_name) {

	if (output_name >= DO_NONE || outputs[output_name].is_initialized == 0) {
		return ENODEV;
	}

	if ((GPIO_PinState) outputs[output_name].mx_state == GPIO_PIN_RESET) {
		output_set_hw_HAL(output_name, GPIO_PIN_SET);
	} else {
		output_set_hw_HAL(output_name, GPIO_PIN_RESET);
	}

	return EOK;
}

retStatus output_set(digOutputs output_name, GPIO_PinState state) {

	if (output_name >= DO_NONE || outputs[output_name].is_initialized == 0) {
		return ENODEV;
	}

	if (state != GPIO_PIN_RESET && state != GPIO_PIN_SET) {
		return EINVAL;
	}

	output_set_hw_HAL(output_name, state);

	return EOK;
}

dioStates output_get_state(digOutputs output_name) {

	if (output_name >= DO_NONE || outputs[output_name].is_initialized == 0) {
		return DIO_NOT_INITIALIZED;
	}

	return (dioStates) outputs[output_name].mx_state;

}

__weak retStatus output_set_hw_HAL(digOutputs output_name, GPIO_PinState state) {

	if (output_name >= DO_NONE || outputs[output_name].is_initialized == 0) {
		swo_print("do: invalid output passed to a static function!");
		return ENODEV;
	}

	struct digitalOutputDef *output = &outputs[output_name];

	output->mx_state = state;
	HAL_GPIO_WritePin(output->inits.port, output->inits.pin, state);
	return EOK;

}

