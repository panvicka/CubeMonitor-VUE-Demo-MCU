/*
 * blink.h
 *
 *  Created on: Jan 8, 2022
 *      Author: panvicka
 */

#ifndef INC_LIB_BLINK_H_
#define INC_LIB_BLINK_H_

#include <lib/uti/hw_hal_includes.h>

typedef enum {
	PWM_BLINK_FADING, PWM_BLINK_ON_OFF,
} pwmBlinkingType;

typedef struct {
	uint16_t top_set_delay;
	uint16_t bottom_set_delay;
	uint16_t delaying_couter;
	pwmBlinkingType type;

} pwmBlinkingTimingInit;

#endif /* INC_LIB_BLINK_H_ */
