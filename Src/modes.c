/* Includes ------------------------------------------------------------------*/
//#include "stm32f10x.h"
#include "modes.h"
#include "debug.h"
#include "string.h"
//#include "math.h"
#include "debug.h"
#include "hw_config.h"
#include <stdbool.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define TIMING 200

#define SNAKE_DELAY 120 //lesser - bigger frequency
#define SNAKE_MAX_LEN 16
#define SNAKE_MIN_LEN 1
#define SNAKE_SHOW_TARGET

#if SNAKE_MIN_LEN<1 || SNAKE_MIN_LEN > SNAKE_MAX_LEN || SNAKE_MAX_LEN>16
#error Invalid SNAKE_MIN_LEN or SNAKE_MAX_LEN
#endif

enum VoxelMove
{
	EQUAL, DECREASE, INCREASE
};

const uint8_t bitmaps[6][8] =
{
{ 0xc3, 0xc3, 0x00, 0x18, 0x18, 0x81, 0xff, 0x7e }, // Smile (small)
		{ 0x3c, 0x42, 0x81, 0x81, 0xc3, 0x24, 0xa5, 0xe7 }, // Omega
		{ 0x00, 0x04, 0x06, 0xff, 0xff, 0x06, 0x04, 0x00 }, // Arrow
		{ 0x81, 0x42, 0x24, 0x18, 0x18, 0x24, 0x42, 0x81 }, // X

		};

const uint16_t sinTable[91] =
{ 0, 18, 36, 54, 71, 89, 107, 125, 143, 160, 178, 195, 213, 230, 248, 265, 282,
		299, 316, 333, 350, 367, 384, 400, 416, 433, 449, 465, 481, 496, 512,
		527, 543, 558, 573, 587, 602, 616, 630, 644, 658, 672, 685, 698, 711,
		724, 737, 749, 761, 773, 784, 796, 807, 818, 828, 839, 849, 859, 868,
		878, 887, 896, 904, 912, 920, 928, 935, 943, 949, 956, 962, 968, 974,
		979, 984, 989, 994, 998, 1002, 1005, 1008, 1011, 1014, 1016, 1018, 1020,
		1022, 1023, 1023, 1024, 1024

};

/* Private macro -------------------------------------------------------------*/
#define abs(x)  ( x<0 ) ? -x : x

/* Private variables ---------------------------------------------------------*/
char *pString;
uint16_t cubeStrPos = 0;
uint8_t stringMode = 0;

//snake
struct SnakeType
{
	uint8_t voxel[3];
	uint8_t complete[3];
} Snake[SNAKE_MAX_LEN + 1];

uint8_t target[3];
uint8_t snakeLen = SNAKE_MIN_LEN;

uint8_t rainDir = DOWN, rainAxis = Z;

uint16_t iterations;

/* Extern variables ----------------------------------------------------------*/
//extern uint8_t Matrix[7];
extern uint8_t image[64];
extern bool flagCubeShift;
extern uint16_t modesCounter;
extern volatile uint16_t frameCubeTime;

extern const char font3x5[];
extern const char font5x8[];

extern uint8_t Matrix[7];

extern uint8_t USBmode;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
 * Function Name  : ModesDelay
 * Description    : delay code execution
 *******************************************************************************/
void ModesDelay(uint16_t delay)
{
	if(USBmode == PC_CONTROLS)
		return;
	//return;
	modesCounter = delay;
	while(modesCounter)
		;
}

/*******************************************************************************
 * Function Name  : RandomPixel
 * Description    : Draws random pixel without accumulation
 *******************************************************************************/
void RandomPixel()
{
	for(iterations = 0; iterations < 70; iterations++)
	{
		CubeReset();
		CubeSetVoxel(GetRandom() % 8, GetRandom() % 8, GetRandom() % 8);

		ModesDelay(50);
	}
}

/*******************************************************************************
 * Function Name  : RandomPixelAccumulate
 * Description    :
 *******************************************************************************/
void RandomPixelAccumulate()
{
	for(iterations = 0; iterations < 512; iterations++)
	{
		static uint16_t massR[512];
		static uint16_t accFlag = 512;

		if(accFlag >= 512)
		{
			CubeReset();

			for(uint16_t i = 0; i < 512; i++)
				massR[i] = i;

			for(uint16_t i = 0; i < 512; i++)
			{
				uint8_t x[4];
				for(uint8_t i = 0; i < 4; i++)
					x[i] = GetRandom();

				uint16_t j = i
						+ (x[0] * 1000 + x[1] * 100 + x[2] * 10 + x[3])
								/ (9999 / (512 - i) + 1);
				j = j % 512;
				uint16_t t = massR[j];
				massR[j] = massR[i];
				massR[i] = t;
			}

			accFlag = 0;
		}
		else
			accFlag++;

		CubeSetVoxel(massR[accFlag] / 64, massR[accFlag] / 8 % 8,
				massR[accFlag] % 8);

#define START_SPEED 50
#define END_SPEED 1

		ModesDelay(
				START_SPEED - (accFlag / (512 / (START_SPEED - END_SPEED)))
						+ 1);
	}
}

/*******************************************************************************
 * Function Name  : RandomInvert
 * Description    :
 *******************************************************************************/
void RandomInvert()
{
	for(iterations = 0; iterations < 8 * 1000 / 15; iterations++)
	{
		uint8_t x = GetRandom() % 8, y = GetRandom() % 8, z = GetRandom() % 8;

		(CubeGetVoxel(x, y, z) == 1) ?
				CubeResetVoxel(x, y, z) : CubeSetVoxel(x, y, z);

		ModesDelay(25);
	}
}

/*******************************************************************************
 * Function Name  : OutlineBox
 * Description    :
 *******************************************************************************/
