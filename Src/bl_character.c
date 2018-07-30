#include "bl_character.h"
#include "debug.h"

bl_Character* bl_CreateCharacter(float _X, float _Y, float _height, bl_Camera* _camera){
	bl_Character* c = (bl_Character*)malloc(sizeof(bl_Character));

	c->x = _X;
	c->y = _Y;
	c->height = _height;
	c->camera = _camera;
	c->camera->Position.Y = c->height;
	bl_CameraUpdate(c->camera);
	return c;
}


void bl_UpdateCharacter(bl_Character* _char, int *_input, float _deltaTime){
	double bl_movex, bl_movey, bl_movez;

	//ADD COLLISION
	//YAW = Direction

	if(_input[0] == 1){

		bl_movex = cos(_char->camera->yaw) * BL_CHARACTER_SPEED;
		bl_movey = -sin(_char->camera->yaw) * BL_CHARACTER_SPEED;

		_char->x = _char->x + bl_movex * _deltaTime;
		_char->y = _char->y - bl_movey * _deltaTime;






		

	}
		//ADD JUMP
		bl_movez = 0;


		_char->camera->Position.X = _char->x;
		_char->camera->Position.Z = _char->y;
		_char->camera->Position.Y = _char->height + bl_movez;
}
