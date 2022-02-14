/**
 * @file swo.c
 * @author panvicka
 * @date 2.1.2022
 *
 */

#include <stdio.h>
#include <string.h>
#include <sys/_stdint.h>
#include <lib/uti/swo.h>

swoType swo_type = SWO_NOT_ACTIVE;

retStatus swo_init(swoType type) {

	if (type >= SWO_NONE)
		return ENODEV;

	swo_type = type;

	return EOK;

}

void swo_print(char text[]) {

	if (swo_type == SWO_NOT_ACTIVE) {
		return;
	} else if (swo_type == SWO_PLAIN_MESSAGES) {
		// add EOL automatically, otherwise the output in console is not shown correctly
		while (*text) {
			ITM_SendChar(*text++);
		}
		ITM_SendChar('\n');

	} else if (swo_type == SWO_RTC_SUPPORT) {
		// implement yourself if you have an on-board RTC time
	}

}
