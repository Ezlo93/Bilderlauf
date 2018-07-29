#include "bl_character.h"
#include "debug.h"

bl_Character* bl_CreateCharacter(float _X, float _Y, float _height, bl_Camera* _camera){
	bl_Character* c = (bl_Character*)malloc(sizeof(bl_Character));

	c->x = _X;
	c->y = _Y;
	c->height = _height;
	c->camera = _camera;

	return c;
}


void bl_UpdateCharacter(bl_Character* _char, int** _input, float _deltaTime){
	float bl_movex, bl_movey, bl_movez;

	bl_movex = BL_CHARACTER_SPEED * (_input[0]-_input[1]);
	bl_movey = BL_CHARACTER_SPEED * (_input[2]-_input[3]);

	_char->x = _char->x + bl_movex * _deltaTime;
	_char->y = _char->y + bl_movey * _deltaTime;

	_char->camera->Position.X = _char->x;
	_char->camera->Position.Y = _char->y;
	bl_CameraUpdate(_char->camera);
}
