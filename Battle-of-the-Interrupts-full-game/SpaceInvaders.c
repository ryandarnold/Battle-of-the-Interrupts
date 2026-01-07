// SpaceInvaders.c
// Runs on LM4F120/TM4C123
// Jonathan Valvano and Daniel Valvano
// This is a starter project for the EE319K Lab 10

// Last Modified: 11/20/2018 
// http://www.spaceinvaders.de/
// sounds at http://www.classicgaming.cc/classics/spaceinvaders/sounds.php
// http://www.classicgaming.cc/classics/spaceinvaders/playguide.php
/* This example accompanies the books
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2018

   "Embedded Systems: Introduction to Arm Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2018

 Copyright 2018 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */
 
 
// ******* Possible Hardware I/O connections*******************
// Slide pot pin 1 connected to ground
// Slide pot pin 2 connected to PD2/AIN5
// Slide pot pin 3 connected to +3.3V 
// fire button connected to PE0
// special weapon fire button connected to PE1
// 8*R resistor DAC bit 0 on PB0 (least significant bit)
// 4*R resistor DAC bit 1 on PB1
// 2*R resistor DAC bit 2 on PB2
// 1*R resistor DAC bit 3 on PB3 (most significant bit)
// LED on PB4
// LED on PB5

// Backlight (pin 10) connected to +3.3 V
// MISO (pin 9) unconnected
// SCK (pin 8) connected to PA2 (SSI0Clk)
// MOSI (pin 7) connected to PA5 (SSI0Tx)
// TFT_CS (pin 6) connected to PA3 (SSI0Fss)
// CARD_CS (pin 5) unconnected
// Data/Command (pin 4) connected to PA6 (GPIO), high for data, low for command
// RESET (pin 3) connected to PA7 (GPIO)
// VCC (pin 2) connected to +3.3 V
// Gnd (pin 1) connected to ground

#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "ST7735.h"
#include "Random.h"
#include "PLL.h"
#include "ADC.h"
#include "Images.h"
#include "Sound.h"
#include "Timer0.h"
#include "Timer1.h"
#include "Dac.h"
#include "ButtonInput.h"
#include "portInit.h"
#include "Characters.h"

#define PF1       (*((volatile uint32_t *)0x40025008))
#define PF2       (*((volatile uint32_t *)0x40025010))
#define PF3       (*((volatile uint32_t *)0x40025020))

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void Delay100ms(uint32_t count); // time delay in 0.1 seconds

extern uint8_t		warrior1Index;
extern uint8_t		warrior2Index;
extern uint8_t flagC0;
extern uint8_t flagC1;
extern uint8_t flagC2;
extern uint8_t flagE0;
extern uint8_t flagE1;
extern uint8_t flagE2;
uint32_t Data;        // 12-bit ADC
uint32_t Position;    // 32-bit fixed-point 0.001 
uint16_t	ADCMail;
uint8_t		ADCstatus=0;

void PortF_Init(void){
	volatile uint32_t delay=0;				// Wait to stabilize
	SYSCTL_RCGCGPIO_R |= 0x20;				// Enable PortF_Init F
	delay++;
	delay++;
	delay++;
	delay++;
	delay++;
	delay++;
	GPIO_PORTF_DIR_R |= 0xE;					// Make PF1-PF3 outputs
	GPIO_PORTF_DEN_R |= 0xE;					// Digitally Enable PF1-PF3
}

void GPIOPortF_Handler(void){
	GPIO_PORTF_ICR_R = 0x10;      // acknowledge flag4
	
	Delay100ms(1);
  
	PF1 ^= 0x2;
}

void SysTick_Init(void){
		NVIC_ST_CTRL_R=0;
		NVIC_ST_RELOAD_R = 0x145855; //this reload value is for 60Hz
		NVIC_ST_CURRENT_R = 0;
		NVIC_SYS_PRI3_R = 0xA0000000;
		NVIC_ST_CTRL_R = 0x07;	
	
}

