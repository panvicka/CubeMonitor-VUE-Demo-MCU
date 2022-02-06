/*
 * back_door.c
 *
 *  Created on: Feb 6, 2022
 *      Author: panvicka
 */

#include <prog/back_door.h>
#include <prog/run.h>

typedef enum backDoorCodes {
	BCK_DOOR_NONE = 0,
	BCK_DOOR_WHILE_LOOP = 100,
	BCK_DOOR_NVIC_RESET = 200,
	BCK_DOOR_RESET_ALL = 300,
} backDoorCodes;

backDoorCodes mx_back_door;

void back_door_handle(void) {

	if (mx_back_door != BCK_DOOR_NONE) {

		switch (mx_back_door) {

		case BCK_DOOR_WHILE_LOOP:
			while (1)
				;
			break;
		case BCK_DOOR_NVIC_RESET:
			HAL_NVIC_SystemReset();
			break;
		case BCK_DOOR_RESET_ALL:
			prog_reset_all();
			break;
		default:
			break;
		}
		mx_back_door = BCK_DOOR_NONE;
	}
}
