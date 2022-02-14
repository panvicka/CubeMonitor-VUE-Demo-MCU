/**
 * @file ai.h
 * @author panvicka
 * @date 8.1.2022
 * @brief analog inputs library for multi-channel DMA running in interrupt mode, averaging and custom linearization functions
 *
 * To use this library define flag LIB_AI in the lib_config.h file and create an enum in init.h file
 * with the outputs you would like to use.
 * You then have to initialize each input with \ref input_init function.
 * @code
 *	typedef enum anaInputs {
 *	MY_ANALOG_INPUT_1,
 *	MY_ANALOG_INPUT_2,
 *	MY_ANALOG_INPUT_3,
 *	AI_NONE
 *	} anaInputs;
 * @endcode
 * @note Do not forget to include the AI_NONE at the end of the enum listing.
 */
#ifndef INC_LIB_AI_H_
#define INC_LIB_AI_H_

#include <lib/uti/hw_hal_includes.h>
#include <lib/uti/utility.h>
#include <lib/uti/lib_config.h>

#ifdef LIB_AI
#include <prog/init.h>
#else
/**
 * If the library is not used (@b LIB_AI is not defined as symbol) this dummy enum will be created
 * to prevent compilation errors
 */
typedef enum anaInputs {
	AI_NONE
}anaInputs;
#endif

extern uint32_t ADC_data_buffer[(AI_NONE * 2) + 1];

/**
 * @brief Analog input initialization function
 * @param[in] an_input_name analog input identification, defined by \ref anaInputs
 * @param[in] sample_from how many samples should be averaged (0-0x7fff)
 * @param[in] linearization_function in case there is an voltage divider or any other input that needs to be converted
 * @note if a simple 0-3.3V input without non-linearities is expected, you can use lin_adc_no_scaling_no_corrections()
 * @return  \ref EOK is the initialization completed OK
 * 			\ref ENODEV if the /p an_input_name is out of range defined by \ref anaInputs
 * 			\ref EFAULT null pointer passed to /p linearization_function
 *
 */
retStatus analog_input_init(anaInputs an_input_name, int32_t sample_from,
		liner_fce linearization_function);

/**
 * @brief Standardized getter function for digital input library.
 * @param[in] analog_input analog input identification, defined by \ref anaInputs
 * @param[in] data data for /p analog_input that can be read out of the module, available data \ref aioData
 * @param[out] value getter output

 * @note /p input and /p data types are changed to @c uint32_t and @c uint16_t to keep the standard function prototype
 *
 * @return  \ref EOK is the /p value is valid
 * 			\ref ENODEV if the /p an_input_name is out of range defined by \ref anaInputs or not initialized
 * 			\ref EINVAL if invalid /p data is read
 */
retStatus analog_input_get(uint16_t analog_input, uint16_t data, int32_t *value);

/**
 * @brief Handler for analog inputs
 * @note call inside @c HAL_ADC_ConvCpltCallback() (from the HAL library)
 */
void analog_input_handle(void);

/**
 * @brief Convert ADC reading to voltage for a simple 0-3.3V input
 */
uint32_t lin_adc_no_scaling_no_corrections(uint32_t adc_value);

#endif /* INC_LIB_AI_H_ */
