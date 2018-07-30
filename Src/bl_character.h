#ifndef BL_CHARACTER_H
#define BL_CHARACTER_H

#include "bl_camera.h"
#include "bl_bmp.h"
#include <math.h>

#define BL_CHARACTER_SPEED 4.5f 
#define GRAVITY 9.81f

typedef struct{
	float x,y;
	float height;
	int inAir;
	int positionOnGridX, positionOnGridY;
	bl_Camera *camera;
} bl_Character;

bl_Character* bl_CreateCharacter(float _X, float _Y, float _height, bl_Camera* _camera);
void bl_UpdateCharacter(bl_Character* _char, int** _input, float _deltaTime, bl_BMPData *data);


#endif