/*
 * init.c
 *
 *  Created on: Jan 8, 2022
 *      Author: panvicka
 */

#include <lib/di.h>
#include <lib/uti/swo.h>
#include <main.h>
#include <prog/init.h>

void init(void) {

	input_init(BUTTON, BUTTON_Pin, BUTTON_GPIO_Port, 50);
	input_init(DI_1, DI_1_Pin, DI_1_GPIO_Port, 50);
	input_init(DI_2, DI_2_Pin, DI_2_GPIO_Port, 50);

	swo_init(SWO_PLAIN_MESSAGES);

	swo_print("hello world");
}
