/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef RANDOM_H
#define RANDOM_H


/* Includes ------------------------------------------------------------------*/
#include <stm32f10x.h>
#include <stm32f10x_adc.h>


/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void RandomInit();
uint16_t GetRandom();
uint16_t GetRandomBig();

#endif //RANDOM_H
