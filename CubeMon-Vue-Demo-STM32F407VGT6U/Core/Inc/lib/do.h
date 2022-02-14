/**
 * @file do.h
 * @author panvicka
 * @date 8.1.2022
 * @brief digital outputs with set/get and toggle functions
 *
 * To use this library define flag LIB_DO in the lib_config.h file and create an enum in init.h file
 * with the outputs you would like to use.
 * You then have to initialize each input with \ref input_init function.
 * @code
 *	typedef enum digOutputs {
 *	MY_OUTPUT_1,
 *	MY_OUTPUT_2,
 *	MY_OUTPUT_3,
 *	DO_NONE
 *	} digOutputs;
 * @endcode
 * @note Do not forget to include the DO_NONE at the end of the enum listing.
 */

#ifndef INC_DO_DO_H_
#define INC_DO_DO_H_

#include <lib/uti/hw_hal_includes.h>
#include <lib/uti/lib_config.h>
#include <lib/uti/utility.h>

#ifdef LIB_DO
#include <prog/init.h>
#else
/**
 * If the library is not used (@b LIB_DO is not defined as symbol) this dummy enum will be created
 * to prevent compilation errors
 */
typedef enum digOutputs {
	DO_NONE,
}digOutputs;
#endif

typedef struct digitalOutputInitData {
	uint16_t pin; ///< MCU GPIO pin number
	GPIO_TypeDef *port; ///< MCU GPIO port
	GPIO_PinState init_state; //<
} digitalOutputInitData;

/**
 * @brief Digital output initialization function
 * @param[in] output output identification, defined by \ref digOutputs
 * @param[in] digital_output_init_data output initialization data \ref digitalOutputInitData
 * @return  \ref EOK is the initialization completed OK
 * 			\ref ENODEV if the /p output is out of range defined by \ref digOutputs
 * 			or there are invalid values in the initialization struct
 */
retStatus output_init(digOutputs output,
		digitalOutputInitData digital_output_init_data);

/**
 * @brief Toggles the output state
 * @param[in] output output identification, defined by \ref digOutputs
 * @return  \ref EOK action completed
 * 			\ref ENODEV if the /p output is out of range defined by \ref digOutputs or is not initialized
 */
retStatus output_toggle(digOutputs output);

/**
 * @brief Sets output state.
 * @param[in] output_name output identification, defined by \ref digOutputs
 * @param[in] state state to be set
 *
 * @return  \ref EOK action completed
 * 			\ref ENODEV if the /p output is out of range defined by \ref digOutputs or is not initialized
 */
retStatus output_set(digOutputs output_name, GPIO_PinState state);

/**
 * @brief Get the current output state.
 * @param[in] output output identification, defined by \ref digOutputs
 * @return  \ref DIO_NOT_INITIALIZED ENODEV if the /p output is out of range defined by \ref digOutputs or is not initialized
 * 			\ref DIO_OFF if output is in off state
 * 			\ref DIO_ON if output is in on state
 */
dioStates output_get_state(digOutputs output);

/**
 * @brief Sets HW output state. In case you want to implement it differently (f.e. not using HAL)
 * @param[in] output_name output identification, defined by \ref digOutputs
 * @param[in] state state to be set
 * @return  \ref ENODEV if if the /p output is out of range defined by \ref digOutputs or is not initialized
 * 			\ref EOK if the output was set OK
 */
__weak retStatus output_set_hw_HAL(digOutputs output_name, GPIO_PinState state);

#endif /* INC_DO_DO_H_ */
