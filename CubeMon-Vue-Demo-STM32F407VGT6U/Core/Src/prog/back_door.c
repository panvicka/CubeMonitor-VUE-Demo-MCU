/**
 * @file back_door.c
 * @author panvicka
 * @date 2.1.2022
 * @brief Secret backdoor to your project allowing direct system manipulation (close it before building for production!)
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
			// checks if the watchdog is a good boy
			while (1)
				;
			break;
		case BCK_DOOR_NVIC_RESET:
			// testing how good can the system recover from system reset
			HAL_NVIC_SystemReset();
			break;
		case BCK_DOOR_RESET_ALL:
			// sets all varibles to zero
			prog_reset_all();
			break;
		default:
			break;
		}
		mx_back_door = BCK_DOOR_NONE;
	}

}
