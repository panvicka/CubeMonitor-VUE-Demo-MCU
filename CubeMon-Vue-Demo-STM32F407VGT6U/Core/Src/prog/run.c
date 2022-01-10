/*
 * run.c
 *
 *  Created on: Jan 8, 2022
 *      Author: panvicka
 */

static uint32_t mx_operation_time_in_seconds = 0;
static uint32_t mx_button_long_presses = 0;
static uint32_t mx_button_short_presses = 0;

void prog_SM_tasks(void) {

}

void prog_1s_tasks(void) {
	mx_operation_time_in_seconds++;

}

void prog_1ms_tasks(void) {

	input_handle();

}


void prog_fast_tasks(void) {

	();

}
