CubeMonitor+Vue Demo (MCU program) {#mainpage}
============

Demonstration video: 
 
This project is a part of a larger project *CubeMonitor+Vue Demo*. This part contains the code for MCU created with HAL libraries, in this for STM32F407 evaluation board. In case you do not have STM32F407 evaluation board, you can port the program to any STM32 MCU as the Cube Monitor only cares about variable names/addresses. 

## Motivation
The CubeMonitor tool enables program runtime monitoring using NodeRed. With the NodeRed 'function-block-like' programming and preconfigured elements (like LEDs for input status or dial for analog input and even a *write* table for variable setting) you can create simple dashboards. As long as you want only few inputs and you do not really care about setting a lot of variables manually, this is completely fine.  

I did not really like the dashboard (it got pretty full and laggy after a while) and the way how it is created (I guess a lot of C/C++ programmers somehow dislike visual programing and wiring function blocks with mouse). I was also missing a simple way how to set variables using graphical elements like toggle switching. 

That is why I decided to use the NodeRed ui-builder plugin to create a custom UI with all the inputs/outputs/information I want. 

## Setup
You will need: 
	- STM32F407 Discovery board or any other evaluation board 
	- STM IDE (turns out the Atollic and STM IDE are not really back-compatible, the program configuration is deleted after you import a program created with CubeIDE into Atollic) 
	
Steps (option 1 - importing the project):
	1) Fork and clone this repository 
	2) Import the project into STM IDE 
	3) Build and flash the board. The project setting should be imported as well but just to be sure those are the `gcc` options I have used

		```
		-mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb
		``` 

Steps (option 2 - just flashing the board):
	1) Fork and clone this repository
	2) Use the Release.hex file in the /Release folder to flash the board (using for example ST-link)
	
## Structure
There are two main parts of the program
	- lib 
		- the libraries **do not** modify HW setting of the peripherials. This is handled completely by the HAL code generation. They do the heavy lifting for falling/raising edge detections, short/long button presses or analog input scaling. 
	- prog 
		- uses the libraries from *lib* to initialize used peripherials in init.c file 
		- handles the program logic created to demonstrate the CubeMonitor+Vue Demo functionality 

## Function
A seamingly nonsence logic is defined, but bear with me, it demonstrates the CubeMonitors functions quite well. 

The STM32F407 Discovery board user button actions changes a counter variable \ref programData.mx_counter:
- a single click (\ref INPUT_ACT_SHORT_PRESS) will increase counter variable by one 
- when pressed down (\ref INPUT_ACT_LONG_PRESS), the counter increases by 1 every second 
- on double click (\ref INPUT_ACT_DOUBLE_PRESS), it resets to 0 

Two digital inputs are defined:
- \ref DI_1 (PE7, active high) 
- \ref DI_2 (PE8, active low)

If both of them are in ON state, \ref DO_LED_RED is on.

Two analog inputs are defined 
- \ref AI_1 (PA1) is reading a potentiometer output with range 0-3.3V
- \ref AI_2 (PA2) is reading a potentiometer output with range 0-5V

Two analog outputs are defined 
- \ref AO_1 (PA4) is outputing a *reversed* value of \ref AI_1
	- example: a value 2.5V was measured on \ref AI_1, 3.3V-2.5V=0.8V is on the \ref AO_1 output 
- \ref AO_2 (PA5) currently without function 

The program utilize the STM32F407 Discovery board LED
- \ref DO_LED_ORANGE (PD13)
	- toggles on and off every second
- \ref DO_LED_RED  (PD14)
	- is on if both \ref DI_1 and \ref DI_2 are on 
- \ref DO_LED_BLUE  (PD15)
	- is on if \ref programData.mx_counter is even number, off if it is odd 