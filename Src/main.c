/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_spi.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_usart.h"
#include "misc.h"
#include "core_cm3.h"

#include "hw_config.h"
#include "usb_lib.h"
#include "usb_pwr.h"
#include "usb_desc.h"

#include "interface.h"
#include "string.h"
#include "modes.h"

#include "debug.h"
#include "random.h"

#include <stdbool.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
bool flagShift = false;
bool flagCubeShift = false;
volatile uint16_t frameTime = 0, frameCubeTime = 0, modesCounter = 0;
volatile uint32_t delay = 0;

uint8_t buttons;
uint8_t USBmode = STANDALONE; //PC sets cube working mode

uint8_t Matrix[7] = /*{255,255,255,255,255,255,255};*/
{ 1, 1, 1, 1, 0, 255, 0 };

uint8_t GOLOrNot = 0;

//cube update time - 178.5hz
typedef void (*pToVoid)(void);
uint8_t modeCnt = 0;

/* Extern variables ----------------------------------------------------------*/
//extern uint8_t buttonsState;
extern uint8_t image[64];
extern uint32_t DebugDecrease;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
 * Function Name  : __delay_ms.
 * Description    : Delay the code on the N cycles of SysTick Timer.
 * Input          : N Delay Cycles.
 *******************************************************************************/
void __delay_ms(volatile uint32_t nTime)
{
	delay = nTime;
	while(delay);
}

/*******************************************************************************
 * Function Name  : SysTick_Handler.
 * Description    : Handles SysTick Timer Interrupts every 1ms.
 *******************************************************************************/
void SysTick_Handler(void)
{
	if(delay)
		delay--;
	//if( debounce ) debounce--;

	if(flagShift && frameTime)
		frameTime--;
	if(flagShift && !frameTime)
		MatrixShift();

	if(flagCubeShift && frameCubeTime)
		frameCubeTime--;
	if(flagCubeShift && !frameCubeTime)
		DrawStringShift();
	if(modesCounter)
		modesCounter--;

	if(bDeviceState == CONFIGURED)
		CUBECheckState();
	if(DebugDecrease < 32000)
		DebugDecrease++;
}

/*******************************************************************************
 * Function Name  : main
 * Description    : Main routine
 *******************************************************************************/
int main(void)
{
	Initialization();

	//RainSetMode(Y, DOWN);

	while(1)
	{

#if 1
		if(bDeviceState == CONFIGURED)
		{
			CUBECheckState();

			/*if(buttonsState& (1<<0)) CUBECheckState();
			 if(buttonsState& (1<<1)) CUBECheckState2();*/
		}
		//else for(uint8_t i=0; i<7; i++) Matrix[i] = 1;
#endif

		//if((buttons & (1<<0))) {for(uint8_t i=0; i<7; i++) Matrix[i] = 1; for(uint8_t i=3; i<7; i++) Matrix[i] = 0;}

		//MatrixString(&text_plus, &Matrix);

		//Matrix2Char(&text, &Matrix);
		/*if((buttons & (1<<0))) {MatrixString(&text, &Matrix);}
		 if((buttons & (1<<1))) {Matrix2Chars(text, Matrix);}
		 if((buttons & (1<<2))) {MatrixString(text2, Matrix); }
		 if((buttons & (1<<3))) {MatrixString("HAPPY NEW YEAR !!!", Matrix);}*/

		//if( buttons & (1<<3) ) {CubeReset(); t=0; f=0;}
		/*	if( buttons & (1<<2) ) {t++;ch[0]++;}
		 if( buttons & (1<<1) ) {t--;ch[0]--;}*/

		//Matrix2Chars(&ch, &Matrix);

		void (*modeArray[])(void) =
				{	RandomPixel, RandomPixelAccumulate, Rain, TrueSnake,
					/*PlaneBoing,*/MoveVoxelsAlongZ, OutlineBox, OutlineRandomBoxes,
					RandMidwayDest, Ripples, SideWaves, Firework, RandomInvert, SineLines,
					/*SphereMove,*/WormQqueeze, /*CharSpin,*//*effect_blinky2,*/RandSelParallel/*,
					 effect_loadbar*/};

		pToVoid pMode;

		//pMode = RandomPixel;
		//pMode = RandomPixelAccumulate;
		//pMode = Rain;
		//pMode = TrueSnake;

		//pMode = Outline;
		//pMode = PlaneBoing;
		//pMode = MoveVoxelsAlongZ;
		//pMode = OutlineBox;
		//pMode = OutlineRandomBoxes;
		//pMode = RandMidwayDest;
		//pMode = Ripples;
		//pMode = SideWaves;
		//pMode = Firework;
		//pMode = RandomInvert;
		//pMode = SineLines;
		//pMode = SphereMove;

		//pMode = effect_wormsqueeze;
		//pMode = CharSpin;
		//pMode = effect_blinky2;
		//pMode = RandSelParallel;
		//pMode = effect_loadbar;
		pMode = GOL_Begin;
		//spectroanalyzer

		//FUCKING stairs
		//pMode = Stairs;

		//const pToVoid ModesOfWork[15] = {PlaneBoing, RandomPixel, Rain, TrueSnake, RandomPixel, RandomPixelAccumulate, PlaneBoing};

		if((USBmode == STANDALONE) || (USBmode == PC_MONITORING))
		{
			if(GOLOrNot)
			{
				if(flagCubeShift == false) {(*pMode)();}
			}
			else
			{
				modeCnt = GetRandomBig() % (sizeof(modeArray) / sizeof(modeArray[0]));

				char numStr[10];
				itoa_(modeCnt, numStr);
				Matrix2Chars(numStr, Matrix);
				BitWrite(GOLOrNot, Matrix[3], 4);

				CubeReset();
				if(!flagCubeShift)
				{
					(*modeArray[modeCnt])();
				}
			}
		}
	}
}


/**
 *
 */
void TIM2_IRQHandler()
{
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
	{
		static uint8_t ItCounter = 0;
		if(++ItCounter >= 8)
		{
			ItCounter = 0;
			MatrixHAL(Matrix, &buttons);

			if(buttons & (1 << 0))
			{
				NVIC_SystemReset();
			}
			if(buttons & (1 << 1))
			{
				DrawString("A B C D E F G H I", STRING_TWO_FACES);
			}
			if(buttons & (1 << 2))
			{
				GOLOrNot ^= 1;
				BitWrite(GOLOrNot, Matrix[3], 4);
			}
			if(buttons & (1 << 3))
			{
			}
		}

		CubeHAL();
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}
