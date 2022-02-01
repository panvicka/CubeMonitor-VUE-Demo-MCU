/*
 * temp.c
 *
 *  Created on: Feb 1, 2022
 *      Author: panvicka
 */

#include <lib/ai.h>
#include <lib/uti/utility.h>
#include <prog/init.h>
#include <sys/_stdint.h>
#include <prog/temp.h>

float temp = 0;

void calc_temp(void) {

	int32_t voltage = 0;
	analog_input_get(AI_TEMP, ANA_DIO_VOLTAGE, &voltage);

	temp = voltage;

	temp /= (float) 100.0; //Reading in Volts
	temp -= (float) 0.76;
	temp /= (float) 0.0025;
	temp += (float) 25.0;
	temp -= (float) 11.0;

}
