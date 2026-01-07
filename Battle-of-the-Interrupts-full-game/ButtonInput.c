#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "ST7735.h"
#include "Random.h"
#include "PLL.h"
#include "ADC.h"
#include "Sound.h"
#include "Timer0.h"
#include "Timer1.h"
#include "Characters.h"
#include "ButtonInput.h"

#define PF1       (*((volatile uint32_t *)0x40025008))
#define PF2       (*((volatile uint32_t *)0x40025010))
#define PF3       (*((volatile uint32_t *)0x40025020))

uint8_t 	correctInput[4];
uint8_t		warrior1Index=0;
uint8_t		warrior2Index=0;
uint8_t		player1Victory=0;
uint8_t		player2Victory=0;

void	testInput(void){
	correctInput[0] = 0;
	correctInput[1] = 1;
	correctInput[2] = 2;
}

void randomInput(void){
	uint32_t randomValue = 0;
	struct	inputStruct{
		uint8_t	inputs[3];
	};
	
	struct inputStruct	inputCombo[6] = {
		{0,1,2},
		{0,2,1},
		{1,0,2},
		{1,2,0},
		{2,1,0},
		{2,0,1}
	};
	
	 randomValue = TIMER0_TAV_R;
	
	if ((randomValue%5) == 0){
		for (uint8_t i =0; i<3; i++){
			correctInput[i] = inputCombo[0].inputs[i];
		}
	}
	else if ((randomValue%5) == 1){
		for (uint8_t i =0; i<3; i++){
			correctInput[i] = inputCombo[1].inputs[i];
		}
	}
	else if ((randomValue%5) == 2){
		for (uint8_t i =0; i<3; i++){
			correctInput[i] = inputCombo[2].inputs[i];
		}
	}
	else if ((randomValue%5) == 3){
		for (uint8_t i =0; i<3; i++){
			correctInput[i] = inputCombo[3].inputs[i];
		}
	}
	else if ((randomValue%5) == 4){
		for (uint8_t i =0; i<3; i++){
			correctInput[i] = inputCombo[4].inputs[i];
		}
	}
	else if ((randomValue%5) == 5){
		for (uint8_t i =0; i<3; i++){
			correctInput[i] = inputCombo[5].inputs[i];
		}
}
}

void Delay1msa(uint32_t n)
	{
	uint32_t volatile time;
  while(n){
    time = 72724*2/91;  // 1msec, tuned at 80 MHz
    while(time){
	  	time--;
    }
    n--;
  }
}

uint8_t flagE0=0;
uint8_t flagE1=0;
uint8_t flagE2=0;


uint8_t	GPIOPortE_Handler(void){
	GPIO_PORTE_ICR_R = 0x7;
	uint32_t 	tempInput;
	tempInput = GPIO_PORTE_DATA_R & 0x7;
	
	//Delay1msa(100);

	
	if (tempInput == 0x1 && flagE0 ==0){
				flagE0=1;
		warrior2.input[warrior2Index] = 0;
		PF1 ^=0x2;
		warrior2Index++;
	}
	else if (tempInput == 0x2 && flagE1 ==0){
				flagE1=1;
		warrior2.input[warrior2Index] = 1;
		PF2^=0x4;
		warrior2Index++;
	}
	else if (tempInput == 0x4 && flagE2 ==0){
				flagE2=1;
		warrior2.input[warrior2Index] = 2;
		PF3^=0x8;
		warrior2Index++;
	}
	

	if (warrior2Index > 4){
		warrior2Index = 4;
	}
	//Call checkPlayer2
	//Since edge trigger does not operate on ADC/Slide pot, make player press button after drawing slide pot
	if (checkPlayer2() == 1){
		return(1);
	}
	else{
		return(0);
	}
}

//Whenever trigger interrupts, will put button into character input array
//Will also check if player meets correct input conditions
//Will return 1 if correct, 0 if incorrect

uint8_t flagC0=0;
uint8_t flagC1=0;
uint8_t flagC2=0;

uint8_t GPIOPortC_Handler(void){	//Have to rename
	//When trigger interrupts, check whatever button was pressed and put into warrior1.input[i]
	GPIO_PORTC_ICR_R = 0x070;
	
	uint32_t 	tempInput;
	tempInput = (GPIO_PORTC_DATA_R | 0x7) >> 4;
	
//	Delay1msa(100);

	
	if (tempInput == 0x1 && flagC0 ==0){
				flagC0=1;
		warrior1.input[warrior1Index] = 0;
		PF1 ^=0x2;
		warrior1Index++;
	}
	else if (tempInput == 0x2 && flagC1 ==0){
		flagC1=1;
		warrior1.input[warrior1Index] = 1;
		PF2^=0x4;
		warrior1Index++;
	}
	else if (tempInput == 0x4 && flagC2 ==0){
				flagC2=1;
		warrior1.input[warrior1Index] = 2;
		PF3^=0x8;
		warrior1Index++;
	}
	

	if (warrior1Index > 4){
		warrior1Index = 4;
	}
	//Call checkPlayer1
	//Since edge trigger does not operate on ADC/Slide pot, make player press button after drawing slide pot
	if (checkPlayer1() == 1){
		return(1);
	}
	else{
		return(0);
	}
	
}

uint8_t checkPlayer1(void){
	if (warrior1.input[0] == correctInput[0] && warrior1.input[1] == correctInput[1] && warrior1.input[2] == correctInput[2]){
		SYSCTL_RCGCGPIO_R &= ~0x10; 	
		player1Victory=1;
		
		return(1);
	}
	else{
		return(0);
	}
}

uint8_t checkPlayer2(void){
	if (warrior2.input[0] == correctInput[0] && warrior2.input[1] == correctInput[1] && warrior2.input[2] == correctInput[2]){
		SYSCTL_RCGCGPIO_R &= ~0x4; 
		player2Victory=1;
		return(1);
	}
	else{
		return(0);
	}	
}
