#include "interface.h"
#include "string.h"
#include <stdbool.h>

#define TIMING 100 // 800

volatile uint8_t image[64];
uint8_t matrPos = 0;
uint8_t *pMatrix2, buttonsss;
char *pText2;

extern bool flagShift;
extern uint16_t frameTime;
extern uint8_t Matrix[7];
volatile uint8_t buttonsState = 0;

extern const char font3x5[];

/*******************************************************************************
 * Function Name  : CubeHAL
 * Description    : Draws the cube by multiplexing 8 layers
 *******************************************************************************/
void CubeHAL()
{
	OE_DIS;
	SHIFT_LO;

	static uint8_t g = 0;
	if(++g >= 8) g = 0;

	GPIO_ResetBits(GPIOA, GPIO_Pin_15);
	GPIO_ResetBits(GPIOB, (uint16_t)0x3F8);

	if(!(7 - g))
		GPIO_SetBits(GPIOA, GPIO_Pin_15);
	else
		GPIO_SetBits(GPIOB, (1 << (9 - g)));

#ifndef USE_DMA
	for(uint8_t i = 0; i < 8; i++)
	{
		for(uint8_t y = 0; y < 8; y++)
		{
			if(image[g * 8 + i] & (1 << (7 - y)))
				DATA_HI;
			else
				DATA_LO;

			CLK_HI;
			CLK_LO;
		}
		// SPI_I2S_SendData(SPI1, image[g*8 + (7-i)]);
		// while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
	}
#else
	DMA_Cmd(DMA1_Channel3, DISABLE);
	DMA1_Channel3->CMAR = (uint32_t)&image[g * 8];
	DMA_SetCurrDataCounter(DMA1_Channel3, 8);
	DMA_Cmd(DMA1_Channel3, ENABLE);
	while(SPI1->SR & SPI_I2S_FLAG_BSY)
		;
#endif

	SHIFT_HI;
	OE_EN;
}

/*******************************************************************************
 * Function Name  : CubeReset
 * Description    : Make all cube voxels (LEDs) off
 *******************************************************************************/
void CubeReset()
{
	for(uint8_t i = 0; i < 64; i++)
		image[i] = 0;
}

/*******************************************************************************
 * Function Name  : CubeSet
 * Description    : Make all cube voxels (LEDs) on
 *******************************************************************************/
void CubeSet()
{
	for(uint8_t i = 0; i < 64; i++)
		image[i] = 0xFF;
}

/*******************************************************************************
 * Function Name  : CubeSetVoxel
 * Description    : Sets one cube voxel (LED)
 * Input          : Voxel coordinates
 * Return         : opCode
 *******************************************************************************/
uint8_t CubeSetVoxel(uint8_t x, uint8_t y, uint8_t z)
{
	if(x > 7 || y > 7 || z > 7) return 2;				 // invalid parameters
	if((image[8 * z + 7 - y] & (1 << x)) == 1) return 1; // voxel is already set

	image[8 * z + 7 - y] |= (1 << x);
	return 0;
}

/*******************************************************************************
 * Function Name  : CubResetVoxel
 * Description    : Resets one cube voxel (LED)
 * Input          : Voxel coordinates
 * Return         : opCode
 *******************************************************************************/
uint8_t CubeResetVoxel(uint8_t x, uint8_t y, uint8_t z)
{
	if(x > 7 || y > 7 || z > 7) return 2;				 // invalid parameters
	if((image[8 * z + 7 - y] & (1 << x)) == 0) return 1; // voxel is already reset

	image[8 * z + 7 - y] &= ~(1 << x);
	return 0;
}

/*******************************************************************************
 * Function Name  : CubeSetPlane
 * Description    : Set cube plane (XPLANE, YPLANE, ZPLANE)
 * Input          : plane (XPLANE, YPLANE, ZPLANE) to Set all leds
 *                : plane number (0-7)
 *******************************************************************************/
void CubeSetPlane(uint8_t plane, uint8_t number)
{
	if(plane == XPLANE)
	{
		for(uint8_t i = 0; i < 8; i++)
			for(uint8_t n = 0; n < 8; n++)
				image[8 * i + n] |= (1 << number);
	}

	if(plane == YPLANE)
	{
		for(uint8_t i = 0; i < 8; i++)
			image[8 * i + 7 - number] = 0xFF;
	}

	if(plane == ZPLANE)
	{
		for(uint8_t i = 0; i < 8; i++)
			image[8 * number + i] = 0xFF;
	}
}

