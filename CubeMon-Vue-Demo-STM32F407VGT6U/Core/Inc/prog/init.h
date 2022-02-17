/**
 * @file init.h
 * @author panvicka
 * @date 2.1.2022
 * @brief Initialization of digital/analog inputs and outputs.
 */

#ifndef INC_PROG_INIT_H_
#define INC_PROG_INIT_H_

void init(void);

// activation of the selected libraries
#define LIB_DI
#define LIB_DO
#define LIB_AI
#define LIB_AO

typedef enum {
	DI_BUTTON, ///< User button
	DI_1, ///< DI_1 (PE7, active high)
	DI_2, ///< DI_2 (PE8, active low)
	DI_NONE, ///< DUMMY to keep the track of enum length
} digInputs;

typedef enum {
	DO_LED_ORANGE, ///< Orange LED (PD13)
	DO_LED_RED, ///< Red LED (PD14)
	DO_LED_BLUE, ///< Blue LED (PD15)
	DO_NONE ///< DUMMY to keep the track of enum length
} digOutputs;

typedef enum {
	AI_1, ///< AI_1 (PA1)
	AI_2, ///< AI_2 (PA2)
	AI_NONE
} anaInputs;

typedef enum {
	AO_1, ///< AO_1 (PA4)
	AO_2, ///< AO_2 (PA5)
	AO_NONE,
} anaOutputs;

#endif /* INC_PROG_INIT_H_ */
