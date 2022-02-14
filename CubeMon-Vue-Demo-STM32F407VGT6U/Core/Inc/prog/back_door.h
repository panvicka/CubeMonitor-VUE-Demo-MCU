/**
 * @file back_door.h
 * @author panvicka
 * @date 2.1.2022
 * @brief Secret backdoor to your project allowing direct system manipulation (close it before building for production!)
 */

#ifndef INC_PROG_BACK_DOOR_H_
#define INC_PROG_BACK_DOOR_H_

#include <lib/uti/hw_hal_includes.h>

void back_door_handle(void);

#endif /* INC_PROG_BACK_DOOR_H_ */
