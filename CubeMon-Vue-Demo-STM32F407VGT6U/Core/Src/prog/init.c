/*
 * init.c
 *
 *  Created on: Jan 8, 2022
 *      Author: panvicka
 */

#include <lib/di.h>
#include <lib/do.h>
#include <lib/uti/swo.h>
#include <main.h>
#include <prog/init.h>

const uint16_t DI_DEBOUNC_MS = 50;
const uint16_t AI_SAMPLING = 100;

ADC_HandleTypeDef hadc1;

void static _init_di(void);
void static _init_do(void);
void static _init_ai(void);

void init(void) {

	_init_di();
	_init_do();
	_init_ai();

	swo_init(SWO_PLAIN_MESSAGES);

	swo_print("hello world");
}

void static _init_di(void) {
	uint16_t status = EOK;

	status += input_init(DI_BUTTON, BUTTON_Pin, BUTTON_GPIO_Port,
			DI_DEBOUNC_MS);
	status += input_init(DI_1, DI_1_Pin, DI_1_GPIO_Port, DI_DEBOUNC_MS);
	status += input_init(DI_2, DI_2_Pin, DI_2_GPIO_Port, DI_DEBOUNC_MS);

	if (status != EOK) {
		initialization_error_handle();
	}
}

void static _init_do(void) {
	uint16_t status = EOK;
	status += output_init(DO_LED_ORANGE, DIODE_DO_ORANGE_Pin,
	DIODE_DO_ORANGE_GPIO_Port, DIO_OFF);
	status += output_init(DO_LED_RED, DIODE_DO_RED_Pin,
	DIODE_DO_RED_GPIO_Port, DIO_OFF);
	status += output_init(DO_LED_BLUE, DIODE_DO_BLUE_Pin,
	DIODE_DO_BLUE_GPIO_Port, DIO_OFF);

	if (status != EOK) {
		initialization_error_handle();
	}

}

void static _init_ai(void) {
	uint16_t status = EOK;
	status += analog_input_init(AI_1, AI_SAMPLING,
			linearization_no_scaling_no_corrections);
	status += analog_input_init(AI_2, AI_SAMPLING,
			linearization_no_scaling_no_corrections);
	status += analog_input_init(AI_TEMP, AI_SAMPLING,
			linearization_no_scaling_no_corrections);

	status += HAL_ADC_Start_DMA(&hadc1, ADC_data_buffer, AI_NONE);

	if (status != EOK) {
		initialization_error_handle();
	}

}
