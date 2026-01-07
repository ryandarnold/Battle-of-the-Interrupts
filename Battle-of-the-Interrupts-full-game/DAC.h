// put prototypes for public functions, explain what it does
// Ryan Arnold and Austin Tsao, May 1, 2019


#ifndef DAC_H
#define DAC_H
#include <stdint.h>
// Header files contain the prototypes for public functions
// this file explains what the module does

// **************DAC_Init*********************
// Initialize 6-bit DAC, called once 
// Input: none
// Output: none
void DAC_Init(void);


// **************DAC_Out*********************
// output to DAC
// Input: 4-bit data, 0 to 15 
// Input=n is converted to n*3.3V/15
// Output: none
void DAC_Out(uint32_t data);

#endif




