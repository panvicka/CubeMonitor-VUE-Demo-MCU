/*
 * init.h
 *
 *  Created on: Jan 8, 2022
 *      Author: panvicka
 */

#ifndef INC_PROG_INIT_H_
#define INC_PROG_INIT_H_

void init(void);

#define LIB_DI
#define LIB_DO
#define LIB_AI
#define LIB_AO

typedef enum {
	DI_BUTTON, DI_1, DI_2, DI_NONE,
} digInputs;

typedef enum {
	DO_LED_ORANGE, DO_LED_RED, DO_LED_BLUE, DO_NONE
} digOutputs;

typedef enum {
	AI_1, AI_2, AI_NONE
} anaInputs;

typedef enum {
	AO_1, AO_2, AO_NONE,
} anaOutputs;

#endif /* INC_PROG_INIT_H_ */
