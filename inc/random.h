#ifndef RANDOM_H
#define RANDOM_H

#include <stm32f10x.h>
#include <stm32f10x_adc.h>

void RandomInit(void);
uint16_t GetRandom(void);
uint16_t GetRandomBig(void);

#endif // RANDOM_H
