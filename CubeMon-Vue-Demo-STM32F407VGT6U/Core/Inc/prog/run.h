/*
 * run.h
 *
 *  Created on: Jan 8, 2022
 *      Author: panvicka
 */

#ifndef INC_PROG_RUN_H_
#define INC_PROG_RUN_H_

#include <lib/uti/hw_hal_includes.h>

void prog_SM_tasks(void);
void prog_1s_tasks(void);
void prog_1ms_tasks(void);

void prog_reset_all(void);

#endif /* INC_PROG_RUN_H_ */
