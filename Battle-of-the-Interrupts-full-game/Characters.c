#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "ST7735.h"
#include "Random.h"
#include "PLL.h"
#include "ADC.h"
#include "Sound.h"
#include "Timer0.h"
#include "Timer1.h"

	struct character {
		uint8_t 	input[3];
		uint16_t	xpos;
		uint16_t	ypos;
		uint32_t	health;
	};
	struct character warrior1;			//Left person
	struct character warrior2;			//Right person 
	
	void characterReset(void){
		for (uint8_t i=0; i<4; i++){
			warrior1.input[i] = 4;
			warrior2.input[i] = 4;
		}			
	}
	
	void characterInit(void){
		for (uint8_t i=0; i<4; i++){
			warrior1.input[i] = 4;
			warrior2.input[i] = 4;
		}
			warrior1.health = 3;
			warrior2.health= 3;
		//Reset position and health?
		
	}
	
	
	
	
	
	
	
	
	

	//
	
	
	



	