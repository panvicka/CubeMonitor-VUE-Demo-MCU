/*
 * run.c
 *
 *  Created on: Jan 8, 2022
 *      Author: panvicka
 */

#include <lib/ai.h>
#include <lib/ao.h>
#include <lib/di.h>
#include <lib/do.h>
#include <lib/uti/utility.h>
#include <prog/init.h>
#include <prog/back_door.h>
#include <prog/run.h>
#include <stm32f4xx_hal_gpio.h>
#include <sys/_stdint.h>
#include <main.h>

typedef struct programData {

	uint32_t mx_counter;
	uint8_t mx_counter_rewrites;
	uint16_t mx_counter_value;

	uint32_t mx_operation_time_in_seconds;
	uint8_t increase_counter_every_1s;

} programData;

static programData data;

void static prog_handle_counter(void);
void static prog_handle_output_setting(void);
void static prog_handle_adc_dac(void);

void prog_SM_tasks(void) {
	prog_handle_counter();
	prog_handle_output_setting();
	prog_handle_adc_dac();

#ifdef ALLOW_CUBEMX_BACKDOOR
	back_door_handle();
#endif
}

void prog_1s_tasks(void) {
	data.mx_operation_time_in_seconds++;

	output_toggle(DO_LED_ORANGE);
	output_toggle(DO_LED_RED);

	if (data.increase_counter_every_1s == 1) {
		data.mx_counter++;
	}

}

void prog_reset_all(void) {
	data.mx_counter = 0;
	data.increase_counter_every_1s = 0;
	data.mx_operation_time_in_seconds = 0;

}

void prog_1ms_tasks(void) {
	input_handle();
}


void static prog_handle_output_setting(void) {
	if (data.mx_counter % 2) {
		output_set(DO_LED_BLUE, GPIO_PIN_SET);
	} else {
		output_set(DO_LED_BLUE, GPIO_PIN_RESET);
	}
}

void static prog_handle_adc_dac(void) {
	int32_t voltage = 0;
	analog_input_get(AI_1, ANA_DIO_VOLTAGE, &voltage);
	int32_t to_set = 330 - voltage;
	to_set = (to_set < 0) ? 0 : to_set;
	analog_output_set(AO_1, ANA_DIO_VOLTAGE, to_set);
}

void static prog_handle_counter(void) {

	if (input_get_action(DI_BUTTON, INPUT_ACT_SHORT_PRESS)) {
		data.mx_counter++;
		input_ack_action(DI_BUTTON, INPUT_ACT_SHORT_PRESS);
	}

	if (input_get_action(DI_BUTTON, INPUT_ACT_DOUBLE_PRESS)) {
		data.mx_counter = 0;
		input_ack_action(DI_BUTTON, INPUT_ACT_DOUBLE_PRESS);
	}

	if (input_get_action(DI_BUTTON, INPUT_ACT_LONG_PRESS)) {
		data.increase_counter_every_1s = 1;
	} else {
		data.increase_counter_every_1s = 0;
	}
}

