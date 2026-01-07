#include <stdint.h>

extern uint8_t 	correctInput[4];

void	testInput(void);
void randomInput(void);
uint8_t GPIOPortC_Handler(void);
uint8_t checkPlayer1(void);
uint8_t checkPlayer2(void);

void Delay100ms(uint32_t count);
