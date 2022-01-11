/*
 * di.c
 *
 *  Created on: Jan 8, 2022
 *      Author: panvicka
 */

#include <main.h>
#include <lib/uti/utility.h>
#include <lib/di.h>

typedef struct digitalInputDef {
	uint16_t state_change_detected;
	uint16_t debounc_counter;
	uint16_t debounc_threshold;

	uint16_t gpio_pin;
	GPIO_TypeDef *pin_port;

	uint8_t mx_state;

	uint8_t mx_rewrites;
	uint8_t mx_value;

	uint8_t ris_edge_detectable;
	uint8_t ris_edge_detected;

	uint8_t fall_edge_detectable;
	uint8_t fall_edge_detected;

	uint8_t long_press_detected;
	uint8_t short_press_detected;
	uint8_t double_press_detected;

	uint8_t touch_detected;
	uint16_t touch_counter;

	digitalInputInitData digital_input_init_data;

} digitalInputDef;

static digitalInputDef inputs[DI_NONE];

void static _di_input_debouns(digInputs input_name);
void static _di_set_input_state(digInputs input_name, uint8_t state);
void static _di_compare_and_count(digInputs input_name);

retStatus input_get(uint32_t input_name, uint16_t type, int32_t *value) {

	if (input_name <= DI_NONE) {
		return ENODEV;
	}

	switch (type) {

	case INPUT_DATA_RISING_EDGE:
		*value = inputs[input_name].ris_edge_detected;
		break;
	case INPUT_DATA_FALLING_EDGE:
		*value = inputs[input_name].fall_edge_detected;
		break;
	case INPUT_DATA_CURRENT_VALUE:
		*value = !(int32_t) HAL_GPIO_ReadPin(inputs[input_name].pin_port,
				inputs[input_name].gpio_pin);
		break;
	case INPUT_DATA_DEBOUNCED_VALUE:
		*value = !inputs[input_name].mx_state;
		break;
	case INPUT_DATA_LONG_PRESS:
		*value = inputs[input_name].long_press_detected;
		break;
	case INPUT_DATA_SHORT_PRESS:
		*value = inputs[input_name].short_press_detected;
		break;
	case INPUT_DATA_DOUBLE_PRESS:
		*value = inputs[input_name].double_press_detected;
		break;

	default:
		return EINVAL;
		break;
	}

	return EOK;

}

retStatus input_set(uint32_t input_name, uint16_t type, int32_t value) {

	if (input_name >= DI_NONE) {
		return ENODEV;
	}

	switch (type) {

	case INPUT_DATA_RISING_EDGE:
		if (value == 0) {
			inputs[input_name].ris_edge_detected = value;
		} else {
			return EOVERFLOW;
		}
		break;
	case INPUT_DATA_FALLING_EDGE:
		if (value == 0) {
			inputs[input_name].fall_edge_detected = value;
		} else {
			return EOVERFLOW;
		}
		break;

	case INPUT_DATA_DEBOUNC_TIME:
		if (value < INT16_MAX) {
			inputs[input_name].debounc_threshold = (uint16_t) value;
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

retStatus input_init(digInputs input_name, uint16_t gpio_pin,
		GPIO_TypeDef *pin_port, digitalInputInitData digital_input_init_data) {

	if (input_name >= DI_NONE) {
		return ENODEV;
	}

	inputs[input_name].gpio_pin = gpio_pin;
	inputs[input_name].pin_port = pin_port;
	inputs[input_name].digital_input_init_data = digital_input_init_data;

	inputs[input_name].mx_state = HAL_GPIO_ReadPin(inputs[input_name].pin_port,
			inputs[input_name].gpio_pin);

	if (inputs[input_name].mx_state == DIO_OFF) {
		inputs[input_name].fall_edge_detectable = 1;
	} else {
		inputs[input_name].ris_edge_detectable = 1;
	}

	return EOK;

}

dio_states input_state_now(digInputs input_name) {
	if (input_name < DI_NONE) {
		return !inputs[input_name].mx_state;
	} else {
		return ENODEV;
	}
}

dio_states input_state_debounced(digInputs input_name) {
	if (input_name < DI_NONE) {
		return HAL_GPIO_ReadPin(inputs[input_name].pin_port,
				inputs[input_name].gpio_pin);
	} else {
		return ENODEV;
	}
}

void input_handle(void) {
	for (uint8_t i = 0; i < DI_NONE; ++i) {
		_di_input_debouns(i);
	}
}

void input_ack_press(digInputs input_name) {
	inputs[input_name].touch_detected = 0;
	inputs[input_name].short_press_detected = 0;
	inputs[input_name].long_press_detected = 0;
	inputs[input_name].touch_counter = 0;
}

void static _di_set_input_state(digInputs input_name, uint8_t state) {

	if (inputs[input_name].mx_rewrites == DIO_ON) {
		inputs[input_name].mx_state = !inputs[input_name].mx_value;
	} else {
		inputs[input_name].mx_state = state;
	}

}
void static _di_input_debouns(digInputs input_name) {

	if (inputs[input_name].mx_rewrites == DIO_OFF) {
		_di_compare_and_count(input_name);
	} else {
		inputs[input_name].mx_state = !inputs[input_name].mx_value;
	}

}

void static _di_compare_and_count(digInputs input_name) {
	uint8_t actual_state;
	actual_state = HAL_GPIO_ReadPin(inputs[input_name].pin_port,
			inputs[input_name].gpio_pin);

	if (actual_state == 0) {
		inputs[input_name].fall_edge_detectable = 1;
	} else {
		inputs[input_name].ris_edge_detectable = 1;
	}

	if (actual_state != inputs[input_name].mx_state) {

		inputs[input_name].touch_detected = 1;
		inputs[input_name].debounc_counter++;

		if (inputs[input_name].debounc_counter
				>= inputs[input_name].digital_input_init_data.debounc_time) {

			if (inputs[input_name].mx_state == DIO_OFF) {
				if (inputs[input_name].fall_edge_detectable == 1) {
					inputs[input_name].fall_edge_detected = 1;
					inputs[input_name].fall_edge_detectable = 0;
				}
			} else {
				if (inputs[input_name].ris_edge_detectable == 1) {
					inputs[input_name].ris_edge_detected = 1;
					inputs[input_name].ris_edge_detectable = 0;
				}
			}

			_di_set_input_state(input_name, actual_state);

		}

	} else {
		inputs[input_name].state_change_detected = 0;
		inputs[input_name].debounc_counter = 0;
	}

	// todo button may be pulled down
	if (inputs[input_name].touch_detected == 1) {

		if (actual_state == 0) {
			if (inputs[input_name].touch_counter < INT16_MAX) {
				inputs[input_name].touch_counter++;
			}
		}

		if (actual_state == 1
				&& inputs[input_name].touch_counter
						< inputs[input_name].digital_input_init_data.long_press_duration
				&& inputs[input_name].long_press_detected == 0) {
			inputs[input_name].short_press_detected = 1;
		} else if (inputs[input_name].debounc_counter
				>= inputs[input_name].long_press_detected) {
			inputs[input_name].long_press_detected = 1;
			inputs[input_name].debounc_counter = 0;
		}

		if (inputs[input_name].long_press_detected == 1 && actual_state == 1) {
			inputs[input_name].short_press_detected = 0;
			inputs[input_name].long_press_detected = 0;
			inputs[input_name].touch_detected = 0;
		}

	}

}

