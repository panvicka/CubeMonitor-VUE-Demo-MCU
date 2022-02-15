/**
 * @file hw_hal_includes.h
 * @author panvicka
 * @date 2.1.2022
 * @brief provides HAL includes for different MCU cores
 *
 */

#ifndef INC_LIB_UTI_HW_HAL_INCLUDES_H_
#define INC_LIB_UTI_HW_HAL_INCLUDES_H_

#define STM32F4

#if defined (STM32F4)
#include <stm32f4xx_hal.h>
#elif defined (STM32F1)
#include <stm32f1xx_hal.h>
#elif defined (STM32F0)
#include <stm32f0xx_hal.h>
#else
#error "Select one of the MCUs!"
#endif

#endif /* INC_LIB_UTI_HW_HAL_INCLUDES_H_ */