void SysTick_Handler(void){
		
		//PF2	^= 0x4;
		//PF1 ^= 0x2;
		ADCMail = ADC_In(); //gets the input from the ADC_In to run at 60Hz
		ADCstatus = 1;
		//PF3	^= 0x8;
	
}


uint32_t Convert(uint32_t input){
	Position = (1976*input)/4096+105; 			
  return(Position);
}


int mainSoundTest(void){ 		//Works, but has tons of static
	PLL_Init(Bus80MHz); 
	Output_Init();
	DAC_Init();
	Timer0_Init(*musicPlay,7526); 
	while(1){}
}
uint32_t	seed;
int mainTestMech(void){ 		
	PLL_Init(Bus80MHz); 
	ADC_Init();
	//PortF_Init();
	portNoEdgeInit();
	Output_Init();
	DAC_Init();
	ST7735_InitR(INITR_REDTAB);
	uint16_t	info;
	testInput();
	characterInit();
	Timer0_Init(*musicPlay,7526); 
	ST7735_FillScreen(0x0000);  
	ST7735_DrawBitmap(10,100,BattleOfTheInterrupts1,100,54);
	Timer1_Init(*task, 0x00FFFFFF);
	waitForPress();
	seed = TIMER1_TAV_R;
	randomInput();
	TIMER1_CTL_R = 0x00000000;  
	randomInput();
	ST7735_FillScreen(0x0000); 
	while(1){}

	
}

void flagZero(void){
	flagE0=0;
	flagE1=0;
	flagE2=0;
	flagC0=0;
	flagC1=0;
	flagC2=0;
}

