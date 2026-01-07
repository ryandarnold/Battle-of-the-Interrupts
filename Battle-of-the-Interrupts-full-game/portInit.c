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

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

void Delay1msb(uint32_t n)
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

//Initializes Port C as edge triggered and allows for PC4-6 to function as inputs
void	portNoEdgeInit(void){
 // --UWU-- Code to initialize PF4 and PF2
	

	SYSCTL_RCGCGPIO_R = 0x21;
	
	volatile uint8_t NOP;
	NOP=NOP*5; // waits for clock to stabalize 
	NOP+=1;
	NOP = NOP/NOP; 
	NOP = NOP*NOP+32423/3-2*2317/23+123*NOP;
	
	GPIO_PORTF_LOCK_R = 0x4C4F434B;  //unlocks PORTF and sets it Direction and Enables it
	GPIO_PORTF_CR_R = 0x1F;
	GPIO_PORTF_DIR_R = 0x02;
	GPIO_PORTF_DEN_R = 0x12;
	GPIO_PORTF_DIR_R |= 0xE;					// Make PF1-PF3 outputs
	GPIO_PORTF_DEN_R |= 0xE;					// Digitally Enable PF1-PF3
	GPIO_PORTF_PUR_R = 0x11;
	
	GPIO_PORTA_DIR_R = 0xEC; //sets port A direction and enables it
	GPIO_PORTA_DEN_R = 0xEC;
	
}

void waitForPress(void) 
	{
		
		
		
 // --UUU-- wait for release; delay for 20ms; and then wait for press
		if ((GPIO_PORTF_DATA_R & 0x10) == 0x0) //pushed 
			{ 
			while ((GPIO_PORTF_DATA_R & 0x10)==0x00)
				{}
					Delay1msb(50);
			}
			
		else
			{
			while ((GPIO_PORTF_DATA_R & 0x10) == 0x10)	//not pushed
				{}
			while ((GPIO_PORTF_DATA_R & 0x10)==0x00)
				{}
					Delay1msb(50);
			}
	}

void portCEdgeInit(void){
	SYSCTL_RCGCGPIO_R |= 0x00000004; // (a) activate clock for port F
	volatile uint8_t		nop;
	nop++;
	nop *= nop;
	nop++;
	nop= nop/3*3423+nop-234;
  GPIO_PORTC_DIR_R &= ~0x70;    // (c) make PC4-6 in (built-in button)
  GPIO_PORTC_AFSEL_R &= ~0x70;  //     disable alt funct on PC4-6
  GPIO_PORTC_DEN_R |= 0x70;     //     enable digital I/O on PC4-6
  GPIO_PORTC_PCTL_R &= ~0x0FFF0000; // configure PC4-6 as GPIO
  GPIO_PORTC_AMSEL_R = 0;       //     disable analog functionality on PC
  //GPIO_PORTF_PUR_R |= 0x10;     //     enable weak pull-up on PF4
  GPIO_PORTC_IS_R &= ~0x70;     // (d) PC4-6 are edge-sensitive
  GPIO_PORTC_IBE_R &= ~0x70;    //     PC4-6 is not both edges
	
	
  GPIO_PORTC_IEV_R |= 0x70;    //     PC4-6 rising edge event
	
	
  GPIO_PORTC_ICR_R = 0x70;      // (e) clear flag4-6
  GPIO_PORTC_IM_R |= 0x70;      // (f) arm interrupt on PC4-6 *** No IME bit as mentioned in Book ***
  NVIC_PRI0_R = (NVIC_PRI0_R&0xFF00FFFF)|0x00400000; // (g) priority 4
  NVIC_EN0_R = 0x00000004;      // (h) enable interrupt 2 in NVIC
  EnableInterrupts();           // (i) Clears the I bit
	
}

void portEEdgeInit(void){
	SYSCTL_RCGCGPIO_R |= 0x00000010; // (a) activate clock for port E
	volatile uint8_t		nop;
	nop++;
	nop *= nop;
	nop++;
	nop= nop/3*3423+nop-234;
  GPIO_PORTE_DIR_R &= ~0x07;    // (c) make PE0-2 in (built-in button)
  GPIO_PORTE_AFSEL_R &= ~0x7;  //     disable alt funct on PE0-2
  GPIO_PORTE_DEN_R |= 0x7;     //     enable digital I/O on PE0-2
  GPIO_PORTE_PCTL_R &= ~0x00000FFF; // configure PE0-2 as GPIO
  GPIO_PORTE_AMSEL_R = 0;       //     disable analog functionality on PC
  //GPIO_PORTF_PUR_R |= 0x10;     //     enable weak pull-up on PF4
  GPIO_PORTE_IS_R &= ~0x7;     // (d) PE0-2 are edge-sensitive
  GPIO_PORTE_IBE_R &= ~0x7;    //     PE0-2 is not both edges
	
  GPIO_PORTE_IEV_R |= 0x7;    //     PE0-2 falling edge event
	
  GPIO_PORTE_ICR_R = 0x7;      // (e) clear flag0-2
  GPIO_PORTE_IM_R |= 0x7;      // (f) arm interrupt on PE0-2 *** No IME bit as mentioned in Book ***
  NVIC_PRI1_R = (NVIC_PRI0_R&0xFFFFFF0F)|0x00000040; // (g) priority 4
  NVIC_EN0_R = 0x00000010;      // (h) enable interrupt 4 in NVIC
  EnableInterrupts();           // (i) Clears the I bit
	
}
