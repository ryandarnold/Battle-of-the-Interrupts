// ADC.c
// Runs on LM4F120/TM4C123
// Provide functions that initialize ADC0
// Last Modified: 11/6/2018
// Student names: change this to your names or look very silly
// Last modification date: change this to the last modification date or look very silly

#include <stdint.h>
#include "tm4c123gh6pm.h"

#define PF1       (*((volatile uint32_t *)0x40025008))
#define PF2       (*((volatile uint32_t *)0x40025010))
#define PF3       (*((volatile uint32_t *)0x40025020))

// ADC initialization function 
// Input: none
// Output: none
// measures from PD2, analog channel 5
void ADC_Init(void){ 
	uint32_t	delay=0;
	delay++;
	volatile uint32_t delay2;
	SYSCTL_RCGCGPIO_R |= 0x8;      // 1) activate clock for Port D 			***CHANGE TO PD2***  good!
  //while((SYSCTL_PRGPIO_R&0x8) == 0){}; 													//		***CHANGE TO PD2***  good!
	delay2++;
	delay2++;
	delay2++;
	delay2++;
	delay2++;
	delay2++;
	delay2++;
  GPIO_PORTD_DIR_R &= ~0x4	;      // 2) make PD2 input  							***CHANGE TO PD2***	 good!
  GPIO_PORTD_AFSEL_R |= 0x4;     // 3) enable alternate fun on PD2 		***CHANGE TO PD2***  good!
  GPIO_PORTD_DEN_R &= ~0x4;      // 4) disable digital I/O on PD2 	 	***CHANGE TO PD2***	 good!
  GPIO_PORTD_AMSEL_R |= 0x4;     // 5) enable analog fun on PD2	    	***CHANGE TO PD2***  good!
  SYSCTL_RCGCADC_R |= 0x01;       // 6) activate ADC0 

  delay2++;
	delay2++;
	delay2++;
	delay2++;
	delay2++;
	delay2++;
	delay2++;
	delay2++;
  ADC0_PC_R = 0x01;               // 7) configure for 125K 
  ADC0_SSPRI_R = 0x0123;          // 8) Seq 3 is highest priority
  ADC0_ACTSS_R &= ~0x0008;        // 9) disable sample sequencer 3
  ADC0_EMUX_R &= ~0xF000;         // 10) seq3 is software trigger
  ADC0_SSMUX3_R = (ADC0_SSMUX3_R&0xFFFFFFF0)+5;  // 11) Ain2 (PD2)		***CHANGE TO PD2***  good!
  ADC0_SSCTL3_R = 0x0006;         // 12) no TS0 D0, yes IE0 END0
	//ADC0_SAC_R = 0x06;              //    2^{n-1} hardware averaging
  ADC0_IM_R &= ~0x0008;           // 13) disable SS3 interrupts
  ADC0_ACTSS_R |= 0x0008;         // 14) enable sample sequencer 3
		
}

//------------ADC_In------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC conversion
// measures from PD2, analog channel 5
uint32_t ADC_In(void){  
	uint32_t data;

	ADC0_PSSI_R = 0x0008;            
  while((ADC0_RIS_R&0x08)==0){}; //ADC0 RIS register sets bit 3 when conversion is complete  
  data = ADC0_SSFIFO3_R&0xFFF; //the ADC FIFO has the queue data and ANDS it will all 1's to get the 12 bit information
  ADC0_ISC_R = 0x0008; 
  return data;
	

}


