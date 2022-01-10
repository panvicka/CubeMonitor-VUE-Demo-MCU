/*
 * swo.h
 *
 *  Created on: Jan 9, 2022
 *      Author: panvicka
 */

#ifndef INC_LIB_SWO_H_
#define INC_LIB_SWO_H_

#include <lib/uti/hw_hal_includes.h>
#include <lib/uti/utility.h>

typedef enum {
	SWO_NOT_ACTIVE,
	SWO_PLAIN_MESSAGES,
	SWO_RTC_SUPPORT,
	SWO_RUN_TIME_INFO,
	SWO_NONE,
} swo_init_type;

retStatus swo_init(swo_init_type type);
void swo_print(char text[]);

#endif /* INC_LIB_SWO_H_ */
