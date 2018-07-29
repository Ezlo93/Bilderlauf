#ifndef BL_CHARACTER_H
#define BL_CHARACTER_H

#include "bl_camera.h"

#define BL_CHARACTER_SPEED 2.5f



typedef struct{
	float x,y;
	float height;
	bl_Camera *camera;
} bl_Character;

bl_Character* bl_CreateCharacter(float _X, float _Y, float _height, bl_Camera* _camera);
void bl_UpdateCharacter(bl_Character* _char, int** _input, float _deltaTime);


#endif