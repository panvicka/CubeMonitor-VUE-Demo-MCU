/*
 * utility.c
 *
 *  Created on: Jan 8, 2022
 *      Author: panvicka
 */

void initialization_error_handle(void) {
	swo_print(
			"utility: unexpected error during initialization, program stopped");
	while (1) {
	}
}
