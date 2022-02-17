/**
 * @file init.c
 * @author panvicka
 * @date 2.1.2022
 * @brief Initialization of digital/analog inputs and outputs.
 */

#include <lib/di.h>
#include <lib/do.h>
#include <lib/ai.h>
#include <lib/ao.h>
#include <lib/uti/swo.h>
#include <main.h>
#include <prog/init.h>

const uint16_t DI_DEBOUNC_MS = 50;
const uint16_t DI_DOUBLE_PRESS_SPACING = 300;
const uint16_t DI_LONG_PRESS_MIN_DURATION = 500;

const uint16_t AI_SAMPLING_AI1 = 600;
const uint16_t AI_SAMPLING_AI2 = 2000;

ADC_HandleTypeDef hadc1;

retStatus static init_di(void);
retStatus static init_do(void);
retStatus static init_ai(void);
retStatus static init_ao(void);

void init(void) {

	swo_init(SWO_PLAIN_MESSAGES);

	retStatus status = EOK;
	status += init_di();
	status += init_do();
	status += init_ai();
	status += init_ao();

	// in case any of the initialization fails, the program will stop
	// check SWO messages for the cause
	if (status != EOK) {
		initialization_error_handle();
	}

}

retStatus static init_di(void) {
	retStatus status = EOK;

	// initialization values that are valid for all inputs
	digitalInputInitData digital_input_init;
	digital_input_init.debounc_time = DI_DEBOUNC_MS;
	digital_input_init.double_press_spacing = DI_DOUBLE_PRESS_SPACING;
	digital_input_init.long_press_ms = DI_LONG_PRESS_MIN_DURATION;
	digital_input_init.input_get_hw_state = NULL;

	/*
	 * blue toggle switch is initialized as INPUT_HW_ACTIVE_HIGH (based on the connection on the
	 * evaluation board and and as a INPUT_SW_BUTTON which means we get access to the following actions:
	 * INPUT_ACT_FALLING_EDGE
	 * INPUT_ACT_RISING_EDGE
	 * INPUT_ACT_SHORT_PRESS
	 * INPUT_ACT_LONG_PRESS
	 * INPUT_ACT_DOUBLE_PRESS
	 *
	 * This SW type is meant to be used with user navigation buttons, toggle switches etc.
	 */
	digital_input_init.mcu_pin = BUTTON_Pin;
	digital_input_init.mcu_port = BUTTON_GPIO_Port;
	digital_input_init.sw_type = INPUT_SW_BUTTON;
	digital_input_init.hw_type = INPUT_HW_ACTIVE_HIGH;
	status += input_init(DI_BUTTON, digital_input_init);

	/*
	 * another input named DI_1 is also initialized as INPUT_HW_ACTIVE_HIGH. There is no connection to
	 * this pin on board so if we want to active this input, we need to connect it to 3.3V. The
	 * INPUT_SW_ON_OFF_INPUT SW type will allow access to following actions:
	 *
	 * INPUT_ACT_FALLING_EDGE
	 * INPUT_ACT_RISING_EDGE
	 *
	 * This SW type is meant to be used to on/off switches, sensors, etc.
	 */
	digital_input_init.mcu_pin = DI_1_Pin;
	digital_input_init.mcu_port = DI_1_GPIO_Port;
	digital_input_init.sw_type = INPUT_SW_ON_OFF_INPUT;
	digital_input_init.hw_type = INPUT_HW_ACTIVE_HIGH;
	status += input_init(DI_1, digital_input_init);

	/*
	 * this input is similar to DI_1 but it is initialized as INPUT_HW_ACTIVE_LOW, which mean we
	 * need to connect this input to GND to activate it. This type of input is usually preferred
	 * to reduce errors caused due to interference.
	 */
	digital_input_init.mcu_pin = DI_2_Pin;
	digital_input_init.mcu_port = DI_2_GPIO_Port;
	digital_input_init.sw_type = INPUT_SW_ON_OFF_INPUT;
	digital_input_init.hw_type = INPUT_HW_ACTIVE_LOW;
	status += input_init(DI_2, digital_input_init);

	return status;
}

retStatus static init_do(void) {
	retStatus status = EOK;

	digitalOutputInitData digital_output_init;
	digital_output_init.init_state = GPIO_PIN_RESET;

	digital_output_init.pin = DIODE_DO_ORANGE_Pin;
	digital_output_init.port = DIODE_DO_ORANGE_GPIO_Port;
	status += output_init(DO_LED_ORANGE, digital_output_init);

	digital_output_init.pin = DIODE_DO_RED_Pin;
	digital_output_init.port = DIODE_DO_RED_GPIO_Port;
	status += output_init(DO_LED_RED, digital_output_init);

	digital_output_init.pin = DIODE_DO_BLUE_Pin;
	digital_output_init.port = DIODE_DO_BLUE_GPIO_Port;
	status += output_init(DO_LED_BLUE, digital_output_init);

	return status;

}

uint32_t static lin_adc_5V_scaling_and_linearization(uint32_t adc) {
	return adc * 500 / 4094;
}

retStatus static init_ai(void) {
	retStatus status = EOK;

	/*
	 * This analog input is connected to 3.3V with potentiometer in the way.
	 * We considered everything linear and thus can use the prepared lin_adc_no_scaling_no_corrections
	 * function
	 */
	status += analog_input_init(AI_1, AI_SAMPLING_AI1,
			lin_adc_no_scaling_no_corrections);

	/*
	 * This analog input is connected to 5V with potentiometer and 1:2 resistor divider
	 * After measuring several points a scaling/linearization function was created and passed
	 * to the init function
	 */
	status += analog_input_init(AI_2, AI_SAMPLING_AI2,
			lin_adc_5V_scaling_and_linearization);

	// Starts the conversion in DMA interrupt mode
	status += analog_input_start(hadc1);

	return status;

}

retStatus static init_ao(void) {
	retStatus status = EOK;

	status += analog_output_init(AO_1, DAC_CHANNEL_1,
			lin_dac_no_scaling_no_corrections);
	status += analog_output_init(AO_2, DAC_CHANNEL_2,
			lin_dac_no_scaling_no_corrections);

	return status;

}