/*******************************************************************************
 * Function Name  : CubeResetPlane
 * Description    : Reset cube plane (XPLANE, YPLANE, ZPLANE)
 * Input          : plane (XPLANE, YPLANE, ZPLANE) to Reset all leds
 *                : plane number (0-7)
 *******************************************************************************/
void CubeResetPlane(uint8_t plane, uint8_t number)
{
	if(plane == XPLANE)
	{
		for(uint8_t i = 0; i < 8; i++)
			for(uint8_t n = 0; n < 8; n++)
				image[8 * i + n] &= ~(1 << number);
	}

	if(plane == YPLANE)
	{
		for(uint8_t i = 0; i < 8; i++)
			image[8 * i + 7 - number] = 0;
	}

	if(plane == ZPLANE)
	{
		for(uint8_t i = 0; i < 8; i++)
			image[8 * number + i] = 0;
	}
}

/*******************************************************************************
 * Function Name  : CubeSetLine
 * Description    : Sets line in the cube
 * Input          : plane (XPLANE, YPLANE, ZPLANE) to set leds line
 *                : leds line coordinates
 *******************************************************************************/
void CubeSetLine(uint8_t plane, uint8_t x, uint8_t y)
{
	if(plane == XPLANE)
	{
		for(uint8_t i = 0; i < 8; i++)
			image[8 * y + 7 - x] |= (1 << i);
	}

	if(plane == YPLANE)
	{
		for(uint8_t i = 0; i < 8; i++)
			image[8 * y + i] |= (1 << (x));
	}

	if(plane == ZPLANE)
	{
		for(uint8_t i = 0; i < 8; i++)
			image[8 * i + y] |= (1 << (x));
	}
}

/*******************************************************************************
 * Function Name  : CubeResetLine
 * Description    : Resets line in the cube
 * Input          : plane (XPLANE, YPLANE, ZPLANE) to reset leds line
 *                : leds line coordinates
 *******************************************************************************/
void CubeResetLine(uint8_t plane, uint8_t x, uint8_t y)
{
	if(plane == XPLANE)
	{
		for(uint8_t i = 0; i < 8; i++)
			image[8 * y + 7 - x] &= ~(1 << i);
	}

	if(plane == YPLANE)
	{
		for(uint8_t i = 0; i < 8; i++)
			image[8 * y + i] &= ~(1 << (x));
	}

	if(plane == ZPLANE)
	{
		for(uint8_t i = 0; i < 8; i++)
			image[8 * i + y] &= ~(1 << (x));
	}
}

/*******************************************************************************
 * Function Name  : CubeGetVoxel
 * Description    : get Voxel state
 * Input          : voxel coordinates
 * Return         : voxel state
 *******************************************************************************/
uint8_t CubeGetVoxel(uint8_t x, uint8_t y, uint8_t z)
{
	if(image[8 * z + 7 - y] & (1 << x))
		return 1;
	else
		return 0;
}

/*******************************************************************************
 * Function Name  : MatrixHAL
 * Description    : Draws the matrix by multiplexing 5 "lines"
 * Input          : pointer to matrix_data massive
 *                : pointer to buttons bit state massive
 *******************************************************************************/
void MatrixHAL(uint8_t *pMatrix, uint8_t *pButtons)
{
	const uint16_t adressPIN[5] = {2, 8, 15, 13, 14};	  // A-A-B-B-B   PORTS
	const uint16_t dataPIN[7] = {6, 2, 0, 12, 1, 10, 11}; // A-B-B-B-B-B PORTS

#define GPIO_AllAdrA ((uint32_t)0x0104)
#define GPIO_AllAdrB ((uint32_t)0xE000)
#define GPIO_AllDatA ((uint32_t)0x1C07)
#define GPIO_AllDatB ((uint32_t)0x1C00)

	static uint8_t i = 0;
	if(++i == 5) i = 0;

	GPIO_ResetBits(GPIOA, GPIO_AllDatA);
	GPIO_ResetBits(GPIOB, GPIO_AllDatB);
	GPIO_ResetBits(GPIOA, GPIO_AllAdrA);
	GPIO_ResetBits(GPIOB, GPIO_AllAdrB);

	// Buttons read routine
	ButtonsRead(pButtons);

	// Draw one line of matrix display
	GPIO_SetBits(GPIOA, GPIO_AllAdrA);
	GPIO_SetBits(GPIOB, GPIO_AllAdrB);
	(i > 1) ? GPIO_ResetBits(GPIOB, (1 << adressPIN[i])) : GPIO_ResetBits(GPIOA, (1 << adressPIN[i]));

	for(uint8_t k = 0; k < 7; k++)
	{
		uint8_t j = k; // 6-k;
		if(*(pMatrix + k) & (1 << i))
			(j) ? GPIO_SetBits(GPIOB, (1 << dataPIN[j])) : GPIO_SetBits(GPIOA, (1 << dataPIN[j]));
		else
			(j) ? GPIO_ResetBits(GPIOB, (1 << dataPIN[j])) : GPIO_ResetBits(GPIOA, (1 << dataPIN[j]));
	}
}

