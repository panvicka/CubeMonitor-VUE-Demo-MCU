/*
 * blink.c
 *
 *  Created on: Jan 8, 2022
 *      Author: panvicka
 */

typedef struct {

	pwmBlinkingTimingInit timing_init;
	uint32_t blinking_counter;
	uint32_t blink_delay_counter;

	TIM_HandleTypeDef tim_handle;
	uint32_t tim_channel;
	uint16_t max_value;

} pwmBlinking;

