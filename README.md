# CubeMonitor+Vue Demo (MCU program part)

This project is a part of a larger project *CubeMonitor+Vue Demo*. This part contains the code for MCU created with HAL libraries, in this for STM32F407 evaluation board (code is quite portable if you do not have exactly this board).

Check this [project github page](https://panvicka.github.io/CubeMonitor-VUE-Demo-MCU/) for more info and Doxygen build documentation.

Check the [UI Vue part](https://github.com/panvicka/CubeMonitor-VUE-Demo-CM) for integration with CubeMonitor 

## Setup to use with the VUE interface 
You will need: 
	- STM32F407 Discovery board or any other evaluation board 
	- STM IDE (turns out the Atollic and STM IDE are not really back-compatible, the configuration is broken...) 
	
Steps:
1) Fork and clone this repository 
2) Import the project into STM IDE 
3) (if you have the STM32F407 Discovery board) flash it with the `program_VX.X.hex` from the  *Release* folder 
3) (build yourself for a different MCU) The project setting should be imported as well but just to be sure this is the build command used: 
```
-mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb
```
5) Continue with the [VUE part setup](https://panvicka.github.io/CubeMonitor-VUE-Demo-CM/)
	

## Libraries usage
This project was created to demonstrate the possibilities of the CubeMonitor Vue UI, but it also contains libraries that can be used separately. 


### Digital input 
0) make sure to define your HW correctly, if you are not using STM HAL, redefine the weak function `input_get_hw_HAL`
1) define symbol `LIB_DI` 
2) create an enum listing all inputs you would like to use, do not forget to include one extra named `DI_NONE`
```c
typedef enum digInputs {
	YOUR_INPUT_1,
	YOUR_INPUT_2,
	DI_NONE,
}digInputs;
```
3) initialize your inputs using `input_init` and `digitalInputInitData` structure. Check the [documentation](https://panvicka.github.io/CubeMonitor-VUE-Demo-MCU/di_8h.html) for more info on the types. 
```c
digitalInputInitData digital_input_init;
digital_input_init.debounc_time = DI_DEBOUNC_MS;
digital_input_init.double_press_spacing = DI_DOUBLE_PRESS_SPACING;
digital_input_init.long_press_ms = DI_LONG_PRESS_MIN_DURATION;
digital_input_init.input_get_hw_state = NULL;
digital_input_init.mcu_pin = BUTTON_Pin;
digital_input_init.mcu_port = BUTTON_GPIO_Port;
digital_input_init.sw_type = INPUT_SW_BUTTON;
digital_input_init.hw_type = INPUT_HW_ACTIVE_HIGH;
input_init(DI_BUTTON, digital_input_init);

```
4) call the `input_handle` every 1ms 
5) then you can use `input_get_action` to fetch input actions:
```c
INPUT_ACT_FALLING_EDGE
INPUT_ACT_RISING_EDGE
INPUT_ACT_SHORT_PRESS
INPUT_ACT_LONG_PRESS
INPUT_ACT_DOUBLE_PRESS
```

### Analog input 
0) make sure to define your HW correctly
1) define symbol `LIB_AI` 
2) create an enum listing all analog inputs you would like to use, do not forget to include one extra named `AI_NONE`
```c
typedef enum anaInputs {
	YOUR_ANALOG_INPUT_1,
	YOUR_ANALOG_INPUT_2,
	AI_NONE
}anaInputs;
```
3) initialize your inputs using `analog_input_init` Check the [documentation](https://panvicka.github.io/CubeMonitor-VUE-Demo-MCU/ai_8h.html) for more info on the types. 
```
analog_input_init(AI_1, AI_SAMPLING_AI1,
			lin_adc_no_scaling_no_corrections);
```
4) call `analog_input_start`
5) place `analog_input_handle` into `HAL_ADC_ConvCpltCallback` 
6) fetch current value, averaged current value or directly scaled voltage with `analog_input_get` 
