/**
 * @file run.h
 * @author panvicka
 * @date 2.1.2022
 * @brief Programm logic.
 */

#ifndef INC_PROG_RUN_H_
#define INC_PROG_RUN_H_

#include <lib/uti/hw_hal_includes.h>

void prog_SM_tasks(void);
void prog_1s_tasks(void);
void prog_1ms_tasks(void);

void prog_reset_all(void);

#endif /* INC_PROG_RUN_H_ */