int main(void){
	PLL_Init(Bus80MHz); 
	ADC_Init();
	//PortF_Init();
	portNoEdgeInit();
	Output_Init();
	DAC_Init();
	ST7735_InitR(INITR_REDTAB);
	uint16_t	info;
	testInput();
	characterInit();
	//randomInput();
	Timer0_Init(*musicPlay,7526); 
	ST7735_FillScreen(0x0000);  
	ST7735_DrawBitmap(10,100,BattleOfTheInterrupts1,100,54);
	Delay100ms(5);
	waitForPress();
	randomInput();
	
	ST7735_FillScreen(0x0000); 
	ST7735_DrawBitmap(35,160,Yerraballi1,25,40);
	ST7735_DrawBitmap(80,160,Ninja1Flipped,20,33);
	//ST7735_DrawBitmap(80,160,Ninja1Flipped,20,33);
//	ST7735_FillScreen(0x0000);   
//	ST7735_DrawBitmap(5,160,Ninja3Flipped,45,36);
//	ST7735_DrawBitmap(5,160,Ninja2Flipped,65,34);
//	S4T7735_DrawBitmap(5,160,Ninja2,65,34);
	portCEdgeInit();
	portEEdgeInit();
	SysTick_Init();
	EnableInterrupts();
	
	ST7735_OutString("Player 1 health: ");
	LCD_OutDec(warrior1.health);
	ST7735_OutString("\nPlayer 2 health: ");
	LCD_OutDec(warrior2.health);
	ST7735_OutString("\nPress buttons: ");
	LCD_OutDec(correctInput[0]); ST7735_OutString(" ");
	LCD_OutDec(correctInput[1]); ST7735_OutString(" ");
	LCD_OutDec(correctInput[2]); 
	
	while(1)
		{
			
		if (ADCMail>500){
			SYSCTL_RCGCGPIO_R &= ~0x10; 
			SYSCTL_RCGCGPIO_R &= ~0x4; 
			ST7735_SetCursor(0,0);
			ST7735_OutString("Slide the pot back\n");
			ST7735_OutString("Player 1 health: ");
			LCD_OutDec(warrior1.health);
			ST7735_OutString("\nPlayer 2 health: ");
			LCD_OutDec(warrior2.health);
			randomInput();
			flagZero();
			
			while(ADCMail>500){}
			SYSCTL_RCGCGPIO_R |= 0x10; 
			volatile uint8_t nop;
			nop= nop*nop+nop-nop/nop*2-384/2*2387;
			SYSCTL_RCGCGPIO_R |= 0x4; 
			nop= nop*nop+nop-nop/nop*2-384/2*2387;
			ST7735_FillScreen(0x0000); 
			ST7735_SetCursor(0,0);
			ST7735_OutString("Player 1 health: ");
			LCD_OutDec(warrior1.health);
			ST7735_OutString("\nPlayer 2 health: ");
			LCD_OutDec(warrior2.health);
			ST7735_OutString("\nPress buttons: ");
			LCD_OutDec(correctInput[0]); ST7735_OutString(" ");
			LCD_OutDec(correctInput[1]); ST7735_OutString(" ");
			LCD_OutDec(correctInput[2]); 
			ST7735_DrawBitmap(35,160,Yerraballi1,25,40);
			ST7735_DrawBitmap(80,160,Ninja1Flipped,20,33);
			ST7735_SetCursor(0,0);
			flagZero();
			
		}			
		
		
		while ((checkPlayer1() ==1)){
				while(ADCstatus == 0){}
				ADCstatus = 0;
				info = Convert(ADCMail);
				randomInput();
					flagZero();
				ST7735_FillScreen(0x0000); 
				ST7735_SetCursor(0,0);
				ST7735_OutString("Player 1 attacks\n");
				ST7735_OutString("Player 1 health: ");
				LCD_OutDec(warrior1.health);
				ST7735_OutString("\nPlayer 2 health: ");
				LCD_OutDec(warrior2.health);
				
				//ST7735_SetCursor(9,0);	
					
				ST7735_DrawBitmap(35,160,Yerraballi1,25,40);
				ST7735_DrawBitmap(80,160,Ninja1Flipped,20,33);
					
				while (info <= 800){
					while(ADCstatus == 0){}
					ADCstatus = 0;
					info = Convert(ADCMail);
					ST7735_SetCursor(0,0);
				}
				
				ST7735_FillScreen(0x0000); 
				ST7735_DrawBitmap(80,160,Ninja1Flipped,20,33);
				ST7735_DrawBitmap(0,160,Yerraballi2,75,40);
				ST7735_SetCursor(0,0);
				ST7735_OutString("Player 1 attacks\n");
				ST7735_OutString("Player 1 health: ");
				LCD_OutDec(warrior1.health);
				ST7735_OutString("\nPlayer 2 health: ");
				LCD_OutDec(warrior2.health);
				
				while (info <= 1800){
					while(ADCstatus == 0){}
					ADCstatus = 0;
					info = Convert(ADCMail);
					ST7735_SetCursor(0,0);
				}
				
				ST7735_FillScreen(0x0000); 
				ST7735_DrawBitmap(80,160,Ninja1Flipped,20,33);
				ST7735_DrawBitmap(15,160,Ninja3,45,36);
				ST7735_SetCursor(0,0);
				ST7735_OutString("Player 1 attacks\n");
				ST7735_OutString("Player 1 health: ");
				LCD_OutDec(warrior1.health);
				ST7735_OutString("\nPlayer 2 health: ");
				LCD_OutDec(warrior2.health);
				
				while (info >= 1800){
					while(ADCstatus == 0){}
					ADCstatus = 0;
					info = Convert(ADCMail);
					ST7735_SetCursor(0,0);
				}
				ST7735_FillScreen(0x0000); 
				ST7735_DrawBitmap(80,160,Ninja1Flipped,20,33);
				ST7735_DrawBitmap(0,160,Yerraballi2,75,40);
				ST7735_SetCursor(0,0);
				ST7735_OutString("Player 1 attacks\n");
				ST7735_OutString("Player 1 health: ");
				LCD_OutDec(warrior1.health);
				ST7735_OutString("\nPlayer 2 health: ");
				LCD_OutDec(warrior2.health);
				
				while (info >= 800){
					while(ADCstatus == 0){}
					ADCstatus = 0;
					info = Convert(ADCMail);
					ST7735_SetCursor(0,0);
				}	
				
				ST7735_FillScreen(0x0000); 
				ST7735_DrawBitmap(80,160,Ninja1Flipped,20,33);
				ST7735_DrawBitmap(35,160,Yerraballi1,25,40);	
				Delay100ms(5);
				
				for (uint8_t i=0; i<4; i++){
					warrior1.input[i] = 4;
					warrior2.input[i] = 4;
				}			
				portEEdgeInit();
				warrior1Index=0;
				warrior2Index=0;
				warrior2.health--;
				flagZero();
				ST7735_SetCursor(0,0);
				ST7735_OutString("Player 1 health: ");
				LCD_OutDec(warrior1.health);
				ST7735_OutString("\nPlayer 2 health: ");
				LCD_OutDec(warrior2.health);
				ST7735_OutString("\nPress buttons: ");
				LCD_OutDec(correctInput[0]); ST7735_OutString(" ");
				LCD_OutDec(correctInput[1]); ST7735_OutString(" ");
				LCD_OutDec(correctInput[2]); 
				ST7735_SetCursor(0,0);
				
				
				if (warrior2.health <= 0){
					//Display victory message
					ST7735_FillScreen(0x0000);
					ST7735_SetCursor(0,0);
					ST7735_OutString("Player 1 health: ");
					LCD_OutDec(warrior1.health);
					ST7735_OutString("\nPlayer 2 health: ");
					LCD_OutDec(warrior2.health);
					ST7735_OutString("\nPlayer 1 wins!\n");
					SYSCTL_RCGCGPIO_R &= ~0x10; 
					SYSCTL_RCGCGPIO_R &= ~0x4; 
					ST7735_OutString("Reset to play again");
					while(1){};
				}
			}
		
			
		
		while (checkPlayer2() == 1){
				while(ADCstatus == 0){}
				ADCstatus = 0;
				info = Convert(ADCMail);
				randomInput();
					flagZero();
				ST7735_FillScreen(0x0000); 
				ST7735_SetCursor(0,0);
				ST7735_OutString("Player 2 attacks\n");
				ST7735_OutString("Player 1 health: ");
				LCD_OutDec(warrior1.health);
				ST7735_OutString("\nPlayer 2 health: ");
				LCD_OutDec(warrior2.health);
				//ST7735_SetCursor(9,0);	
					
				ST7735_DrawBitmap(35,160,Yerraballi1,25,40);
				ST7735_DrawBitmap(80,160,Ninja1Flipped,20,33);
					
				while (info <= 800){
					while(ADCstatus == 0){}
					ADCstatus = 0;
					info = Convert(ADCMail);
					ST7735_SetCursor(0,0);
				}
				
				ST7735_FillScreen(0x0000); 
				ST7735_DrawBitmap(58,160,Ninja2Flipped,65,34);
				ST7735_DrawBitmap(35,160,Yerraballi1,25,40);
				ST7735_SetCursor(0,0);
				ST7735_OutString("Player 2 attacks\n");
				ST7735_OutString("Player 1 health: ");
				LCD_OutDec(warrior1.health);
				ST7735_OutString("\nPlayer 2 health: ");
				LCD_OutDec(warrior2.health);
				
				while (info <= 1800){
					while(ADCstatus == 0){}
					ADCstatus = 0;
					info = Convert(ADCMail);
					ST7735_SetCursor(0,0);
				}
				
				ST7735_FillScreen(0x0000); 
				ST7735_DrawBitmap(60,160,Ninja3Flipped,45,36);
				ST7735_DrawBitmap(35,160,Yerraballi1,25,40);
				ST7735_SetCursor(0,0);
				ST7735_OutString("Player 2 attacks\n");
				ST7735_OutString("Player 1 health: ");
				LCD_OutDec(warrior1.health);
				ST7735_OutString("\nPlayer 2 health: ");
				LCD_OutDec(warrior2.health);
				
				while (info >= 1800){
					while(ADCstatus == 0){}
					ADCstatus = 0;
					info = Convert(ADCMail);
					ST7735_SetCursor(0,0);
				}
				ST7735_FillScreen(0x0000); 
				ST7735_DrawBitmap(58,160,Ninja2Flipped,65,34);
				ST7735_DrawBitmap(35,160,Yerraballi1,25,40);
				ST7735_SetCursor(0,0);
				ST7735_OutString("Player 2 attacks\n");
				ST7735_OutString("Player 1 health: ");
				LCD_OutDec(warrior1.health);
				ST7735_OutString("\nPlayer 2 health: ");
				LCD_OutDec(warrior2.health);
				
				while (info >= 800){
					while(ADCstatus == 0){}
					ADCstatus = 0;
					info = Convert(ADCMail);
					ST7735_SetCursor(0,0);
				}	
				
				ST7735_FillScreen(0x0000); 
				ST7735_DrawBitmap(80,160,Ninja1Flipped,20,33);
				ST7735_DrawBitmap(35,160,Yerraballi1,25,40);	

				Delay100ms(5);
				
				for (uint8_t i=0; i<4; i++){
					warrior1.input[i] = 4;
					warrior2.input[i] = 4;
				}			
				portCEdgeInit();
				warrior1Index=0;
				warrior2Index=0;
				flagZero();
				
					for (uint8_t i=0; i<4; i++){
					warrior1.input[i] = 4;
					warrior2.input[i] = 4;
				}			
				portCEdgeInit();
				warrior1Index=0;
				warrior2Index=0;
				warrior1.health--;
				ST7735_SetCursor(0,0);
				ST7735_OutString("Player 1 health: ");
				LCD_OutDec(warrior1.health);
				ST7735_OutString("\nPlayer 2 health: ");
				LCD_OutDec(warrior2.health);
				ST7735_OutString("\nPress buttons: ");
				LCD_OutDec(correctInput[0]); ST7735_OutString(" ");
				LCD_OutDec(correctInput[1]); ST7735_OutString(" ");
				LCD_OutDec(correctInput[2]); 
				ST7735_SetCursor(0,0);
				
				if (warrior1.health <= 0){
					//Display victory message
					ST7735_FillScreen(0x0000);
					ST7735_SetCursor(0,0);
					ST7735_OutString("Player 1 health: ");
					LCD_OutDec(warrior1.health);
					ST7735_OutString("\nPlayer 2 health: ");
					LCD_OutDec(warrior2.health);
					ST7735_OutString("\nPlayer 2 wins!\n");
					SYSCTL_RCGCGPIO_R &= ~0x10; 
					SYSCTL_RCGCGPIO_R &= ~0x4; 
					ST7735_OutString("Reset to play again");
					while(1){};
				}
			}
		
			while (warrior1Index>=3 && warrior2Index>=3){
				SYSCTL_RCGCGPIO_R &= ~0x10; 
				SYSCTL_RCGCGPIO_R &= ~0x4; 
				randomInput();
				ST7735_SetCursor(0,0);
				ST7735_FillScreen(0x0000); 
				ST7735_DrawBitmap(80,160,Ninja1Flipped,20,33);
				ST7735_DrawBitmap(35,160,Yerraballi1,25,40);	
				flagZero();
				ST7735_OutString("Both of y'all suck\n");
				ST7735_OutString("Player 1 health: ");
				LCD_OutDec(warrior1.health);
				ST7735_OutString("\nPlayer 2 health: ");
				LCD_OutDec(warrior2.health);
				ST7735_SetCursor(0,0);
				for (uint8_t i=0; i<4; i++){
					warrior1.input[i] = 4;
					warrior2.input[i] = 4;
				}			
				portCEdgeInit();
				warrior1Index=0;
				warrior2Index=0;
				flagZero();
				
				SYSCTL_RCGCGPIO_R &= ~0x10; 
				SYSCTL_RCGCGPIO_R &= ~0x4; 
				
				Delay100ms(30);
				SYSCTL_RCGCGPIO_R |= 0x10; 
				volatile uint8_t nop;
				nop= nop*nop+nop-nop/nop*2-384/2*2387;
				SYSCTL_RCGCGPIO_R |= 0x4; 
				nop= nop*nop+nop-nop/nop*2-384/2*2387;
				ST7735_FillScreen(0x0000); 
				ST7735_DrawBitmap(80,160,Ninja1Flipped,20,33);
				ST7735_DrawBitmap(35,160,Yerraballi1,25,40);	
				ST7735_SetCursor(0,0);
				ST7735_OutString("Player 1 health: ");
				LCD_OutDec(warrior1.health);
				ST7735_OutString("\nPlayer 2 health: ");
				LCD_OutDec(warrior2.health);
				ST7735_OutString("\nPress buttons: ");
				LCD_OutDec(correctInput[0]); ST7735_OutString(" ");
				LCD_OutDec(correctInput[1]); ST7735_OutString(" ");
				LCD_OutDec(correctInput[2]); 
				ST7735_SetCursor(0,0);
			}
			

			}
				
			}
	
	
