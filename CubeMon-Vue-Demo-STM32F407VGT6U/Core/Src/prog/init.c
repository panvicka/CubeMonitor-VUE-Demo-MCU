/*
 * init.c
 *
 *  Created on: Jan 8, 2022
 *      Author: panvicka
 */

#include <lib/di.h>
#include <lib/do.h>
#include <lib/ai.h>
#include <lib/ao.h>
#include <lib/uti/swo.h>
#include <main.h>
#include <prog/init.h>

const uint16_t DI_DEBOUNC_MS = 50;
const uint16_t DI_DOUBLE_PRESS_SPACING = 300;
const uint16_t DI_LONG_PRESS_MIN_DURATION = 500;

const uint16_t AI_SAMPLING = 600;

ADC_HandleTypeDef hadc1;

void static _init_di(void);
void static _init_do(void);
void static _init_ai(void);
void static _init_ao(void);

void init(void) {

	swo_init(SWO_PLAIN_MESSAGES);

	_init_di();
	_init_do();
	_init_ai();
	_init_ao();

	swo_print("hello world");
}

void static _init_di(void) {
	uint16_t status = EOK;

	digitalInputInitData digital_input_init;
	digital_input_init.debounc_time = DI_DEBOUNC_MS;
	digital_input_init.double_press_spacing = DI_DOUBLE_PRESS_SPACING;
	digital_input_init.long_press_ms = DI_LONG_PRESS_MIN_DURATION;
	digital_input_init.input_get_hw_state = NULL;

	digital_input_init.mcu_pin = BUTTON_Pin;
	digital_input_init.mcu_port = BUTTON_GPIO_Port;
	digital_input_init.sw_type = INPUT_SW_BUTTON;
	digital_input_init.hw_type = INPUT_HW_ACTIVE_HIGH;
	status += input_init(DI_BUTTON, digital_input_init);

	digital_input_init.mcu_pin = DI_1_Pin;
	digital_input_init.mcu_port = DI_1_GPIO_Port;
	digital_input_init.sw_type = INPUT_SW_ON_OFF_INPUT;
	digital_input_init.hw_type = INPUT_HW_ACTIVE_LOW;
	status += input_init(DI_1, digital_input_init);

	digital_input_init.mcu_pin = DI_2_Pin;
	digital_input_init.mcu_port = DI_2_GPIO_Port;
	digital_input_init.sw_type = INPUT_SW_ON_OFF_INPUT;
	digital_input_init.hw_type = INPUT_HW_ACTIVE_LOW;
	status += input_init(DI_2, digital_input_init);

	if (status != EOK) {
		initialization_error_handle();
	}
}

void static _init_do(void) {
	uint16_t status = EOK;

	digitalOutputInitData digital_output_init;
	digital_output_init.init_state = GPIO_PIN_RESET;

	digital_output_init.pin = DIODE_DO_ORANGE_Pin;
	digital_output_init.port = DIODE_DO_ORANGE_GPIO_Port;
	status += output_init(DO_LED_ORANGE, digital_output_init);

	digital_output_init.pin = DIODE_DO_RED_Pin;
	digital_output_init.port = DIODE_DO_RED_GPIO_Port;
	status += output_init(DO_LED_RED, digital_output_init);

	digital_output_init.pin = DIODE_DO_BLUE_Pin;
	digital_output_init.port = DIODE_DO_BLUE_GPIO_Port;
	status += output_init(DO_LED_BLUE, digital_output_init);

	if (status != EOK) {
		initialization_error_handle();
	}

}

void static _init_ai(void) {
	uint16_t status = EOK;
	status += analog_input_init(AI_1, AI_SAMPLING,
			lin_adc_no_scaling_no_corrections);
	status += analog_input_init(AI_2, AI_SAMPLING,
			lin_adc_no_scaling_no_corrections);

	status += HAL_ADC_Start_DMA(&hadc1, ADC_data_buffer, AI_NONE);

	if (status != EOK) {
		initialization_error_handle();
	}

}

void static _init_ao(void) {
	uint16_t status = EOK;
	status += analog_output_init(AO_1, DAC_CHANNEL_1,
			lin_dac_no_scaling_no_corrections);
	status += analog_output_init(AO_2, DAC_CHANNEL_2,
			lin_dac_no_scaling_no_corrections);
	if (status != EOK) {
		initialization_error_handle();
	}
}
