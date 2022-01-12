/*
 * di.c
 *
 *  Created on: Jan 8, 2022
 *      Author: panvicka
 */

#include <main.h>
#include <lib/uti/utility.h>
#include <lib/uti/swo.h>
#include <lib/di.h>

//todo move me to def
#define ALLOW_CUBEMX_OVERWRITE

typedef struct digitalInputDef {
	uint16_t debounc_counter;

	uint8_t mx_state;
	uint8_t mx_rewrites;
	uint8_t mx_value;

	uint8_t ris_edge_detectable;
	uint8_t ris_edge_detected;

	uint8_t fall_edge_detectable;
	uint8_t fall_edge_detected;

	uint16_t long_press_detected;
	uint8_t short_press_detected;
	uint8_t double_press_detected;

	uint8_t change_detected;
	uint16_t touch_counter;
	uint16_t double_press_counter;

	uint8_t touch_detected;

	GPIO_PinState on_state;
	GPIO_PinState off_state;
	GPIO_PinState last_confirmed_hw_state;

	digitalInputInitData inits;

} digitalInputDef;

static digitalInputDef inputs[DI_NONE - 1];

void static _input_debounc(digInputs input_name);
dio_states static _input_get_logical_value(digInputs input_name,
		GPIO_PinState pin_state);