void OutlineBox()
{
	for(iterations = 0; iterations < 2; iterations++)
	{

#if 0
#define OUTLINE_MODE 1
#else
		static uint8_t OUTLINE_MODE = 0;
		OUTLINE_MODE++;
#endif

//000->777
		for(uint8_t i = 0; i < 8; i++)
		{
			CubeReset();
			Outline(0, 0, 0, i, i, i, OUTLINE_MODE % 2);
			ModesDelay(140);
		}

		for(uint8_t i = 0; i < 8; i++)
		{
			CubeReset();
			Outline(7, 7, 7, i, i, i, OUTLINE_MODE % 2);
			ModesDelay(140);
		}
//700->077
		for(uint8_t i = 0; i < 8; i++)
		{
			CubeReset();
			Outline(7, 0, 0, 7 - i, i, i, OUTLINE_MODE % 2);
			ModesDelay(140);
		}

		for(uint8_t i = 0; i < 8; i++)
		{
			CubeReset();
			Outline(0, 7, 7, 7 - i, i, i, OUTLINE_MODE % 2);
			ModesDelay(140);
		}
//from center to outside
		for(uint8_t i = 0; i < 4; i++)
		{
			CubeReset();
			Outline(3 - i, 3 - i, 3 - i, 4 + i, 4 + i, 4 + i, OUTLINE_MODE % 2);
			ModesDelay(300);
		}

		for(uint8_t i = 0; i < 4; i++)
		{
			CubeReset();
			Outline(i, i, i, 7 - i, 7 - i, 7 - i, OUTLINE_MODE % 2);
			ModesDelay(300);
		}
	}
}

/*******************************************************************************
 * Function Name  : OutlineRandomBoxes
 * Description    :
 *******************************************************************************/
void OutlineRandomBoxes()
{
	for(iterations = 0; iterations < 100; iterations++)
	{
		static uint8_t flagReached = 0x3F;
		static uint8_t target[6], cur[6];

		if(flagReached == 0x3F) //all 6 bits set to 1
		{
			for(uint8_t i = 0; i < 3; i++)
			{
				target[i] = target[i + 3] = 0;
				while(abs(target[i] - target[i+3]) < 3)
				{
					target[i] = GetRandom() % 8;
					target[i + 3] = GetRandom() % 8;
				}
			}

			flagReached = 0;
			ModesDelay(800);
		}

		for(uint8_t i = 0; i < 6; i++)
		{
			if(BitIsReset(flagReached, i))
			{
				if(cur[i] < target[i])
					cur[i]++;
				else if(cur[i] > target[i])
					cur[i]--;
				else
					BitSet(flagReached, i);
			}
		}

		CubeReset();
		Outline(cur[0], cur[1], cur[2], cur[3], cur[4], cur[5], 0);

		ModesDelay(50);
	}
}

/*******************************************************************************
 * Function Name  : Outline
 * Description    :
 *******************************************************************************/
void Outline(uint8_t x1, uint8_t y1, uint8_t z1, uint8_t x2, uint8_t y2,
		uint8_t z2, uint8_t outlineMode)
{
	uint8_t origX = x2, origY = y2, origZ = z2;
	signed char xflg = 0, yflg = 0, zflg = 0;

	if(x1 < x2)
	{
		xflg = +1;
		x2++;
	}
	else
	{
		xflg = -1;
		x2--;
	}

	if(y1 < y2)
	{
		yflg = +1;
		y2++;
	}
	else
	{
		yflg = -1;
		y2--;
	}

	if(z1 < z2)
	{
		zflg = +1;
		z2++;
	}
	else
	{
		zflg = -1;
		z2--;
	}

	for(uint8_t z = z1; z != z2; z += zflg)
		for(uint8_t y = y1; y != y2; y += yflg)
			for(uint8_t x = x1; x != x2; x += xflg)
			{
				if(!outlineMode)
				{
					/* only outline */
					if(((x == x1 || x == origX) && (y == y1 || y == origY))
							|| ((x == x1 || x == origX)
									&& (z == z1 || z == origZ))
							|| ((y == y1 || y == origY)
									&& (z == z1 || z == origZ)))
						CubeSetVoxel(x, y, z);
				}
				else
					/* full volume */
					CubeSetVoxel(x, y, z);
			}
}

/*******************************************************************************
 * Function Name  : RandMidwayDest
 * Description    :
 *******************************************************************************/
void RandMidwayDest()
{
	for(iterations = 0; iterations < 10; iterations++)
	{
#define SMALL_DELAY 100
#define LONG_DELAY 	500

		uint8_t invert = 0, axis;
		invert = GetRandom();
		axis = GetRandom() % 3;

		uint8_t position[64] =
		{ 0 }, destination[64];

		for(uint8_t i = 0; i < 64; i++)
			destination[i] = GetRandom() % 8;

		/* Loop 8 times to allow destination 7 to reach all the way */
		RandMidwayDest_Draw(axis, position, destination, invert % 2);

		/* Set all destinations to 7 (opposite from the side they started out) */
		for(uint8_t i = 0; i < 64; i++)
			destination[i] = 7;

		/* Suspend the positions in mid-air for a while */
		ModesDelay(LONG_DELAY);

		/* Then do the same thing one more time */
		RandMidwayDest_Draw(axis, position, destination, invert % 2);
	}
}

/*******************************************************************************
 * Function Name  :
 * Description    :
 *******************************************************************************/
void RandMidwayDest_Draw(uint8_t axis, uint8_t position[64],
		uint8_t destination[64], uint8_t invert)
{
	for(uint8_t i = 0; i < 8; i++)
	{
		// For every iteration, move all position one step closer to their destination
		for(uint8_t px = 0; px < 64; px++)
		{
			if(position[px] < destination[px])
				position[px]++;
		}

		// Draw the positions and take a nap
		CubeReset();

		for(uint8_t x = 0; x < 8; x++)
		{
			for(uint8_t y = 0; y < 8; y++)
			{
				int p;

				if(invert)
					p = (7 - position[(x * 8) + y]);
				else
					p = position[(x * 8) + y];

				if(axis == Z)
					CubeSetVoxel(x, y, p);
				if(axis == Y)
					CubeSetVoxel(x, p, y);
				if(axis == X)
					CubeSetVoxel(p, y, x);
			}
		}

		ModesDelay(SMALL_DELAY);
	}
}

/*******************************************************************************
 * Function Name  : Ripples
 * Description    :
 *******************************************************************************/
void Ripples()
{
	for(signed int i = 0; i <= 32; i++)
	{
		CubeReset();

		for(uint8_t x = 0; x < 8; x++) //2ms
		{
			for(uint8_t y = 0; y < 8; y++)
			{
				uint8_t height = (uint8_t)(
						4
								+ sin_(
										Distance2D(3.5, 3.5, x, y) * 35
												+ (i - 16) * 33) * 3.5);
				CubeSetVoxel(x, y, height);
			}
		}
		ModesDelay(120); //120
	}
}

