CubeMonitor+Vue Demo (MCU programm part) {#mainpage}
============

Demonstration video: 
 
This project is a part of a larger project *CubeMonitor+Vue Demo*. This part contains the code for MCU created with HAL libraries, in this for STM32F407 evaluation board. In case you do not have STM32F407 evaluation board, you can port the program to any STM32 MCU as the Cube Monitor only cares about variable names/addresses. 

## Motivation for creating CubeMonitor+Vue Demo
The CubeMonitor tool enables program runtime monitoring using NodeRed. With the NodeRed 'function-block-like' programming and preconfigured elements (like LEDs for input status or dial for analog input and even a *write* table for variable setting) you can create simple dashboards. As long as you want only few inputs and you do not really care about setting a lot of variables manually, this is completely fine.  

I did not really like the dashboard (it got pretty full and laggy after a while) and the way how it is created (I guess a lot of C/C++ programmers somehow dislike visual programing and wiring function blocks with mouse). I was also missing a simple way how to set variables using graphical elements like toggle switching. 

That is why I decided to use the NodeRed ui-builder plugin to create a custom UI with all the inputs/outputs/information I want. 


## About this part 

### Setup
You will need: 
	- STM32F407 Discovery board or any other evaluation board 
	- STM IDE (turns out the Atollic and STM IDE are not really back-compatible, the configuration is broken...) 
	
Steps:
	1) Fork and clone this repository 
	2) Import the project into STM IDE 
	3) The project setting should be imported as well but just to be sure this is the build command used: 
		`` 
	4) Build and flash the Discovery board with the code 
	5) Continue with the second part setup: 
	
### Structure
There are two main parts of the program
	- lib 
		- contains libraries for manipulation of the digital and analog inputs/outputs 
	- prog 
		- uses the libraries from *lib* to initialize used peripherials in init.c file 
		- handles the program logic created to demonstrate the CubeMonitor+Vue Demo functionality 
