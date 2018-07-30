#include "bl_character.h"
#include "debug.h"

bl_Character* bl_CreateCharacter(float _X, float _Y, float _height, bl_Camera* _camera){
	bl_Character* c = (bl_Character*)malloc(sizeof(bl_Character));

	c->x = _X;
	c->y = _Y;
	c->positionOnGridX = 0;
	c->positionOnGridY = 0;

	c->height = _height;
	c->inAir = 0;
	c->camera = _camera;
	c->camera->Position.Y = c->height;
	bl_CameraUpdate(c->camera);
	return c;
}


void bl_UpdateCharacter(bl_Character* _char, int *_input, float _deltaTime, bl_BMPData *data){
	double bl_movex, bl_movey, bl_movez, char_actual_speed = BL_CHARACTER_SPEED, yaw_inc = 0;
	int i, inputcounter = 0, keepmoving = 1;
	//ADD COLLISION
	//YAW = Direction

	for(i = 0; i < 4; i++){
		if(_input[i] == 1){
			inputcounter++;
		}
	}
	if(inputcounter == 0){ return;}

	//ADD JUMP
	bl_movez = 0;

	if(_input[4] == 1 || _char->inAir == 1){

		if(_char->inAir == 0){
			_char->inAir = 1;

		}else{


		}

	}

	//W A S D movement
	
	//W + strafe A D

	if(_input[0] == 1){
		if(_input[3] == 1){			//+ right strafe
			yaw_inc = M_PI*0.25f;
		}else if(_input[2] == 1){   //+ left strafe
			yaw_inc = M_PI*-0.25f;
		}else if(_input[1] == 1){   //+ s = stop
			yaw_inc = 0;
			keepmoving = 0;
		}
		if(_input[3] == _input[2]){ //normal or both strafes
		yaw_inc = 0;
		}
	}else
	
	//S + strafe A D
	if(_input[1] == 1){
		if(_input[3] == 1){			//+ right strafe
			yaw_inc = M_PI*0.75f;
		}else if(_input[2] == 1){   //+ left strafe
			yaw_inc = M_PI*1.25f;
		}else if(_input[0] == 1){   //+ s = stop
			yaw_inc = 0;
			keepmoving = 0;
		}
		if(_input[3] == _input[2]){ //normal or both strafes
		yaw_inc = M_PI;
		}
	}else
	//strafe left
	if(_input[2] == 1){
		yaw_inc += M_PI * 1.5f;
	}else
	//strafe right
	if(_input[3] == 1){
		yaw_inc += M_PI * 0.5f;
	}


	//movement equation
	bl_movex = cos(_char->camera->yaw + yaw_inc) * char_actual_speed * keepmoving;
	bl_movey = -sin(_char->camera->yaw + yaw_inc) * char_actual_speed * keepmoving;

	//set position
	_char->x = _char->x + bl_movex * _deltaTime;
	_char->y = _char->y - bl_movey * _deltaTime;


	_char->camera->Position.X = _char->x;
	_char->camera->Position.Z = _char->y;
	_char->camera->Position.Y = _char->height + bl_movez;
}
