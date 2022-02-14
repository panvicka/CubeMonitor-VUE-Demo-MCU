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
 * You then have to initialize each input with \ref input_init() function.
 * @code
 *	typedef enum digInputs{
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
/**
 * If the library is not used (@b LIB_DI is not defined as symbol) this dummy enum will be created
 * to prevent compilation errors
 */
typedef enum digInputs {
	DI_NONE,
}digInputs;
#endif

typedef GPIO_PinState (*input_get_hw_state)(digInputs input);


/// possible input actions
typedef enum digitalInputActions {
	INPUT_ACT_FALLING_EDGE,  ///< falling edge of the input @b logical state
	INPUT_ACT_RISING_EDGE,  ///< rising edge of the input @b logical state
	INPUT_ACT_SHORT_PRESS, ///< short press (input active for a defined debounced time and release again, no second press follows)
	INPUT_ACT_LONG_PRESS, ///< input is hold active for defined time
	INPUT_ACT_DOUBLE_PRESS, ///< two short presses with maximal time between them
	INPUT_ACT_NONE ///< dummy so we can always have the enum length
} digitalInputActions;

/// possible input HW types
typedef enum digitalInputHWType {
	/** \brief for inputs that are active if a voltage is applied to the pin (usually with pulldown resistor) */
	INPUT_HW_ACTIVE_HIGH,

	/** \brief for inputs that are activated when pulled to the GND (always with pullup resistor) */
	INPUT_HW_ACTIVE_LOW,
} digitalInputHWType;

/// possible input SW types
typedef enum digitalInputSWType {
	/** \brief only evaluates logical state and rising/falling edge
	 * best used for toggle switches or similar type of input */
	INPUT_SW_ON_OFF_INPUT,

	/** \brief evaluates state and rising/falling edges and also detects three types of button presses
	 *  best used for tactile switches (for example for navigating in the display menu) */
	INPUT_SW_BUTTON,
} digitalInputSWType;

/// digital input initialization structure
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


/// digital input data
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

/**
 * @brief Standardized getter function for digital input library.
 *
 * @param[in] input is the input name defined by \ref digInputs
 * @param[in] data data for /p input that can be read out of the module, available data \ref digitalInputDateTypes
 * @param[out] value getter output
 *
 * @note /p input and /p data types are changed to @c uint32_t and @c uint16_t to keep the standard function prototype
 * @note In case you do not need this function footprint you are better of using input_is_short_press(),
 * input_is_long_press() and input_get_action() functions.
 *
 * @return \ref ENODEV if the /p input is not defined or initialized, \ref EINVAL
 * if you are trying to read invalid data and \ref EOK if the /p value contains valid value
 */
retStatus input_get(uint32_t input, uint16_t data, int32_t *value);

/**
 * @brief Standardized setter function for digital input library.
 *
 * @param[in] input input identification, defined by \ref digInputs
 * @param[in] data data for /p input that can be read out of the module, available data \ref digitalInputDateTypes
 * @param[in] value value to be set
 *
 * You can not set values that are read from HW or calculated from the HW value: \ref INPUT_DAT_CURR and \ref INPUT_DAT_DEBOUNCED
 *
 *
 * @note \p input and /p data types are changed to @c uint32_t and @c uint16_t to keep the standard function prototype
 * @note In case you do not need this function footprint you are better of using input_ack_action()
 *
 * @return returns \ref EACCES in case you try setting HW dependent variables, \ref EOVERFLOW if the set value is invalid, \ref EINVAL
 * if invalid data type is passed, \ref ENODEV in case the input is not defined or initialized and \ref EOK if the value was
 * successfully set
 */
retStatus input_set(uint32_t input, uint16_t data, int32_t value);

/**
 * @brief Digital input initialization function
 *
 * @param[in] input_name input identification, defined by \ref digInputs
 * @param[in] digital_input_init_data input initialization data \ref digitalInputInitData
 *
 * @return  \ref EOK is the initialization completed OK
 * 			\ref ENODEV if the \p input_name is out of range defined by \ref digInputs
 * 			\ref EINVAL if the \p digital_input_init_data contains invalid values (check SWO)
 *
 */
retStatus input_init(digInputs input_name,
		digitalInputInitData digital_input_init_data);

/**
 * @brief Return input initialization data
 *
 * In case you want to re-initialize some values but keep the rest as it was
 *
 * @param[in] input_name input identification, defined by \ref digInputs
 * @param[out] digital_input_init_data input initialization data \ref digitalInputInitData
 *
 * @return  \ref EOK if the \p digital_input_init_data value is valid
 * 			\ref ENODEV if the \p input_name is out of range defined by \ref digInputs
 *
 */
retStatus input_get_init_data(digInputs input_name,
		digitalInputInitData *digital_input_init_data);

/**
 * @brief Acknowledge digital input action
 *
 * Digital input action \ref digitalInputActions needs to be manually cleared before they are
 * evaluated again.
 *
 * @param[in] input_name input identification, defined by \ref digInputs
 * @param[out] action action to be cleared
 *
 * @return  \ref EOK if the \p digital_input_init_data value is valid
 * 			\ref ENODEV if the \p input_name is out of range defined by \ref digInputs
 *
 */
retStatus input_ack_action(digInputs input_name, digitalInputActions action);

//todo rename this + maybe boolean?
uint8_t input_get_action(digInputs input_name, digitalInputActions action);

/**
 * @brief Checks if the input detected short press.
 * After reading out the value, you need to clean the action flag with input_ack_action() or input_set()
 * @param[in] input_name input identification, defined by \ref digInputs
 * @return  1 - short press detected
 * 			0 - short press not detected
 */
uint8_t input_is_short_press(digInputs input_name);

/**
 * @brief Checks if the input detected long press.
 * @param[in] input_name input identification, defined by \ref digInputs
 * @note action \ref INPUT_ACT_LONG_PRESS is self-cleaning (action active only if the input is in the long-hold state)
 * @return  1 - input is being hold in "active" state that classifies as "long press"
 * 			0 - long press not active
 */
uint8_t input_is_long_press(digInputs input_name);

/**
 * @brief Return the input state read from HW at the moment the function is called.
 * @param[in] input_name input identification, defined by \ref digInputs
 * @note the value is not converted to a logical state (you need to consider the \ref INPUT_HW_ACTIVE_HIGH and \ref INPUT_HW_ACTIVE_LOW
 * 		in your logic *
 * @return  DIO_OFF - if the value on HW is "0"
 * 			DIO_ON - if the value on HW is "1
 * 			ENODEV - if the /p input_name is out of range defined by \ref digInputs
 */
dioStates input_state_now(digInputs input_name);

/**
 * @brief Return the input debounced @b logical state
 *
 * Debounced input logical state is evaluated using the initialization data.
 * @param[in] input_name input identification, defined by \ref digInputs
 * @return  DIO_OFF - if the logical state of the input is "0" (not active)
 * 			DIO_ON - if the logical state of the input is "1" (active)
 * 			ENODEV - if the /p input_name is out of range defined by \ref digInputs
 */
dioStates input_logical_state_debounced(digInputs input_name);

/**
 * @brief Handles digital input logic. Call every 1ms.
 * @note can be called faster or slower but the initialization values are no longer in ms.
 * @return  void
 */
void input_handle(void);

/**
 * @brief Get HW input state. In case you want to implement it differently (f.e. not using HAL)
 * @param[in] input_name input identification, defined by \ref digInputs
 * @return @c GPIO_PinState (from the HAL library)
 */
__weak GPIO_PinState input_get_hw_HAL(digInputs input_name);

#endif /* INC_DI_DI_H_ */
