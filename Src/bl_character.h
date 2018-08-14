#ifndef BL_CHARACTER_H
#define BL_CHARACTER_H

#include "bl_camera.h"
#include "bl_bmp.h"
#include <math.h>

#define BL_CHARACTER_SPEED 12.f 
#define BL_CHARACTER_HEIGHT 4.f
#define BL_CHARACTER_CLIMB_THRESHOLD .8f
#define BL_CHARACTER_JUMP_VELOCITY 18.f
#define GRAVITY 9.81f

typedef struct{
	float x,y,z;
	int isRunning;
	float height;

	int inAir;
	float airTime;
	float zVelocity;
	float zOrigin;

	int positionOnGridX, positionOnGridY;
	bl_Camera *camera;
} bl_Character;

typedef union{
	int pInt;
	float pFloat;
}bl_uif;

typedef struct{
	bl_uif x;
	bl_uif y;
}bl_Point;

bl_Character* bl_CreateCharacter(float _X, float _Y, float _Z, float _height, bl_Camera* _camera);
void bl_UpdateCharacter(bl_Character* _char, int *_input, float _deltaTime, bl_BMPData *_data,
	float _hexasize);
int pointHexagonCollision(float _hexacenterx, float _hexacentery, float _size, float _pointx, float _pointy);
bl_Point convertPosition(bl_Character *_char, int _pos);

#endif