/*******************************************************************************
 * Function Name  : Distance2D
 * Description    :
 *******************************************************************************/
float Distance2D(float x1, float y1, float x2, float y2)
{
	return (float) sqrt_((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

/*******************************************************************************
 * Function Name  : Distance3D
 * Description    :
 *******************************************************************************/
float Distance3D(float x1, float y1, float z1, float x2, float y2, float z2)
{
	return sqrt_(
			(x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2)
					+ (z1 - z2) * (z1 - z2));
}

#if 0
float sinReal(float x)
{
	signed char sign = 1;
	if(x<-90)
	{
		while(x<-90)
		{	x += 180; sign=-sign;}
	}
	else if(x>90)
	{
		while(x>90)
		{	x -= 180; sign=-sign;}
	}

	x = x*0.0174532925;
	float sine = x;

#define LIMIT 7
	for(double i=2; i<=LIMIT; i++)
	sine+= pow_(-1,i-1)*pow_(x,2*i-1)/factorial(2*i-1);

	return ((float)sign)*sine;
}
#endif

/*******************************************************************************
 * Function Name  :
 * Description    :
 *******************************************************************************/
float sin_(float x)
{
	float sign = 1, sine;

	if(x < 0)
	{
		while(x < 0)
		{
			x += 180;
			sign = -sign;
		}
	}
	else if(x > 180)
	{
		while(x > 180)
		{
			x -= 180;
			sign = -sign;
		}
	}

	if(x < 90)
		sine = sinTable[(uint16_t) x];
	else
		sine = sinTable[(uint16_t)(180 - x)];

	return sign * sine / 1024;
}

/*******************************************************************************
 * Function Name  :
 * Description    :
 *******************************************************************************/
float cos_(float x)
{
	x -= 90;
	float sign = 1, sine;

	if(x < 0)
	{
		while(x < 0)
		{
			x += 180;
			sign = -sign;
		}
	}
	else if(x > 180)
	{
		while(x > 180)
		{
			x -= 180;
			sign = -sign;
		}
	}

	if(x < 90)
		sine = sinTable[(uint16_t) x];
	else
		sine = sinTable[(uint16_t)(180 - x)];

	return -sign * sine / 1024;
}

/*******************************************************************************
 * Function Name  :
 * Description    :
 *******************************************************************************/
float tan_(float x)
{
	return sin_(x) / cos_(x);
}

/*******************************************************************************
 * Function Name  :
 * Description    :
 *******************************************************************************/
float factorial(double x)
{
	double result = 1;

	for(int i = 1; i <= x; i++)
		result *= i;

	return result;
}

/*******************************************************************************
 * Function Name  :
 * Description    :
 *******************************************************************************/
float pow_(float x, float y)
{
	double result = 1;

	for(int i = 0; i < y; i++)
		result *= x;

	return result;
}

/*******************************************************************************
 * Function Name  :
 * Description    :
 *******************************************************************************/
float sqrt_(float x)
{
	union
	{
		int i;
		float x;
	} u;

	u.x = x;
	u.i = (1 << 29) + (u.i >> 1) - (1 << 22);
	return u.x;
}

/*******************************************************************************
 * Function Name  :
 * Description    :
 *******************************************************************************/
void SideWaves() //wave from one side to another is rotating
{
	for(uint16_t i = 0; i < 184; i++)
	{
		CubeReset();

		for(uint8_t x = 0; x < 8; x++) //2ms
		{
			for(uint8_t y = 0; y < 8; y++)
			{
				float distance = Distance2D(3.5 + sin_(i * 2) * 4,
						3.5 + cos_(i * 2) * 4, x, y);

				uint8_t height = (uint8_t)(
						4 + sin_(distance * 35 + (i - 16) * 33) * 3.5);
				CubeSetVoxel(x, y, (int) height);
			}
		}

		ModesDelay(100);
	}
}

/*******************************************************************************
 * Function Name  : OutlineRandomBoxes
 * Description    :
 *******************************************************************************/
void SineLines()
{
	int i, x;

	float left, right, sine_base, x_dividor, ripple_height;

	for(i = 0; i < 1000; i++)
	{
		for(x = 0; x < 8; x++)
		{
			x_dividor = 2 + sin_((float) i / 100 * 57.29) + 1;
			ripple_height = 3 + (sin_((float) i / 200 * 57.29) + 1) * 6;

			sine_base = (float) i / 40 + (float) x / x_dividor;

			left = 4 + sin_(sine_base * 57.29) * ripple_height;
			right = 4 + cos_(sine_base * 57.29) * ripple_height;
			right = 7 - left;

			//printf("%i %i \n", (int) left, (int) right);

			line_3d(0 - 3, x, (int) left, 7 + 3, x, (int) right);
			//line_3d((int) right, 7, x);
		}

		ModesDelay(10);
		CubeReset();
	}
}

/*******************************************************************************
 * Function Name  : SphereMove
 * Description    :
 *******************************************************************************/
void SphereMove()
{
	CubeReset();

	float origin_x, origin_y, origin_z, distance, diameter;

	origin_x = 0;
	origin_y = 3.5;
	origin_z = 3.5;

	diameter = 3;

	int x, y, z, i;

	for(i = 0; i < 1500; i++)
	{
		origin_x = 3.5 + sin_((float) i / 50 * 57.29) * 2.5;
		origin_y = 3.5 + cos_((float) i / 50 * 57.29) * 2.5;
		origin_z = 3.5 + cos_((float) i / 30 * 57.29) * 2;

		diameter = 2 + sin_((float) i / 150 * 57.29);

		for(x = 0; x < 8; x++)
		{
			for(y = 0; y < 8; y++)
			{
				for(z = 0; z < 8; z++)
				{
					distance = Distance3D(x, y, z, origin_x, origin_y,
							origin_z);

					if(distance > diameter && distance < diameter + 1)
					{
						CubeSetVoxel(x, y, z);
					}
				}
			}
		}

		ModesDelay(2);
		CubeReset();
	}
}

/*******************************************************************************
 * Function Name  : Stairs
 * Description    :
 *******************************************************************************/
void Stairs()
{
	static uint8_t invert = 1;
	static uint8_t fill = 0;

	for(uint8_t x = 0; x < 16; x++)
	{
		uint8_t xx;

		if(invert)
			xx = 15 - x;
		else
			xx = x;

		for(uint8_t y = 0; y <= x; y++)
		{
			//uint8_t yy;
			if(xx)
				xx--;
			//if(xx>=16) xx=15;

			/*if(invert) yy=15-y;
			 else yy=y;*/

			if(xx < 8 && y < 8)
			{
				image[xx * 8 + y] = 0xFF;
			}
		}

		DebugSendNum(10000);

		ModesDelay(100);

		if(fill)
			CubeReset();
		//CubeReset();

	}
	DebugSendNum(9999);

	invert ^= (1 << 0);
	if(invert)
		fill ^= (1 << 0);
	CubeReset();
}

/*******************************************************************************
 * Function Name  : WormQqueeze
 * Description    :
 *******************************************************************************/
void WormQqueeze()
{
	int size = 2; //1 2
	int axis = Z;
	int direction = -1; // 1 -1

	int origin = (direction == -1) ? 7 : 0;
	uint8_t wormSize = 8 - (size - 1);

	uint8_t x = GetRandom() % wormSize;
	uint8_t y = GetRandom() % wormSize;

	for(uint16_t i = 0; i < 100; i++)
	{
		signed char dx = GetRandom() % 3 - 1;
		signed char dy = GetRandom() % 3 - 1;

		if((x + dx) > 0 && (x + dx) < wormSize)
			x += dx;
		if((y + dy) > 0 && (y + dy) < wormSize)
			y += dy;

		Shift(axis, direction);

		for(uint8_t j = 0; j < size; j++)
		{
			for(uint8_t k = 0; k < size; k++)
			{
				if(axis == Z)
					CubeSetVoxel(x + j, y + k, origin);
				if(axis == Y)
					CubeSetVoxel(x + j, origin, y + k);
				if(axis == X)
					CubeSetVoxel(origin, y + j, x + k);
			}
		}

		ModesDelay(150);
	}
}

/*******************************************************************************
 * Function Name  : CharSpin
 * Description    :
 *******************************************************************************/
void CharSpin()
{
	int count = 2;
	uint8_t bitmap = 2; //0 1 2

	uint8_t dybde[] =
	{ 0, 1, 2, 3, 4, 5, 6, 7, 1, 1, 2, 3, 4, 5, 6, 6, 2, 2, 3, 3, 4, 4, 5, 5, 3,
			3, 3, 3, 4, 4, 4, 4 };
	int d = 0;
	int flip = 0;
	uint8_t off;

	for(uint16_t i = 0; i < count; i++)
	{
		for(uint8_t mode = 0; mode < 4; mode++)
		{
			off = 0;
			flip = 0;
			d = 0;

			// front:
			for(uint8_t s = 0; s < 7; s++)
			{
				if(!flip)
				{
					off++;
					if(off == 4)
					{
						flip = 1;
						off = 0;
					}
				}
				else
					off++;

				for(uint8_t x = 0; x < 8; x++)
				{
					d = 0;

					for(uint8_t y = 0; y < 8; y++)
					{
						if((bitmaps[bitmap][7 - x] >> y) & 0x01)
						{
							if(!flip)
							{
								if(mode % 2 == 0)
									CubeSetVoxel(y, dybde[8 * off + d++], x);
								else
									CubeSetVoxel(dybde[8 * off + d++], 7 - y,
											x);
							}
							else
							{
								if(mode % 2 == 0)
									CubeSetVoxel(y, dybde[31 - 8 * off - d++],
											x);
								else
									CubeSetVoxel(dybde[31 - 8 * off - d++],
											7 - y, x);
							}

						}
						else
							d++;
					}
				}

				ModesDelay(80);
				CubeReset();
			}
		}
	}
}

/*******************************************************************************
 * Function Name  :
 * Description    :
 *******************************************************************************/
void effect_blinky2()
{
	CubeReset();

	int i = 750;
	while(i > 0)
	{
		CubeReset();
		ModesDelay(i);

		CubeSet();
		ModesDelay(100);

		i = i - (15 + (1000 / (i / 10)));
	}

	ModesDelay(1000);

	i = 750;
	while(i > 0)
	{
		CubeReset();
		ModesDelay(751 - i);

		CubeSet();
		ModesDelay(100);

		i = i - (15 + (1000 / (i / 10)));
	}
}

/*******************************************************************************
 * Function Name  :
 * Description    :
 *******************************************************************************/
void RandSelParallel()
{
	/*
	 * 	(AXIS_Z, 0 , 200,1);
	 ModesDelay(1500);
	 (AXIS_Z, 1 , 200,1);
	 ModesDelay(1500);

	 (AXIS_Z, 0 , 200,2);
	 ModesDelay(1500);
	 (AXIS_Z, 1 , 200,2);
	 ModesDelay(1500);

	 (AXIS_Y, 0 , 200,1);
	 ModesDelay(1500);
	 (AXIS_Y, 1 , 200,1);
	 ModesDelay(1500);
	 */

	static uint8_t origin = 0; //0 1
	int delay = 100;
	int mode = 1; //1 2

	for(iterations = 0; iterations < 8; iterations++)
	{

		char axis = GetRandom() % 6;

		int done;
		uint8_t cubePos[64];
		uint8_t pos[64] =
		{ 0 };
		int sent = 0;

		while(1)
		{
			if(mode == 1)
			{
				//uint8_t notdone2 = 1;
				while(/*notdone2 && */sent < 64)
				{
					uint8_t i = GetRandomBig() % 64;
					if(pos[i] == 0)
					{
						sent++;
						pos[i] += 1;
						//notdone2 = 0;
						break;
					}
				}
			}
			else if(mode == 2)
			{
				if(sent < 64)
					pos[sent++] += 1;
			}

			done = 0;
			for(uint8_t i = 0; i < 64; i++)
			{
				if(pos[i] > 0 && pos[i] < 7)
					pos[i] += 1;

				if(pos[i] == 7)
					done++;
			}

			if(done == 64)
				break;

			for(uint8_t i = 0; i < 64; i++)
				cubePos[i] = (origin == 0) ? pos[i] : (7 - pos[i]);

			ModesDelay(delay);
			draw_positions_axis(axis, cubePos, 0);
		}

		origin ^= 1 /*GetRandom()%2*/;
	}
}

/*******************************************************************************
 * Function Name  :
 * Description    :
 *******************************************************************************/
void draw_positions_axis(char axis, uint8_t positions[64], int invert)
{
	CubeReset();

	for(uint8_t x = 0; x < 8; x++)
	{
		for(uint8_t y = 0; y < 8; y++)
		{
			if(axis >= Z)
				CubeSetVoxel(x, y,
						invert ?
								(7 - positions[(x * 8) + y]) :
								positions[(x * 8) + y]);
			if(axis == Y)
				CubeSetVoxel(x,
						invert ?
								(7 - positions[(x * 8) + y]) :
								positions[(x * 8) + y], y);
			if(axis == X)
				CubeSetVoxel(
						invert ?
								(7 - positions[(x * 8) + y]) :
								positions[(x * 8) + y], y, x);
		}
	}
}

/*******************************************************************************
 * Function Name  :
 * Description    :
 *******************************************************************************/
// Light all leds layer by layer,
// then unset layer by layer
void effect_loadbar()
{
	CubeReset();

	uint8_t plane = Z; // X Y Z
	uint8_t direction = 1; //0 not0

	for(uint8_t i = 0; i < 8; i++)
	{
		CubeSetPlane(plane, (direction) ? (7 - i) : i);
		ModesDelay(100);
	}

	ModesDelay(100);

	for(uint8_t i = 0; i < 8; i++)
	{
		CubeResetPlane(plane, (direction) ? (7 - i) : i);
		ModesDelay(100);
	}
}

/*******************************************************************************
 * Function Name  :
 * Description    :
 *******************************************************************************/
void line_3d(int x1, int y1, int z1, int x2, int y2, int z2)
{
	int dx, dy, dz, l, m, n, x_inc, y_inc, z_inc, err_1, err_2, dx2, dy2, dz2;
	int pixel[3];

	pixel[0] = x1;
	pixel[1] = y1;
	pixel[2] = z1;

	dx = x2 - x1;
	dy = y2 - y1;
	dz = z2 - z1;

	x_inc = (dx < 0) ? -1 : 1;
	y_inc = (dy < 0) ? -1 : 1;
	z_inc = (dz < 0) ? -1 : 1;

	l = abs(dx);
	m = abs(dy);
	n = abs(dz);

	dx2 = l << 1;
	dy2 = m << 1;
	dz2 = n << 1;

	if((l >= m) && (l >= n))
	{
		err_1 = dy2 - l;
		err_2 = dz2 - l;

		for(uint8_t i = 0; i < l; i++)
		{
			CubeSetVoxel(pixel[0], pixel[1], pixel[2]);

			if(err_1 > 0)
			{
				pixel[1] += y_inc;
				err_1 -= dx2;
			}

			if(err_2 > 0)
			{
				pixel[2] += z_inc;
				err_2 -= dx2;
			}

			err_1 += dy2;
			err_2 += dz2;
			pixel[0] += x_inc;
		}
	}
	else if((m >= l) && (m >= n))
	{
		err_1 = dx2 - m;
		err_2 = dz2 - m;

		for(uint8_t i = 0; i < m; i++)
		{
			CubeSetVoxel(pixel[0], pixel[1], pixel[2]);

			if(err_1 > 0)
			{
				pixel[0] += x_inc;
				err_1 -= dy2;
			}

			if(err_2 > 0)
			{
				pixel[2] += z_inc;
				err_2 -= dy2;
			}

			err_1 += dx2;
			err_2 += dz2;
			pixel[1] += y_inc;
		}
	}
	else
	{
		err_1 = dy2 - n;
		err_2 = dx2 - n;

		for(uint8_t i = 0; i < n; i++)
		{
			CubeSetVoxel(pixel[0], pixel[1], pixel[2]);

			if(err_1 > 0)
			{
				pixel[1] += y_inc;
				err_1 -= dz2;
			}

			if(err_2 > 0)
			{
				pixel[0] += x_inc;
				err_2 -= dz2;
			}

			err_1 += dy2;
			err_2 += dx2;
			pixel[2] += z_inc;
		}
	}

	CubeSetVoxel(pixel[0], pixel[1], pixel[2]);
}

/*******************************************************************************
 * Function Name  : PlaneBoing
 * Description    :
 *******************************************************************************/
void PlaneBoing()
{
	for(iterations = 0; iterations < 3; iterations++)
	{
		for(uint8_t k = XPLANE; k < ZPLANE + 1; k++)
		{
			for(uint8_t i = 0; i < 8; i++)
			{
				CubeReset();
				CubeSetPlane(k, i);
				ModesDelay(50);
			}
			for(uint8_t i = 7; i != 0; i--)
			{
				CubeReset();
				CubeSetPlane(k, i);
				ModesDelay(50);
			}
		}
	}
}

/*******************************************************************************
 * Function Name  : MoveVoxelsAlongZInit
 * Description    :
 *******************************************************************************/
void MoveVoxelsAlongZInit()
{
	CubeReset();

	for(uint8_t x = 0; x < 8; x++)
		for(uint8_t y = 0; y < 8; y++)
			CubeSetVoxel(x, y, (GetRandom() % 2) * 7);

	ModesDelay(200);
}

/*******************************************************************************
 * Function Name  : MoveVoxelsAlongZ
 * Description    :
 *******************************************************************************/
// For each coordinate along X and Y, a voxel is set either at level 0 or at level 7
// for n iterations, a random voxel is sent to the opposite side of where it was.
void MoveVoxelsAlongZ()
{
	MoveVoxelsAlongZInit();

	for(iterations = 0; iterations < 100; iterations++)
	{
		static uint8_t isInitialized = DISABLE;
		if(isInitialized == DISABLE)
		{
			isInitialized = ENABLE;
			MoveVoxelsAlongZInit();
		}

		static uint8_t lastX = 0, lastY = 0;
		uint8_t x = GetRandom() % 8, y = GetRandom() % 8;

		if(y != lastY && x != lastX)
		{
			MoveVoxelAlongZ(x, y, CubeGetVoxel(x, y, 0) ? 0 : 7);

			lastY = y;
			lastX = x;
			ModesDelay(80);
		}
	}
}

/*******************************************************************************
 * Function Name  : MoveVoxelAlongZ
 * Description    :
 *******************************************************************************/
// Send a voxel flying from one side of the image to the other
// If its at the bottom, send it to the top..
void MoveVoxelAlongZ(uint8_t x, uint8_t y, uint8_t z)
{
	for(uint8_t i = 0; i < 8; i++)
	{
		if(z == 7)
		{
			CubeResetVoxel(x, y, 7 - i + 1);
			CubeSetVoxel(x, y, 7 - i);
		}
		else
		{
			CubeResetVoxel(x, y, i - 1);
			CubeSetVoxel(x, y, i);
		}

		ModesDelay(60);
	}
}

/*******************************************************************************
 * Function Name  : RainSetMode
 * Description    : Set Rain mode
 *******************************************************************************/
void RainSetMode(uint8_t axis, uint8_t dir)
{
	rainAxis = axis;
	rainDir = dir;
}

/*******************************************************************************
 * Function Name  : Rain
 * Description    :
 *******************************************************************************/
void Rain()
{
	for(iterations = 0; iterations < 12000 / 150; iterations++)
	{
		static uint8_t prevPlane[8]; //2-led falling drops
		uint8_t newPoint = 7 * (1 - rainDir);

		switch(rainAxis)
		{
		case X:
			for(uint8_t i = 0; i < 8; i++)
				for(uint8_t j = 0; j < 8; j++)
					BitWrite(BitIsSet(prevPlane[i], j), image[8*i+j], newPoint);

			for(uint8_t i = 0; i < GetRandom() % 3; i++)
				CubeSetVoxel(newPoint, GetRandom() % 8, GetRandom() % 8);

			//A - B = (A xor B) and A
			for(uint8_t i = 0; i < 8; i++)
				for(uint8_t j = 0; j < 8; j++)
					BitWrite(
							((BitIsSet(image[8*i+j], 7) ^ BitIsSet(prevPlane[i], j)) & BitIsSet(image[8*i+j], 7)),
							prevPlane[i], j);
			break;

		case Y:
			for(uint8_t i = 0; i < 8; i++)
				image[8 * i + newPoint] = prevPlane[i];

			for(uint8_t i = 0; i < GetRandom() % 3; i++)
				CubeSetVoxel(GetRandom() % 8, newPoint, GetRandom() % 8);

			//A - B = (A xor B) and A
			for(uint8_t i = 0; i < 8; i++)
				prevPlane[i] = (image[8 * i + newPoint] ^ prevPlane[i])
						& image[8 * i + newPoint];
			break;

		case Z:
			for(uint8_t i = 0; i < 8; i++)
				image[8 * newPoint + i] = prevPlane[i];

			for(uint8_t i = 0; i < GetRandom() % 3; i++)
				CubeSetVoxel(GetRandom() % 8, GetRandom() % 8, newPoint);

			//A - B = (A xor B) and A
			for(uint8_t i = 0; i < 8; i++)
				prevPlane[i] = (image[8 * newPoint + i] ^ prevPlane[i])
						& image[8 * newPoint + i];
			break;
		}

		ModesDelay(150);
		Shift(rainAxis, -(1 - rainDir));
	}
}

/*******************************************************************************
 * Function Name  : Shift
 * Description    :
 *******************************************************************************/
// Shift the entire contents of the image along an axis
// This is great for effects where you want to draw something
// on one side of the image and have it flow towards the other
// side. Like rain flowing down the Z axiz.
void Shift(uint8_t axis, signed char direction)
{
	for(uint8_t i = 0; i < 8; i++)
	{
		uint8_t next = (direction == -1) ? i : (7 - i);

		for(uint8_t x = 0; x < 8; x++)
		{
			for(uint8_t y = 0; y < 8; y++)
			{
				uint8_t prev = (direction == -1) ? (next + 1) : (next - 1);

				if(axis == Z)
					(CubeGetVoxel(x, y, prev)) ?
							CubeSetVoxel(x, y, next) :
							CubeResetVoxel(x, y, next);
				if(axis == Y)
					(CubeGetVoxel(x, prev, y)) ?
							CubeSetVoxel(x, next, y) :
							CubeResetVoxel(x, next, y);
				if(axis == X)
					(CubeGetVoxel(prev, y, x)) ?
							CubeSetVoxel(next, y, x) :
							CubeResetVoxel(next, y, x);
			}
		}
	}

	uint8_t i = (direction == -1) ? 7 : 0;

	for(uint8_t x = 0; x < 8; x++)
	{
		for(uint8_t y = 0; y < 8; y++)
		{
			if(axis == X)
				CubeResetVoxel(i, y, x);
			if(axis == Y)
				CubeResetVoxel(x, i, y);
			if(axis == Z)
				CubeResetVoxel(x, y, i);
		}
	}
}

/*******************************************************************************
 * Function Name  : OutlineRandomBoxes
 * Description    :
 *******************************************************************************/
void Firework()
{
	//const uint8_t n = 20;//50 orig 20 stable
#define n 20

	uint8_t origin_x = 3, origin_y = 3, origin_z = 3;
	signed int rand_y, rand_x, rand_z;

	float slowrate, gravity;

	// Particles and their position, x,y,z and their movement, dx, dy, dz
	float particles[n][6];

	CubeReset();
	for(uint8_t i = 0; i < 7; i++)
	{
		origin_x = GetRandom() % 4 + 2;
		origin_y = GetRandom() % 4 + 2;
		origin_z = GetRandom() % 2 + 5;

		// shoot a particle up in the air
		for(uint8_t e = 0; e < origin_z; e++)
		{
			CubeSetVoxel(origin_x, origin_y, e);
			ModesDelay(50 + 10 * e);
			CubeReset();
		}

		// Fill particle array
		for(uint8_t f = 0; f < n; f++)
		{
			// Position
			particles[f][X] = origin_x;
			particles[f][Y] = origin_y;
			particles[f][Z] = origin_z;

			rand_x = GetRandomBig() % 200 - 100;
			rand_y = GetRandomBig() % 200 - 100;
			rand_z = GetRandomBig() % 200 - 100;

			// Movement
			particles[f][3] = (float) rand_x / 100; // dx
			particles[f][4] = (float) rand_y / 100; // dy
			particles[f][5] = (float) rand_z / 100; // dz
		}

		// explode
		for(uint8_t e = 0; e < 15; e++)
		{
			slowrate = 1.4 + tan_((e) * 4);
			gravity = tan_(e / 3) / 2;

			CubeReset();
			for(uint8_t f = 0; f < n; f++)
			{
				particles[f][X] += particles[f][3] / slowrate;
				particles[f][Y] += particles[f][4] / slowrate;
				particles[f][Z] += particles[f][5] / slowrate;
				particles[f][Z] -= gravity;

				CubeSetVoxel(particles[f][X], particles[f][Y], particles[f][Z]);
			}

			ModesDelay(130);
		}
	}
}

/*******************************************************************************
 * Function Name  : DrawString
 * Description    : Draws string
 * Input			 :
 * 				 :
 *******************************************************************************/
void DrawString(char *pText, uint8_t mode)
{
	if(!cubeStrPos)
	{
		cubeStrPos = 0;
		flagCubeShift = true;

		pString = pText;
		stringMode = mode;

		DrawStringShift();
	}
}

/*******************************************************************************
 * Function Name  : DrawStringShift
 * Description    :
 *******************************************************************************/
void DrawStringShift()
{
	uint16_t len = strlen(pString);
	CubeReset();

	if(stringMode == STRING_ONE_FACE || stringMode == STRING_PIPES)
	{
		for(uint8_t i = 0; i < 8; i++)
		{
			if(!((cubeStrPos + i + 1) % 6) || (cubeStrPos + i + 1) > len * 6) //Blank columns
			{
				for(uint8_t c = 0; c < 8; c++)
				{
					if(stringMode == STRING_PIPES)
						CubeResetLine(YPLANE, 7 - i, 7 - c);
					else
						//STRING_ONE_FACE
						image[8 * (7 - c) + 7] &= ~(1 << (7 - i));
				}
			}
			else
			{
				uint8_t col = font5x8[5
						* (*(pString + (cubeStrPos + i) / 6) - 32)
						+ (cubeStrPos + i) % 6];

				for(uint8_t c = 0; c < 8; c++)
				{
					if(stringMode == STRING_PIPES) //draw chars - lines - 3D
					{
						if(col & (1 << c))
							CubeSetLine(YPLANE, 7 - i, 7 - c);
						else
							CubeResetLine(YPLANE, 7 - i, 7 - c);
					}
					else //STRING_ONE_FACE			draw flat chars
					{
						if(col & (1 << c))
							image[8 * (7 - c) + 7] |= (1 << (7 - i));
						else
							image[8 * (7 - c) + 7] &= ~(1 << (7 - i));
					}
				}
			}
		}

		//End reached?
		if(((++cubeStrPos/*+7*/) / 6) >= strlen(pString))
		{
			flagCubeShift = false;
			cubeStrPos = 0;
		}

		//How long text frame will be displayed
		if(len > 8)
			frameCubeTime = TIMING / 2;
		else
			frameCubeTime = TIMING;
	}
	else //STRING_TWO_FACES
	{
		for(uint8_t i = 0; i < 15; i++)
		{
			if(!((cubeStrPos + i + 1) % 6) || (cubeStrPos + i + 1) > len * 6) //Blank columns
			{
				for(uint8_t c = 0; c < 8; c++)
				{
					if(i <= 7)
						image[8 * (7 - c) + 7] &= ~(1 << (7 - i));
					else
						image[8 * (7 - c) + 7 - (i - 7)] &= ~(1 << (0));
				}
			}
			else
			{
				uint8_t col = font5x8[5
						* (*(pString + (cubeStrPos + i) / 6) - 32)
						+ (cubeStrPos + i) % 6];

				for(uint8_t c = 0; c < 8; c++)
				{
					if(i <= 7)
					{
						if(col & (1 << c))
							BitSet(image[8*(7-c)+7], 1<<(7-i));
						else
							BitReset(image[8*(7-c)+7], 1<<(7-i));
					}
					else
					{
						if(col & (1 << c))
							image[8 * (7 - c) + 7 - (i - 7)] |= (1 << (0));
						else
							image[8 * (7 - c) + 7 - (i - 7)] &= ~(1 << (0));

					}
				}
			}
		}

		//End reached?
		if(((++cubeStrPos) / 6) >= len)
		{
			flagCubeShift = false;
			cubeStrPos = 0;
		}

		//How long text frame will be displayed
		if(len > 8)
			frameCubeTime = TIMING / 3;
		else
			frameCubeTime = TIMING;
	}
}

/*******************************************************************************
 * Function Name  : TrueSnake
 * Description    :
 *******************************************************************************/
void TrueSnake()
{
	for(; snakeLen < SNAKE_MAX_LEN;)
	{
		const uint8_t variant[6][3] =
		{
		{ 0, 1, 2 },
		{ 0, 2, 1 },
		{ 1, 2, 0 },
		{ 1, 0, 2 },
		{ 2, 1, 0 },
		{ 2, 0, 1 } };
		uint8_t i = GetRandom() % 6;

		do
		{
			target[X] = GetRandom() % 8;
			target[Y] = GetRandom() % 8;
			target[Z] = GetRandom() % 8;
		} while(target[X] == Snake[0].voxel[X] || target[Y] == Snake[0].voxel[Y]
				|| target[Z] == Snake[0].voxel[Z]);

#ifdef SNAKE_SHOW_TARGET
		CubeSetVoxel(target[X], target[Y], target[Z]);
#endif

		SnakeMove(variant[i][0]);
		SnakeMove(variant[i][1]);
		SnakeMove(variant[i][2]);

		Snake[0].complete[X] = Snake[0].complete[Y] = Snake[0].complete[Z] = 0;

		if(++snakeLen >= SNAKE_MAX_LEN)
		{
			snakeLen = SNAKE_MIN_LEN;
			CubeReset();
			return;
		}
	}
}

/*******************************************************************************
 * Function Name  : SnakeMove.
 * Description    :
 * Input			 : axis to reach position
 *******************************************************************************/
void SnakeMove(uint8_t axis)
{
	while(!Snake[0].complete[axis])
	{
		uint8_t dir;

		if(Snake[0].voxel[axis] < target[axis])
			dir = INCREASE;
		if(Snake[0].voxel[axis] > target[axis])
			dir = DECREASE;

		for(uint8_t i = snakeLen - 1; i != 0; i--)
		{
			Snake[i].voxel[X] = Snake[i - 1].voxel[X];
			Snake[i].voxel[Y] = Snake[i - 1].voxel[Y];
			Snake[i].voxel[Z] = Snake[i - 1].voxel[Z];
		}

		if(dir == INCREASE)
			Snake[0].voxel[axis]++;
		if(dir == DECREASE)
			Snake[0].voxel[axis]--;

		if(Snake[0].voxel[axis] == target[axis])
			Snake[0].complete[axis] = 1;

		for(uint8_t i = 0; i < snakeLen; i++)
			CubeSetVoxel(Snake[i].voxel[X], Snake[i].voxel[Y],
					Snake[i].voxel[Z]);

		ModesDelay(SNAKE_DELAY);

		if(!(Snake[0].complete[X] && Snake[0].complete[Y]
				&& Snake[0].complete[Z]))
			CubeResetVoxel(Snake[snakeLen - 1].voxel[X],
					Snake[snakeLen - 1].voxel[Y], Snake[snakeLen - 1].voxel[Z]);
	}
}

// Game of Life for the 4x4x4 and 8x8x8 led image

// Original rules:
// live cells:
//  fewer than two neighbour: die
//  two or three neighbours: live
//  more than three neighbours: die
// dead cells:
//  exactly three live neighbours becomes alive

// This is 3d space, so the cell can have life on two more sides.
// We have to tweak the rules a bit to make it work:

// Create life in a dead cell if neighbours == 4
#define GOL_CREATE_MIN 4
#define GOL_CREATE_MAX 4

// Underpopulation
#define GOL_TERMINATE_LONELY 3
// Overpopulation
#define GOL_TERMINATE_CROWDED 5

#define GOL_X 8
#define GOL_Y 8
#define GOL_Z 8

#define GOL_WRAP 0

volatile uint8_t fb[64];

/*******************************************************************************
 * Function Name  :
 * Description    :
 *******************************************************************************/
void GOL_Begin()
{
	CubeReset();
	ModesDelay(1000);

	// Create a random starting point for the Game of Life effect.
	for(uint8_t i = 0; i < 12; i++)
	{
		CubeSetVoxel(GetRandom() % 4, GetRandom() % 4, GetRandom() % 4);
	}

	GOL_Play(40, 80);
}

/*******************************************************************************
 * Function Name  :
 * Description    :
 *******************************************************************************/
void GOL_Play(int iterations, int delay)
{
	for(uint16_t i = 0; i < iterations; i++)
	{
		GOL_NextGeneration();

		if(GOL_CountChanges() == 0)
			return;

		GOL_Temp2Cube();

		ModesDelay(delay);
	}
}

/*******************************************************************************
 * Function Name  :
 * Description    :
 *******************************************************************************/
void GOL_NextGeneration()
{
	for(uint8_t i = 0; i < 64; i++)
		fb[i] = 0; //reset temp buffer

	for(uint8_t x = 0; x < 8; x++)
	{
		for(uint8_t y = 0; y < 8; y++)
		{
			for(uint8_t z = 0; z < 8; z++)
			{
				uint8_t neigh = GOL_CountNeighbors(x, y, z);

				if(CubeGetVoxel(x, y, z) == 1)
				{
					// Current voxel is alive
					if(neigh <= GOL_TERMINATE_LONELY)
					{
						GOL_TempClearVoxel(x, y, z);
					}
					else if(neigh >= GOL_TERMINATE_CROWDED)
					{
						GOL_TempClearVoxel(x, y, z);
					}
					else
					{
						GOL_TempSetVoxel(x, y, z);
					}
				}
				else
				{
					// Current voxel is dead
					if(neigh >= GOL_CREATE_MIN && neigh <= GOL_CREATE_MAX)
						GOL_TempSetVoxel(x, y, z);
				}
			}
		}
	}
}

/*******************************************************************************
 * Function Name  :
 * Description    :
 *******************************************************************************/
uint8_t GOL_CountNeighbors(int x, int y, int z)
{
	uint8_t neigh = 0; // number of alive neighbours.

	for(signed int ix = -1; ix < 2; ix++) //offset 1 in each direction in each dimension
	{
		for(signed int iy = -1; iy < 2; iy++)
		{
			for(signed int iz = -1; iz < 2; iz++)
			{
				// Your not your own neighbour, exclude 0,0,0, offset.
				if(!(ix == 0 && iy == 0 && iz == 0))
				{
					signed int nx, ny, nz; // neighbours address.

					if(GOL_WRAP == 0x01)
					{
						nx = (x + ix) % 8;
						ny = (y + iy) % 8;
						nz = (z + iz) % 8;
					}
					else
					{
						nx = x + ix;
						ny = y + iy;
						nz = z + iz;
					}

					if((nx >= 0 && nx <= 7) && (ny >= 0 && ny <= 7)
							&& (nz >= 0 && nz <= 7))
					{
						if(CubeGetVoxel(nx, ny, nz))
							neigh++;
					}
					/*	if(nx>7 || nx<0 || ny>7 || ny<0 || nz>7 || nz<0)
					 {

					 }
					 else
					 {
					 if(CubeGetVoxel(nx, ny, nz))
					 neigh++;
					 }*/
				}
			}
		}
	}

	return neigh;
}

/*******************************************************************************
 * Function Name  :
 * Description    :
 *******************************************************************************/
uint16_t GOL_CountChanges()
{
	uint16_t count = 0;

	for(uint8_t i = 0; i < 64; i++)
	{
		if(fb[i] != image[i])
			count++;
	}

	return count;
}

/*******************************************************************************
 * Function Name  :
 * Description    :
 *******************************************************************************/
void GOL_Temp2Cube()
{
	for(uint8_t i = 0; i < 64; i++)
		image[i] = fb[i];
}

/*******************************************************************************
 * Function Name  :
 * Description    :
 *******************************************************************************/
void GOL_TempSetVoxel(int x, int y, int z)
{
	if(x > 7 || y > 7 || z > 7)
		return;
	//if( (fb[8*z+7-y] & (1<<x)) == 1 ) return ;

	fb[8 * z + 7 - y] |= (1 << x);
	//fb[8*z+y] |= (1 << x);
}

/*******************************************************************************
 * Function Name  :
 * Description    :
 *******************************************************************************/
void GOL_TempClearVoxel(int x, int y, int z)
{
	if(x > 7 || y > 7 || z > 7)
		return;
	//if( (fb[8*z+7-y] & (1<<x)) == 0 ) return ;

	fb[8 * z + 7 - y] &= ~(1 << x);
	//fb[8*x+y] &= ~(1 << x);
}
