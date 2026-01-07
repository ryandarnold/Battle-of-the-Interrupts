// dac.c
// This software configures DAC output
// Lab 6 requires a minimum of 4 bits for the DAC, but you could have 5 or 6 bits
// Runs on LM4F120 or TM4C123
// Program written by: put your names here
// Date Created: 3/6/17 
// Last Modified: 3/5/18 
// Lab number: 6
// Hardware connections
// TO STUDENTS "REMOVE THIS LINE AND SPECIFY YOUR HARDWARE********

#include <stdint.h>
#include "tm4c123gh6pm.h"
// Code files contain the actual implemenation for public functions
// this file also contains an private functions and private data

// **************DAC_Init*********************
// Initialize 4-bit DAC, called once 
// Input: none
// Output: none
void DAC_Init(void){
		SYSCTL_RCGCGPIO_R 	|= 0x2; 	// Turn on clock for port B
	
		uint8_t volatile nop;				// Wait for clock to stabilize
		nop ++;
		nop = nop * 3;
		nop = nop/4;
	
		GPIO_PORTB_DIR_R 		|= 0x3F;		// Setting PB0-5 to outputs
	
		GPIO_PORTB_DEN_R		|= 0x3F;		// Digitally enable PB0-5
	
}

// **************DAC_Out*********************
// output to DAC
// Input: 4-bit data, 0 to 63
// Input=n is converted to n*3.3V/15
// Output: none
void DAC_Out(uint32_t data){
	
		// Input is put into port B, causing certain PB pins to activate.  DAC hardware outputs desired voltage
		GPIO_PORTB_DATA_R = data;				
	
}
