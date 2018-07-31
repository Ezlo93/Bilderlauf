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


void bl_UpdateCharacter(bl_Character* _char, int *_input, float _deltaTime, bl_BMPData *data,
						float _hexasize, float _hexawidth, float _hexaheight){


	double bl_movex, bl_movey, bl_movez, char_actual_speed = BL_CHARACTER_SPEED, yaw_inc = 0;
	int i, inputcounter = 0, keepmoving = 1, currentX, currentY,gridPosX, gridPosY;
	float centerX, centerY, intendedMoveX, intendedMoveY;
	float hexagon_distances[7];
	


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
	bl_movex = cos(_char->camera->yaw + yaw_inc) * char_actual_speed * keepmoving * _deltaTime;
	bl_movey = -sin(_char->camera->yaw + yaw_inc) * char_actual_speed * keepmoving * _deltaTime;

	//check for collision
	
	//calculate distance to neighbors and saved hexagon, least distance = where char is positioned

	//begin left of current hexagon
	currentX = _char->positionOnGridX-1;
	currentY = _char->positionOnGridY;

	intendedMoveX = _char->x + bl_movex;
	intendedMoveY = _char->y - bl_movey;

	for(i = 0; i < 7; i++){

		if(i == 1){ currentX++; currentY--;}
		if(i > 1 && i < 4){ currentY++;}
		if(i == 4){currentX++; currentY -= 2;}
		if(i > 4){currentY++;}

		//distinguish between odd and even rows
		if(currentY % 2 == 0){
			centerX = currentX * _hexawidth;
			centerY = currentY * (3.f/4 * _hexaheight);
			//centerX = currentX * _hexawidth + (0.5 * _hexawidth);
			//centerY = currentY * (3.f/4 * _hexaheight) + (0.5 * _hexaheight);
		}else{
			centerX = currentX * _hexawidth - 0.5f * _hexawidth;
			centerY = currentY * (3.f/4 * _hexaheight);
			//centerX = currentX * _hexawidth + _hexawidth;
			//centerY = currentY * (3.f/4 * _hexaheight) + (0.5 * _hexaheight);
		}

		hexagon_distances[i] = sqrt( pow( (intendedMoveX - centerX), 2) + pow( (intendedMoveY - centerY), 2)) ;
	}


	
	//find the lowest distance and store index in gridPosY
	gridPosX = 100;
	gridPosY = 1;

	for(i=0; i < 7; i++){
		if(hexagon_distances[i] < gridPosX){
			gridPosX = hexagon_distances[i];
			gridPosY = i;
		}
	}

	//calculate gridPosY index back to hexagon coordinates
	switch (gridPosY){
	case 0: gridPosX = _char->positionOnGridX-1; gridPosY = _char->positionOnGridY; break;
	case 1: gridPosX = _char->positionOnGridX; gridPosY = _char->positionOnGridY-1; break;
	case 2: gridPosX = _char->positionOnGridX; gridPosY = _char->positionOnGridY; break;
	case 3: gridPosX = _char->positionOnGridX; gridPosY = _char->positionOnGridY+1; break;
	case 4: gridPosX = _char->positionOnGridX+1; gridPosY = _char->positionOnGridY-1; break;
	case 5: gridPosX = _char->positionOnGridX+1; gridPosY = _char->positionOnGridY; break;
	case 6: gridPosX = _char->positionOnGridX+1; gridPosY = _char->positionOnGridY+1; break;
	}



	if(gridPosX != _char->positionOnGridX || gridPosY != _char->positionOnGridY){
#if DEBUG > 0
		printf("P: %d : %d\n", gridPosX, gridPosY);
#endif
		_char->positionOnGridX = gridPosX;
		_char->positionOnGridY = gridPosY;
	}

	//set position
	_char->x = _char->x + bl_movex;
	_char->y = _char->y - bl_movey;

	//update camera
	_char->camera->Position.X = _char->x;
	_char->camera->Position.Z = _char->y;

	//BANDAID -> char pos snaps to height of hexagon
	_char->camera->Position.Y = _char->height + data->bmpData[data->bmpWidth*_char->positionOnGridY+_char->positionOnGridX].Height + bl_movez;
}


int round(double number)
{
    return (number >= 0) ? (int)(number + 0.5) : (int)(number - 0.5);
}