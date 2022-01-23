/**
 * @file di.c
 * @author panvicka
 * @date 2.1.2022
 *
 */

#include <main.h>
#include <lib/uti/utility.h>
#include <lib/uti/swo.h>
#include <lib/di.h>

//todo move me to def
#define ALLOW_CUBEMX_OVERWRITE

typedef enum digitalInputSM {
	INPUT_SM_WAITING_FOR_CHANGE,
	INPUT_SM_DEBOUNC,
	INPUT_SM_CONFIRMED,
	INPUT_SM_BUTTON_PRESS,
} digitalInputSM;

typedef struct digitalInputAction {
	uint8_t detectable;
	uint8_t detected;
} digitalInputAction;

typedef struct digitalInputDef {

	uint8_t is_initialized;

	uint8_t mx_logical_state;
	uint8_t mx_rewrites;
	uint8_t mx_value;

	uint8_t double_press_monitoring;
	uint16_t double_press_counter;

	uint16_t counter;

	GPIO_PinState on_state;
	GPIO_PinState off_state;
	GPIO_PinState last_confirmed_hw_state;

	digitalInputInitData inits;
	digitalInputSM state;
	digitalInputAction actions[INPUT_ACT_NONE];

} digitalInputDef;

static digitalInputDef inputs[DI_NONE];

void static _input_debounc(digitalInputDef *input,
		GPIO_PinState current_HW_state);
dioStates static _input_get_logical_value(digitalInputDef *input,
		GPIO_PinState pin_state);

retStatus input_get(uint32_t input_name, uint16_t type, int32_t *value) {

	if (input_name >= DI_NONE || inputs[input_name].is_initialized == 0) {
		return ENODEV;
	}

	digitalInputDef *input = &inputs[input_name];

	switch (type) {

	case INPUT_DAT_CURR:
		*value = (int32_t) _input_get_logical_value(&inputs[input_name],
				input->inits.input_get_hw_state(input_name));
		break;
	case INPUT_DAT_DEBOUNCED:
		*value = (int32_t) input->mx_logical_state;
		break;

	case INPUT_DAT_RISING_EDGE:
	case INPUT_DAT_FALLING_EDGE:
	case INPUT_DAT_LONG_PRESS:
	case INPUT_DAT_SHORT_PRESS:
	case INPUT_DAT_DOUBLE_PRESS:
		*value = (int32_t) input->actions[type].detected;
		break;

	default:
		return EINVAL;
		break;
	}

	return EOK;

}

retStatus input_set(uint32_t input_name, uint16_t type, int32_t value) {

	if (input_name >= DI_NONE || inputs[input_name].is_initialized == 0) {
		return ENODEV;
	}

	// it is not possible to force the input value (INPUT_DAT_CURR or INPUT_DAT_DEBOUNCED) with setter
	if (type == INPUT_DAT_CURR || type == INPUT_DAT_DEBOUNCED) {
		return EACCES;
	}

	digitalInputDef *input = &inputs[input_name];

	switch (type) {

	case INPUT_DAT_CURR:
	case INPUT_DAT_DEBOUNCED:
		return EACCES;
		break;

	case INPUT_DAT_RISING_EDGE:
	case INPUT_DAT_FALLING_EDGE:
	case INPUT_DAT_SHORT_PRESS:
	case INPUT_DAT_LONG_PRESS:
	case INPUT_DAT_DOUBLE_PRESS:
		if (value != 0) {
			return EOVERFLOW;
		}
		input->actions[type].detected = (uint8_t) value;
		break;

	default:
		return EINVAL;
		break;
	}
	return EOK;

}

