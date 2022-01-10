/*
 * ai.c
 *
 *  Created on: Jan 8, 2022
 *      Author: panvicka
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