retStatus input_get(uint32_t input_name, uint16_t type, int32_t *value) {

	if (input_name <= DI_NONE) {
		return ENODEV;
	}

	digitalInputDef *input = &inputs[input_name];

	switch (type) {

	case INPUT_DATA_RISING_EDGE:
		*value = input->ris_edge_detected;
		break;
	case INPUT_DATA_FALLING_EDGE:
		*value = input->fall_edge_detected;
		break;
	case INPUT_DATA_CURRENT_VALUE:
		*value = _input_get_logical_value(input_name,
				input_get_hw_state(input_name));
		break;
	case INPUT_DATA_DEBOUNCED_VALUE:
		*value = input->mx_state;
		break;
	case INPUT_DATA_LONG_PRESS:
		*value = input->long_press_detected;
		break;
	case INPUT_DATA_SHORT_PRESS:
		*value = input->short_press_detected;
		break;
	case INPUT_DATA_DOUBLE_PRESS:
		*value = input->double_press_detected;
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

	digitalInputDef *input = &inputs[input_name];

	switch (type) {

	case INPUT_DATA_RISING_EDGE:
		if (value == 0) {
			input->ris_edge_detected = (uint8_t) value;
		} else {
			return EOVERFLOW;
		}
		break;
	case INPUT_DATA_FALLING_EDGE:
		if (value == 0) {
			input->fall_edge_detected = (uint8_t) value;
		} else {
			return EOVERFLOW;
		}
		break;

	case INPUT_DATA_DEBOUNC_TIME:
		if (value < UINT16_MAX) {
			input->inits.debounc_time = (uint16_t) value;
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

retStatus input_init(digInputs input_name,
		digitalInputInitData digital_input_init_data) {

	if (input_name >= DI_NONE) {
		swo_print("di: initialization of non-existent input");
		return ENODEV;
	}

	struct digitalInputDef *input = &inputs[input_name];

	input->inits = digital_input_init_data;

	if (input->inits.hw_type == INPUT_HW_ACTIVE_HIGH) {
		// inputs that are connected to a signal for activation needs "on-state" to be defined as "1"
		input->on_state = GPIO_PIN_SET;
		input->off_state = GPIO_PIN_RESET;
	} else if (input->inits.hw_type == INPUT_HW_ACTIVE_LOW) {
		// inputs that are pulled to GND for activation needs "on-state" to be defined as "0"
		input->on_state = GPIO_PIN_RESET;
		input->off_state = GPIO_PIN_SET;
	} else {
		swo_print("di: initialization of wrong HW type");
		return EINVAL;
	}

	/* get the input hw state at time of initialization and set the detectability
	 * of the rising/falling edge manually once
	 */
	input->last_confirmed_hw_state = input_get_hw_state(input_name);

	if (input->last_confirmed_hw_state == input->off_state) {
		input->ris_edge_detectable = 1;
	} else {
		input->fall_edge_detectable = 1;
	}

	return EOK;

}

dio_states input_state_debounced(digInputs input_name) {
	if (input_name < DI_NONE) {
		return !inputs[input_name].mx_state;
	} else {
		return ENODEV;
	}
}

dio_states input_state_now(digInputs input_name) {
	if (input_name < DI_NONE) {
		return input_get_hw_state(input_name);
	} else {
		return ENODEV;
	}
}

void input_handle(void) {
	for (uint8_t i = 0; i < DI_NONE; ++i) {
		_input_debounc(i);
	}
}

uint8_t input_is_long_press(digInputs input_name) {
	if (input_name >= DI_NONE)
		return ENODEV;
	return inputs[input_name].long_press_detected;
}

uint8_t input_is_short_press(digInputs input_name) {
	if (input_name >= DI_NONE)
		return ENODEV;
	return inputs[input_name].short_press_detected;
}

retStatus input_ack_press(digInputs input_name) {

	if (input_name >= DI_NONE)
		return ENODEV;

	digitalInputDef *input = &inputs[input_name];
	input->change_detected = 0;
	input->short_press_detected = 0;
	input->long_press_detected = 0;
	input->touch_counter = 0;

	return EOK;
}

__weak GPIO_PinState input_get_hw_state(digInputs input_name) {

	digitalInputDef *input = &inputs[input_name];

#ifdef ALLOW_CUBEMX_OVERWRITE

	if (input->mx_rewrites == 1) {
		if (input->mx_value == 1) {
			return input->on_state;
		} else {
			return input->off_state;
		}
	} else {
		return HAL_GPIO_ReadPin(inputs[input_name].inits.mcu_port,
				inputs[input_name].inits.mcu_pin);
	}

#else
	return HAL_GPIO_ReadPin(inputs[input_name].inits.mcu_port,
			inputs[input_name].inits.mcu_pin);
#endif

}

dio_states static _input_get_logical_value(digInputs input_name,
		GPIO_PinState pin_state) {

	if (input_name > DI_NONE) {
		swo_print("di: invalid input passed to a static function");
		return ENODEV;
	}

	digitalInputDef *input = &inputs[input_name];

	if (pin_state == GPIO_PIN_RESET) {

		if (input->off_state == GPIO_PIN_RESET) {
			return 0;
		} else {
			return 1;
		}
	} else if (pin_state == GPIO_PIN_SET) {
		if (input->on_state == GPIO_PIN_SET) {
			return 1;
		} else {
			return 0;
		}
	} else {
		swo_print("di: input in invalid hw state");
		return DIO_PROBLEM_OCCURED;
	}
}

void __attribute__((optimize("-O3"))) static _input_debounc(
		digInputs input_name) {

	digitalInputDef *input = &inputs[input_name];

	GPIO_PinState current_HW_state = input_get_hw_state(input_name);

	/* set detectability of falling/rising edges
	 * falling edge can be only detectable if the input is in logical "1" state
	 * rising edge can be only detecble if the input is in logical "0" state
	 */
	if (current_HW_state == input->on_state) {
		input->fall_edge_detectable = 1;
	} else {
		input->ris_edge_detectable = 1;
	}

	// check if there is a change of the input state
	if (current_HW_state != input->last_confirmed_hw_state) {

		// trigger button press recognition algorithms as well
		input->touch_detected = 1;

		// increase debounc counter as long as the change holds but do not overfloat
		if (input->debounc_counter < UINT16_MAX) {
			input->debounc_counter++;
		}

		if (input->debounc_counter >= input->inits.debounc_time) {
			// the change on the input was a valid input change

			// if the previous state was logical "0" and now there is a confirmed "1", rising edge happened
			if (input->last_confirmed_hw_state == input->off_state) {
				if (input->ris_edge_detectable == 1) {
					input->ris_edge_detected = 1;
					input->ris_edge_detectable = 0;
				}
			} else {
				// if the previous state was logical "1" and now there is a confirmed "0", falling edge happened
				if (input->fall_edge_detectable == 1) {
					input->fall_edge_detected = 1;
					input->fall_edge_detectable = 0;
				}

			}

			// set the new state as last confirmed HW state and convert to logical value based on input HW type
			input->last_confirmed_hw_state = current_HW_state;
			input->mx_state = _input_get_logical_value(input_name,
					input->last_confirmed_hw_state);

		}

	} else {
		// the change was not active long enough, could be a EMI or a mechanical button jitter
		input->debounc_counter = 0;
	}

	if (input->inits.sw_type == INPUT_SW_BUTTON) {

		if (input->touch_detected == 1) {
			// change on the input detected, increase the touch counter

			if (current_HW_state == input->on_state) {
				if (input->touch_counter < UINT16_MAX) {
					input->touch_counter++;
				}
			}

			// if the touch counter is larger then the minimal time for long time press, active it
			if (input->touch_counter > input->inits.long_press_ms) {
				input->long_press_detected = 1;
			}

			/*
			 * if was the button was active longer then the debounc limit and then it was released
			 * start the timer for double press detection
			 * if it was released too soon, it may have been only a jitter
			 */
			if (current_HW_state == input->off_state) {
				if (input->touch_counter > input->inits.debounc_time) {
					input->double_press_counter++;
				} else {
					input->touch_detected = 0;
					input->touch_counter = 0;
				}
			}

			// if the input changes to logic "1" again with the counter started, we detected a double press
			// clean the timers and exit the algoritmus with setting  touch_detected to 0
			if (current_HW_state == input->on_state
					&& input->double_press_counter > 0
					&& input->double_press_counter
							< input->inits.double_press_spacing) {
				input->double_press_detected = 1;
				input->touch_detected = 0;
				input->touch_counter = 0;
				input->double_press_counter = 0;
			}

			/*
			 * if the double press counter reaches the maximal value for press spacing,
			 * we may have detected only a single press
			 */
			if (input->double_press_counter
					> input->inits.double_press_spacing) {

				if (input->touch_counter > input->inits.debounc_time
						&& input->touch_counter < input->inits.long_press_ms) {
					/*
					 * it the press was longer then the debounc limit but shorter then the long press,
					 * it was a short press
					 */
					input->short_press_detected = 1;

				}

				input->double_press_counter = 0;
				input->touch_detected = 0;
				input->touch_counter = 0;
			}

			if (input->long_press_detected == 1
					&& current_HW_state == input->off_state) {
				input->long_press_detected = 0;
				input->touch_detected = 0;
			}
		}
	}
}