retStatus input_get_init_data(digInputs input_name,
		digitalInputInitData *digital_input_init_data) {
	if (input_name >= DI_NONE) {
		return ENODEV;
	}

	*digital_input_init_data = inputs[input_name].inits;

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

	if (input->inits.sw_type != INPUT_SW_ON_OFF_INPUT
			&& input->inits.sw_type != INPUT_SW_BUTTON) {
		swo_print("di: initialization of wrong SW type");
		return EINVAL;
	}

	if (input->inits.input_get_hw_state == NULL) {
		input->inits.input_get_hw_state = input_get_hw_HAL;
	}

	if (input->inits.double_press_spacing != 0) {
		input->actions[INPUT_ACT_DOUBLE_PRESS].detectable = 1;
	}

	if (input->inits.long_press_ms != 0) {
		input->actions[INPUT_ACT_LONG_PRESS].detectable = 1;
	}
	// short press always detectable
	input->actions[INPUT_ACT_SHORT_PRESS].detectable = 1;

	/* get the input hw state at time of initialization and set the detectability
	 * of the rising/falling edge manually once
	 */
	input->last_confirmed_hw_state = input->inits.input_get_hw_state(
			input_name);

	if (input->last_confirmed_hw_state == input->off_state) {
		input->actions[INPUT_ACT_RISING_EDGE].detectable = 1;
	} else {
		input->actions[INPUT_ACT_FALLING_EDGE].detectable = 1;
	}

	input->is_initialized = 1;

	return EOK;

}

dioStates input_state_debounced(digInputs input_name) {
	if (input_name >= DI_NONE || inputs[input_name].is_initialized == 0) {
		return !inputs[input_name].mx_logical_state;
	} else {
		return ENODEV;
	}
}

dioStates input_state_now(digInputs input_name) {
	if (input_name >= DI_NONE || inputs[input_name].is_initialized == 0) {
		return inputs[input_name].inits.input_get_hw_state(input_name);
	} else {
		return ENODEV;
	}
}

void input_handle(void) {
	for (uint8_t i = 0; i < DI_NONE - 1; i++) {
		if (inputs[i].is_initialized == 1) {
			_input_debounc(&inputs[i], inputs[i].inits.input_get_hw_state(i));
		}
	}
}

uint8_t input_get_action(digInputs input_name, digitalInputActions action) {
	if (input_name >= DI_NONE || inputs[input_name].is_initialized == 0) {
		return ENODEV;
	}
	return inputs[input_name].actions[action].detected;
}

retStatus input_ack_action(digInputs input_name, digitalInputActions action) {
	if (input_name >= DI_NONE || inputs[input_name].is_initialized == 0) {
		return ENODEV;
	}
	inputs[input_name].actions[action].detected = 0;
	return EOK;
}

__weak GPIO_PinState input_get_hw_HAL(digInputs input_name) {

	if (input_name >= DI_NONE) {
		swo_print("di: invalid input passed to a static function!");
		return GPIO_PIN_RESET;
	}

	digitalInputDef *input = &inputs[input_name];

#ifdef ALLOW_CUBEMX_OVERWRITE

	if (input->mx_rewrites == 1) {
		if (input->mx_value == 1) {
			return input->on_state;
		} else {
			return input->off_state;
		}
	} else {
		return HAL_GPIO_ReadPin(input->inits.mcu_port, input->inits.mcu_pin);
	}

#else
	return HAL_GPIO_ReadPin(input->inits.mcu_port, input->inits.mcu_pin);
#endif

}

dioStates static _input_get_logical_value(digitalInputDef *input,
		GPIO_PinState pin_state) {

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
		swo_print("di: input in invalid HW state");
		return DIO_PROBLEM_OCCURED;
	}
}

static void _input_set_action_if_possible(digitalInputAction *action_struct,
		digitalInputActions action, uint8_t value) {

	if (action_struct[action].detectable == 1 && value == 1) {
		action_struct[action].detected = 1;
	} else if (value == 0) {
		action_struct[action].detected = 0;
	}

}

