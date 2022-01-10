/**
 * @file callbacks.c
 * @author panvicka
 * @date 2.1.2022
 * @brief HAL library callbacks
 */

#include <prog/callbacks.h>
#include <prog/run.h>
#include <lib/ao.h>
#include <lib/ai.h>

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
	analog_input_handle();
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {

	if (htim->Instance == TIM10) {
		prog_1ms_tasks();
	} else if (htim->Instance == TIM14) {
		prog_SM_tasks();
	} else if (htim->Instance == TIM13) {
		prog_1s_tasks();
	}

}
