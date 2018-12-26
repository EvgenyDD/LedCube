/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef MODES_H
#define MODES_H


/* Includes ------------------------------------------------------------------*/
//#include <stm32f10x.h>
//#include <stm32f10x_gpio.h>
#include "interface.h"
#include "random.h"
#include "debug.h"
#include "string.h"


/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
enum StringDisplayMode{STRING_ONE_FACE, STRING_PIPES, STRING_TWO_FACES};
enum {DOWN, UP};


/* Exported macro ------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void ModesDelay(uint16_t);



void RandomPixel();
void RandomPixelAccumulate();

void RainSetMode(uint8_t, uint8_t);
void Rain();

void Shift (uint8_t, signed char);

void TrueSnake();
void SnakeMove(uint8_t);

void Outline(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);
void OutlineBox();
void OutlineRandomBoxes();

void PlaneBoing();

void RandMidwayDest();
void RandMidwayDest_Draw(uint8_t, uint8_t position[64], uint8_t destination[64], uint8_t);

void DrawString(char*, uint8_t);
void DrawStringShift();

void MoveVoxelsAlongZInit();
void MoveVoxelsAlongZ();
void MoveVoxelAlongZ(uint8_t x, uint8_t y, uint8_t z);

void SphereMove ();

float Distance2D(float x1, float y1, float x2, float y2);
float Distance3D(float x1, float y1, float z1, float x2, float y2, float z2);
void Ripples();
void SideWaves();
void Firework();
void RandomInvert();

void SineLines ();
void spheremove ();
void line_3d (int x1, int y1, int z1, int x2, int y2, int z2);



void Stairs();
void WormQqueeze();
void CharSpin();
//typedef struct {float x; float y; float z;} vertex;
void effect_blinky2();
void RandSelParallel ();
void draw_positions_axis (char axis, unsigned char positions[64], int invert);
void effect_loadbar();
void effect_random_sparkle_flash (int iterations, int voxels, int delay);


void GOL_Begin();
void GOL_Play(int iterations, int delay);
unsigned char GOL_CountNeighbors(int x, int y, int z);
void GOL_NextGeneration(void);
uint16_t GOL_CountChanges(void);

void GOL_Temp2Cube (void);
void GOL_TempSetVoxel(int x, int y, int z);
void GOL_TempClearVoxel(int x, int y, int z);



float sinReal(float x);
float sin_(float x);
float cos_(float x);
float tan_(float x);
float factorial(double x);
float pow_(float x,float y);
float sqrt_(float x);


#endif //MODES_H