void __attribute__((optimize("-O3"))) static _input_debounc(
		digitalInputDef *input, GPIO_PinState current_HW_state) {

	digitalInputAction *actions = input->actions;

	/* set detectability of falling/rising edges
	 * falling edge can be only detectable if the input is in logical "1" state
	 * rising edge can be only detecble if the input is in logical "0" state
	 */
	if (current_HW_state == input->on_state) {
		actions[INPUT_ACT_FALLING_EDGE].detectable = 1;
	} else {
		actions[INPUT_ACT_RISING_EDGE].detectable = 1;
	}

	/*
	 * increase independent timer for double press monitoring
	 * if the timer overflows the limit, we can set the single short press active
	 * to disable double press detection, set the upper limit to "0"
	 */
	if (input->double_press_monitoring == 1) {
		input->double_press_counter =
				(input->double_press_counter < UINT16_MAX) ?
						input->double_press_counter + 1 :
						input->double_press_counter;

		if (input->double_press_counter > input->inits.double_press_spacing) {

			_input_set_action_if_possible(actions, INPUT_ACT_SHORT_PRESS, 1);
			input->double_press_monitoring = 0;
			input->double_press_counter = 0;
		}

	}

	switch (input->state) {

	case INPUT_SM_WAITING_FOR_CHANGE:
		if (current_HW_state != input->last_confirmed_hw_state) {
			input->state = INPUT_SM_DEBOUNC;
			input->counter = 0;
		}
		break;

	case INPUT_SM_DEBOUNC:
		input->counter =
				(input->counter < UINT16_MAX) ?
						input->counter + 1 : input->counter;

		if (input->counter >= input->inits.debounc_time) {
			input->state = INPUT_SM_CONFIRMED;
		}

		// the changed input state was not active long enough, probably a jitter
		if (current_HW_state == input->last_confirmed_hw_state) {
			input->state = INPUT_SM_WAITING_FOR_CHANGE;
		}
		break;

	case INPUT_SM_CONFIRMED:
		// if the previous state was logical "0" and now there is a confirmed "1", rising edge happened
		if (input->last_confirmed_hw_state == input->off_state) {
			_input_set_action_if_possible(actions, INPUT_ACT_RISING_EDGE, 1);
			actions[INPUT_ACT_RISING_EDGE].detectable = 0;

		} else {
			// if the previous state was logical "1" and now there is a confirmed "0", falling edge happened
			_input_set_action_if_possible(actions, INPUT_ACT_FALLING_EDGE, 1);
			actions[INPUT_ACT_FALLING_EDGE].detectable = 0;

		}
		// set the new state as last confirmed HW state and convert to logical value based on input HW type
		input->last_confirmed_hw_state = current_HW_state;
		input->mx_logical_state = _input_get_logical_value(input,
				input->last_confirmed_hw_state);

		if (input->inits.sw_type == INPUT_SW_ON_OFF_INPUT) {
			input->state = INPUT_SM_WAITING_FOR_CHANGE;
		} else {
			/*
			 * in button mode, we may be expecting a double press
			 * check if a double press is detected and if so, do not start the button evaluation
			 * again
			 */
			if (input->double_press_monitoring == 1
					&& current_HW_state == input->on_state
					&& input->double_press_counter
							< input->inits.double_press_spacing) {

				_input_set_action_if_possible(actions, INPUT_ACT_DOUBLE_PRESS,
						1);

				input->double_press_monitoring = 0;
				input->counter = 0;
				input->double_press_counter = 0;
				input->state = INPUT_SM_WAITING_FOR_CHANGE;
			} else {
				input->counter = 0;
				input->state = INPUT_SM_BUTTON_PRESS;
			}

		}

		break;

	case INPUT_SM_BUTTON_PRESS:

		if (current_HW_state == input->on_state) {
			input->counter =
					(input->counter < UINT16_MAX) ?
							input->counter + 1 : input->counter;

			if (input->counter > input->inits.long_press_ms) {
				actions[INPUT_ACT_LONG_PRESS].detected = 1;
			}

		} else if (current_HW_state == input->off_state && input->counter > 0) {
			if (actions[INPUT_ACT_LONG_PRESS].detected == 0) {
				input->state = INPUT_SM_WAITING_FOR_CHANGE;
				input->counter = 0;

				/*
				 * double press detection is turned off, we can set the short press
				 * right away
				 */
				if (actions[INPUT_ACT_DOUBLE_PRESS].detectable == 0) {
					input->double_press_monitoring = 0;
					_input_set_action_if_possible(actions,
							INPUT_ACT_SHORT_PRESS, 1);

				} else {
					input->double_press_monitoring = 1;
				}

			} else {
				input->double_press_monitoring = 0;

				_input_set_action_if_possible(actions, INPUT_ACT_LONG_PRESS, 0);

				input->counter = 0;
				input->state = INPUT_SM_WAITING_FOR_CHANGE;
			}

		} else {
			input->state = INPUT_SM_WAITING_FOR_CHANGE;
			input->counter = 0;
		}
		break;

	default:
		input->state = INPUT_SM_WAITING_FOR_CHANGE;
		swo_print("di: input state machine in invalid state");
		break;

	}

}

