#include <stdint.h>

	struct character {
		uint8_t 	input[4];
		uint16_t	xpos;
		uint16_t	ypos;
		uint32_t	health;
	};

extern	struct character warrior1;			//Left person
extern	struct character warrior2;
	
void characterInit(void);
