/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef INTERFACE_H
#define INTERFACE_H


/* Includes ------------------------------------------------------------------*/
#include <stm32f10x.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_dma.h>


/* Exported types ------------------------------------------------------------*/
struct Voxel
{
	uint8_t x, y, z;
};


/* Exported constants --------------------------------------------------------*/
enum PLANE{ XPLANE, YPLANE, ZPLANE};
enum AXIS{ X, Y, Z};


/* Exported macro ------------------------------------------------------------*/
#define DATA_HI		GPIO_SetBits(GPIOA, GPIO_Pin_7)
#define DATA_LO		GPIO_ResetBits(GPIOA, GPIO_Pin_7)
#define CLK_HI		GPIO_SetBits(GPIOA, GPIO_Pin_5)
#define CLK_LO		GPIO_ResetBits(GPIOA, GPIO_Pin_5)
#define SHIFT_HI	GPIO_SetBits(GPIOA, GPIO_Pin_4)
#define SHIFT_LO	GPIO_ResetBits(GPIOA, GPIO_Pin_4)
#define OE_DIS		GPIO_SetBits(GPIOA, GPIO_Pin_3)
#define OE_EN		GPIO_ResetBits(GPIOA, GPIO_Pin_3)

#define BitSet(p,m) ((p) |= (1<<(m)))
#define BitReset(p,m) ((p) &= ~(1<<(m)))
#define BitFlip(p,m) ((p) ^= (m))
#define BitWrite(c,p,m) ((c) ? BitSet(p,m) : BitReset(p,m))
#define BitIsSet(reg, bit) (((reg) & (1<<(bit))) != 0)
#define BitIsReset(reg, bit) (((reg) & (1<<(bit))) == 0)

/* Exported define -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
/*HAL*/
void MatrixHAL(uint8_t*, uint8_t*);
void CubeHAL();
void ButtonsRead(uint8_t*);

/* Cube functions */
void CubeSet();
void CubeReset();

uint8_t CubeSetVoxel(uint8_t, uint8_t, uint8_t);
uint8_t CubeResetVoxel(uint8_t, uint8_t, uint8_t);
uint8_t CubeGetVoxel(uint8_t, uint8_t, uint8_t);
void CubeSetPlane(uint8_t, uint8_t);
void CubeResetPlane(uint8_t, uint8_t);
void CubeSetLine(uint8_t, uint8_t, uint8_t);
void CubeResetLine(uint8_t, uint8_t, uint8_t);


/* Matrix functions */
void Matrix2Chars(char*, uint8_t*);
void MatrixString(char*, uint8_t*);
void MatrixShift();

//void __delay_ms( volatile uint32_t nTime );

#endif //INTERFACE_H
