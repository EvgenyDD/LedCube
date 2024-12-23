#ifndef MODES_H
#define MODES_H

#include "debug.h"
#include "interface.h"
#include "random.h"
#include "string.h"

enum StringDisplayMode
{
	STRING_ONE_FACE,
	STRING_PIPES,
	STRING_TWO_FACES
};
enum
{
	DOWN,
	UP
};

void ModesDelay(uint16_t);

void RandomPixel(void);
void RandomPixelAccumulate(void);

void RainSetMode(uint8_t, uint8_t);
void Rain(void);

void Shift(uint8_t, signed char);

void TrueSnake(void);
void SnakeMove(uint8_t);

void Outline(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);
void OutlineBox(void);
void OutlineRandomBoxes(void);

void PlaneBoing(void);

void RandMidwayDest(void);
void RandMidwayDest_Draw(uint8_t, uint8_t position[64], uint8_t destination[64], uint8_t);

void DrawString(const char *, uint8_t);
void DrawStringShift(void);

void MoveVoxelsAlongZInit(void);
void MoveVoxelsAlongZ(void);
void MoveVoxelAlongZ(uint8_t x, uint8_t y, uint8_t z);

void SphereMove(void);

float Distance2D(float x1, float y1, float x2, float y2);
float Distance3D(float x1, float y1, float z1, float x2, float y2, float z2);
void Ripples(void);
void SideWaves(void);
void Firework(void);
void RandomInvert(void);

void SineLines(void);
void spheremove(void);
void line_3d(int x1, int y1, int z1, int x2, int y2, int z2);

void Stairs(void);
void WormQqueeze(void);
void CharSpin(void);
// typedef struct {float x; float y; float z;} vertex;
void effect_blinky2(void);
void RandSelParallel(void);
void draw_positions_axis(char axis, unsigned char positions[64], int invert);
void effect_loadbar(void);
void effect_random_sparkle_flash(int iterations, int voxels, int delay);

void GOL_Begin(void);
void GOL_Play(int iterations, int delay);
unsigned char GOL_CountNeighbors(int x, int y, int z);
void GOL_NextGeneration(void);
uint16_t GOL_CountChanges(void);

void GOL_Temp2Cube(void);
void GOL_TempSetVoxel(int x, int y, int z);
void GOL_TempClearVoxel(int x, int y, int z);

float sinReal(float x);
float sin_(float x);
float cos_(float x);
float tan_(float x);
float factorial(double x);
float pow_(float x, float y);
float sqrt_(float x);

#endif // MODES_H
