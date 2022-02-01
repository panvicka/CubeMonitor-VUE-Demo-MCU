/*
 * run.c
 *
 *  Created on: Jan 8, 2022
 *      Author: panvicka
 */

#include <lib/di.h>
#include <prog/run.h>
#include <sys/_stdint.h>

typedef struct programData {

	uint32_t mx_counter;
	uint32_t mx_operation_time_in_seconds;

	uint8_t increase_counter_every_1s;

} programData;

static programData data;

void static prog_counter_handling(void);

void prog_SM_tasks(void) {
	prog_counter_handling();
	calc_temp();
}

void prog_1s_tasks(void) {
	data.mx_operation_time_in_seconds++;

	if (data.increase_counter_every_1s == 1) {
		data.mx_counter++;
	}

}

void prog_1ms_tasks(void) {

	input_handle();

}

void prog_fast_tasks(void) {

}

void static prog_counter_handling(void) {

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
