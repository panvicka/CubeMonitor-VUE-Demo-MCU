/**
 * @file di.h
 * @author panvicka
 * @date 2.1.2022
 * @brief digital inputs with falling/rising edge, short press, long press and double press detection
 *
 * Both active/low input types can be used. The detection of presses can be deactivated if the inputs is
 * a toggle switch. For the tactile input types, the double press detection can be deactived as well.
 *
 * A support for a direct manipulation of the input state with CUBE MX Monitoring tool is build in. If you
 * want to use it, define flag ALLOW_CUBEMX_OVERWRITE in you project.
 *
 * To use this library define flag LIB_DI in the lib_config.h file and create an enum in init.h file
 * with the inputs you would like to use.
 * You then have to initialize each input with \ref input_init function.
 * @code
 *	typedef enum {
 *	MY_INPUT_1,
 *	MY_INPUT_2,
 *	MY_INPUT_3,
 *	DI_NONE
 *	} digInputs;
 * @endcode
 * @note Do not forget to include the DI_NONE at the end of the enum listing.
 */

#ifndef INC_DI_DI_H_
#define INC_DI_DI_H_

#include <lib/uti/hw_hal_includes.h>
#include <lib/uti/utility.h>
#include <lib/uti/lib_config.h>


#ifdef LIB_DI
#include <prog/init.h>
#else
typedef enum digInputs {
	DI_NONE,
}digInputs;
#endif

typedef GPIO_PinState (*input_get_hw_state)(digInputs input);

typedef enum digitalInputActions {
	INPUT_ACT_FALLING_EDGE,
	INPUT_ACT_RISING_EDGE,
	INPUT_ACT_SHORT_PRESS,
	INPUT_ACT_LONG_PRESS,
	INPUT_ACT_DOUBLE_PRESS,
	INPUT_ACT_NONE
} digitalInputActions;

typedef enum digitalInputHWType {
	/** \brief for inputs that are active if a voltage is applied to the pin (usually with pulldown resistor) */
	INPUT_HW_ACTIVE_HIGH,

	/** \brief for inputs that are activated when pulled to the GND (always with pullup resistor) */
	INPUT_HW_ACTIVE_LOW,
} digitalInputHWType;

typedef enum digitalInputSWType {
	/** \brief only evaluates logical state and rising/falling edge
	 * best used for toggle switches or similar type of input */
	INPUT_SW_ON_OFF_INPUT,

	/** \brief evaluates state and rising/falling edges and also detects three types of button presses
	 *  best used for tactile switches (for example for navigating in the display menu) */
	INPUT_SW_BUTTON,
} digitalInputSWType;

typedef struct digitalInputInitData {
	uint16_t mcu_pin;  ///< MCU GPIO pin number
	GPIO_TypeDef *mcu_port; ///< MCU GPIO port

	/** \brief minimal time [ms] to consider the input change as a valid change
	 * recommended value for mechanical switch is 5-10ms
	 * set to "0" to deactivate this functions */
	uint16_t debounc_time;

	/** \brief minimal time [ms] to consider the input change to logical "true" as a long press
	 * set to "0" to deactivate this functions */
	uint16_t long_press_ms;

	/** \brief maximal time [ms] before two presses that will result in double press,
	 *  recommended value ~60ms
	 * 	set to "0" to deactivate this functions */
	uint16_t double_press_spacing;

	digitalInputHWType hw_type; ///< input hw_type
	digitalInputSWType sw_type; ///< input sw_type

	input_get_hw_state input_get_hw_state; ///< function for getting GPIO state, pass NULL pointer to default to HAL_GPIO_Read
} digitalInputInitData;

typedef enum digitalInputDateTypes {
	INPUT_DAT_CURR, ///< input current logical value (not debounced), reading directly from HW
	INPUT_DAT_DEBOUNCED, ///< input debounced logical value

	/** \brief logical rising edge detected
	 * for INPUT_HW_ACTIVE_HIGH this will be set if the input goes from "0" to "1"
	 * for INPUT_HW_ACTIVE_LOW this will be set if the input goes from "1" to "0"
	 * it must be cleaned:
	 * with /ref input_set function and set value "0"
	 * or with /ref input_ack_action with argument /ref INPUT_ACT_RISING_EDGE */
	INPUT_DAT_RISING_EDGE,

	/** \brief logical falling edge detected
	 * for INPUT_HW_ACTIVE_HIGH this will be set if the input goes from "1" to "0"
	 * for INPUT_HW_ACTIVE_LOW this will be set if the input goes from "0" to "1"
	 * it must be cleaned:
	 * with /ref input_set function and set value "0"
	 * or with /ref input_ack_action with argument /ref INPUT_ACT_FALLING_EDGE */
	INPUT_DAT_FALLING_EDGE,

	/** \brief input short press status
	 * read status with \ref input_get, clean only the short press status with \ref input_set and value "0"
	 * it must be cleaned:
	 * with /ref input_set function and set value "0"
	 * or with /ref input_ack_action with argument /ref INPUT_ACT_SHORT_PRESS */
	INPUT_DAT_SHORT_PRESS,

	/** \brief input long press status
	 * read status with \ref input_get, clean only the long press status with \ref input_set and value "0"
	 * it must be cleaned:
	 * with /ref input_set function and set value "0"
	 * or with /ref input_ack_action with argument /ref INPUT_ACT_LONG_PRESS */
	INPUT_DAT_LONG_PRESS,

	/** \brief input double press status
	 * read status with \ref input_get, clean only the double press status with \ref input_set and value "0"
	 * it must be cleaned:
	 * with /ref input_set function and set value "0"
	 * or with /ref input_ack_action with argument /ref INPUT_ACT_DOUBLE_PRESS */
	INPUT_DAT_DOUBLE_PRESS,
} digitalInputDateTypes;

retStatus input_get(uint32_t input, uint16_t data, int32_t *value);
retStatus input_set(uint32_t input, uint16_t data, int32_t value);

retStatus input_init(digInputs input_name,
		digitalInputInitData digital_input_init_data);

retStatus input_get_init_data(digInputs input_name,
		digitalInputInitData *digital_input_init_data);

retStatus input_ack_action(digInputs input_name, digitalInputActions action);
uint8_t input_get_action(digInputs input_name, digitalInputActions action);

uint8_t input_is_short_press(digInputs input_name);
uint8_t input_is_long_press(digInputs input_name);

dioStates input_state_now(digInputs input_name);
dioStates input_state_debounced(digInputs input_name);

void input_handle(void);

__weak GPIO_PinState input_get_hw_HAL(digInputs input_name);

#endif /* INC_DI_DI_H_ */