/*******************************************************************************
 * Function Name  : ButtonsRead
 * Description    : Read the buttons state
 * Input          : pointer to buttons bit massive
 *******************************************************************************/
void ButtonsRead(uint8_t *pButtons)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	if(BitIsReset(GPIOA->IDR, 2))
	{
		if(BitIsReset(buttonsState, 3))
		{
			BitSet(buttonsState, 3);
			BitSet(*pButtons, 3);
		}
		else
		{
			BitReset(*pButtons, 3);
		}
	}
	else
	{
		BitReset(buttonsState, 3);
		BitReset(*pButtons, 3);
	}

	if(BitIsReset(GPIOA->IDR, 8))
	{
		if(BitIsReset(buttonsState, 2))
		{
			BitSet(buttonsState, 2);
			BitSet(*pButtons, 2);
		}
		else
		{
			BitReset(*pButtons, 2);
		}
	}
	else
	{
		BitReset(buttonsState, 2);
		BitReset(*pButtons, 2);
	}

	if(BitIsReset(GPIOB->IDR, 15))
	{
		if(BitIsReset(buttonsState, 1))
		{
			BitSet(buttonsState, 1);
			BitSet(*pButtons, 1);
		}
		else
		{
			BitReset(*pButtons, 1);
		}
	}
	else
	{
		BitReset(buttonsState, 1);
		BitReset(*pButtons, 1);
	}

	if(BitIsReset(GPIOB->IDR, 14))
	{
		if(BitIsReset(buttonsState, 0))
		{
			BitSet(buttonsState, 0);
			BitSet(*pButtons, 0);
		}
		else
		{
			BitReset(*pButtons, 0);
		}
	}
	else
	{
		BitReset(buttonsState, 0);
		BitReset(*pButtons, 0);
	}

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/*******************************************************************************
 * Function Name  : Matrix2Chars
 * Description    : Displays two chars on matrix
 * Input          : pointer (2x char) to display
 * 				 : pointer to matrix massive
 *******************************************************************************/
void Matrix2Chars(char *pText, uint8_t *pMatrix)
{
	flagShift = 0;
	matrPos = 0;

	*(pMatrix + 0) = font3x5[3 * (*pText - 32) + 0];
	*(pMatrix + 1) = font3x5[3 * (*pText - 32) + 1];
	*(pMatrix + 2) = font3x5[3 * (*pText - 32) + 2];
	*(pMatrix + 3) = 0;
	if(*(pText + 1) == '\0')
	{
		*(pMatrix + 4) = *(pMatrix + 5) = *(pMatrix + 6) = 0;
		return;
	}
	*(pMatrix + 4) = font3x5[3 * (*(pText + 1) - 32) + 0];
	*(pMatrix + 5) = font3x5[3 * (*(pText + 1) - 32) + 1];
	*(pMatrix + 6) = font3x5[3 * (*(pText + 1) - 32) + 2];
}

/*******************************************************************************
 * Function Name  : MatrixString
 * Description    : Starts "running string" on matrix
 * Input          : pointer (string) to display
 * 				 : pointer to matrix massive
 *******************************************************************************/
void MatrixString(char *pText, uint8_t *pMatrix)
{
	if(!matrPos)
	{
		matrPos = false;
		pText2 = pText;
		pMatrix2 = pMatrix;
		flagShift = true;
		MatrixShift();
	}
}

/*******************************************************************************
 * Function Name  : MatrixShift
 * Description    : Makes "running string" on matrix
 *******************************************************************************/
void MatrixShift()
{
	// Do some magic shifting text operations
	uint16_t len = strlen(pText2);
	for(uint8_t i = 0; i < 7; i++)
	{
		if(!((matrPos + i + 1) % 4) || (matrPos + i + 1) > len * 4)
			*(pMatrix2 + i) = 0;
		else
			*(pMatrix2 + i) = font3x5[3 * (*(pText2 + (matrPos + i) / 4) - 32) + (matrPos + i) % 4];
	}

	// End reached?
	if(((++matrPos /*+7*/) / 4) >= strlen(pText2))
	{
		flagShift = false;
		matrPos = 0;
	}

	// How long text frame will be displayed
	if(strlen(pText2) > 8)
		frameTime = TIMING * 2 / 3;
	else
		frameTime = TIMING;
}
