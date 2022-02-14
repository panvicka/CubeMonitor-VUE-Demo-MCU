/**
 * @file hw_hal_includes.h
 * @author panvicka
 * @date 2.1.2022
 * @brief provides HAL includes for different MCU cores
 *
 */


#ifndef INC_LIB_UTI_HW_HAL_INCLUDES_H_
#define INC_LIB_UTI_HW_HAL_INCLUDES_H_

#define STM32F407

#ifdef STM32F407
#include <stm32f4xx_hal.h>
#else
#error "Select one of the MCUs!"
#endif

#endif /* INC_LIB_UTI_HW_HAL_INCLUDES_H_ */
