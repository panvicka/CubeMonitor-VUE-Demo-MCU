/*
 * swo.c
 *
 *  Created on: Jan 9, 2022
 *      Author: panvicka
 */

#include <stdio.h>
#include <string.h>
#include <sys/_stdint.h>
#include <lib/uti/swo.h>
#include <lib/uti/run_time.h>

swo_init_type swo_type = SWO_NOT_ACTIVE;

retStatus swo_init(swo_init_type type) {

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
//		strcat(text, '\n');
	} else if (swo_type == SWO_RUN_TIME_INFO) {
//		int32_t seconds_since_start = 0;
//		run_time_get(RUN_TIME_SINCE_START, RUN_TIME_SECONDS,
//				&seconds_since_start);
//		strcat("%lu", text);
	} else if (swo_type == SWO_RTC_SUPPORT) {
		// implement yourself if you have an on-board RTC time
	}

//	printf(text);

}
