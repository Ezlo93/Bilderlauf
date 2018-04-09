#ifndef BL_CHARACTER_H
#define BL_CHARACTER_H

#include "bl_camera.h"

typedef struct{
	float X,Y;
	float height;
	bl_Camera *camera;
} bl_Character;

bl_Character* bl_CreateCharacter(float X, float Y, float Z, float height);

#endif