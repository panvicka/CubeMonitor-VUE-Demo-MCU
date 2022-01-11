/*
 * callbacks.c
 *
 *  Created on: Jan 8, 2022
 *      Author: panvicka
 */

#include <prog/callbacks.h>
#include <prog/run.h>
#include <lib/ao.h>
#include <lib/ai.h>

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
	analog_input_handle();
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {

	if (htim->Instance == TIM4) {
		prog_1ms_tasks();
	} else if (htim->Instance == TIM6) {
		prog_SM_tasks();
	} else if (htim->Instance == TIM5) {
		prog_1s_tasks();
	}

}