//	while(1){}


int mainTest(void){
  PLL_Init(Bus80MHz);       // Bus clock is 80 MHz 
	

		
	PortF_Init();
	ADC_Init();
	ST7735_InitR(INITR_REDTAB);
	
  //Random_Init(1);
  //Output_Init();
	Sound_Init();
	
	uint16_t	info;
  ST7735_FillScreen(0x0000);            // set screen to black
	ST7735_DrawBitmap(10,100,BattleOfTheInterrupts1,100,54);
	
	//Delay100ms(10); //change this out for an interrupt thing
	
	ST7735_FillScreen(0x0000);            // set screen to black
	
	
	//first two tell position, name is name of pic, and then next numbers are size (check in paint)
	ST7735_DrawBitmap(5,160,Ninja3Flipped,45,36); //THIS PICTURE IS 100% CORRECT DON'T ALTER - ONLY WORKS WITH ADDED MEMORY
	
	//ST7735_DrawBitmap(5,160,Ninja2Flipped,65,34);//THIS PICTURE IS 100% CORRECT DONT ALTER
  //ST7735_DrawBitmap(5,160,Ninja1,23,37); //THIS PICTURE IS 100% CORRECT DONT ALTER
	//ST7735_DrawBitmap(0,160,Yerraballi2,75,40); //THIS PICTURE IS 100% CORRECT DONT ALTER
	//ST7735_DrawBitmap(70,160,Ninja3,45,36); // THIS PICTURE IS 100% CORRECT DONT ALTER
	//ST7735_DrawBitmap(80,160,Ninja1Flipped,20,33); //THIS PICTURE IS 100% CORRECT DONT ALTER
	
	ST7735_SetCursor(1, 1);
	ST7735_OutString("GAME OVER");
	LCD_OutDec(1234);
	
	SysTick_Init();
	EnableInterrupts();  //systick only works for ADC if i enable it after i display shit. 
	//DisableInterrupts();
	while(1)
		{
		while(ADCstatus == 0){}
			ADCstatus = 0;
			info = Convert(ADCMail);
			ST7735_SetCursor(0,0);
			LCD_OutDec(info);	 ST7735_OutString(" cm");  ST7735_OutString("    ");
			ST7735_SetCursor(9,0);
		}

  
}

//void Systick_Handler





// You can't use this timer, it is here for starter code only 
// you must use interrupts to perform delays
void Delay100ms(uint32_t count){uint32_t volatile time;
  while(count>0){
    time = 727240;  // 0.1sec at 80 MHz
    while(time){
	  	time--;
    }
    count--;
  }
}

