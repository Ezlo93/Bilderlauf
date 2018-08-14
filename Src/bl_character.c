#include "bl_character.h"
#include "debug.h"

bl_Character* bl_CreateCharacter(float _X, float _Y, float _Z, float _height, bl_Camera* _camera){
	bl_Character* c = (bl_Character*)malloc(sizeof(bl_Character));

	c->x = _X;
	c->y = _Y;
	c->z = _Z;
	c->positionOnGridX = 0;
	c->positionOnGridY = 0;
	c->isRunning = 0;

	c->height = _height;
	c->inAir = 0;
	c->airTime = 0;
	c->camera = _camera;
	c->camera->Position.Y = c->height+c->z;
	bl_CameraUpdate(c->camera);
	return c;
}


void bl_UpdateCharacter(bl_Character* _char, int *_input, float _deltaTime, bl_BMPData *_data,
	float _hexasize){

		double yaw_inc = 0;
		int i,  keepmoving = 1, collision = 0, movement = 0;
		bl_Point gridPos, currentPos, intendedMove, centerPos;
		float _hexawidth, _hexaheight, bl_movex, bl_movey, bl_movez,char_actual_speed;

		_hexaheight = _hexasize*2; 
		_hexawidth = (float)(sqrt(3.f)) * _hexasize;


		//check if wasd movement
		for(i = 0; i < 4; i++ ){
			if(_input[i] == 1){
				movement = 1; break;
			}
		}

		char_actual_speed = _char->isRunning ? BL_CHARACTER_SPEED * 2 : BL_CHARACTER_SPEED;

		//Jump/Fall

		if(_input[4] == 1 && !_char->inAir){

			_char->inAir = 1;
			_char->airTime = 0;
			_char->zVelocity = _char->isRunning ? BL_CHARACTER_JUMP_VELOCITY * 1.5f : BL_CHARACTER_JUMP_VELOCITY;
			_char->zOrigin = _char->z;

		}

		if(_char->inAir){
			_char->airTime += _deltaTime;
			bl_movez = (_char->zVelocity * _char->airTime) - (GRAVITY * _char->airTime * _char->airTime); 
		}

		//W A S D movement

		//W + strafe A D

		if(movement){
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
			}else if(_input[1] == 1){

				//S + strafe A D

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
			}else if(_input[2] == 1){
				//strafe left

				yaw_inc += M_PI * 1.5f;
			}else if(_input[3] == 1){
				//strafe right

				yaw_inc += M_PI * 0.5f;
			}


			//movement equation
			bl_movex = (float)(cos(_char->camera->yaw + yaw_inc)) * char_actual_speed * keepmoving * _deltaTime;
			bl_movey = (float)(-sin(_char->camera->yaw + yaw_inc)) * char_actual_speed * keepmoving * _deltaTime;


			//check collision


			collision = 0;
			intendedMove.x.pFloat = _char->x + bl_movex;
			intendedMove.y.pFloat = _char->y - bl_movey;

			currentPos.x.pInt = _char->positionOnGridX;
			currentPos.y.pInt = _char->positionOnGridY;

			//check current hexagon plus 6 neighbors for collision
			for(i= 0; i < 7; i++){
				//difference between odd/even rows

				currentPos = convertPosition(_char, i);

				//even
				if(currentPos.y.pInt % 2 == 0){

					centerPos.x.pFloat = currentPos.x.pInt * _hexawidth;
					centerPos.y.pFloat = currentPos.y.pInt * (3.f/4 * _hexaheight);


					//odd
				}else{

					centerPos.x.pFloat = (currentPos.x.pInt * _hexawidth) - (_hexawidth/2);
					centerPos.y.pFloat = currentPos.y.pInt * (3.f/4 * _hexaheight);

				}

				if(pointHexagonCollision(centerPos.x.pFloat, centerPos.y.pFloat, _hexasize, intendedMove.x.pFloat, intendedMove.y.pFloat)){
					collision = i;
					break;
				}

			}

			//translate collision int back to x,y position on grid
			//even

			gridPos = convertPosition(_char, collision);


			//only when intended move puts character on neighbor hexagon
			if(collision != 3){

				//check height difference of hexagons and out of bounds

				if((_data->bmpData[_data->bmpWidth*gridPos.y.pInt+gridPos.x.pInt].Height - 
					_char->z) > (_char->inAir ? 0 : BL_CHARACTER_CLIMB_THRESHOLD)
					||
					gridPos.x.pInt < 0
					||
					gridPos.y.pInt < 0
					||
					gridPos.x.pInt > _data->bmpWidth-1
					||
					gridPos.y.pInt > _data->bmpHeight-1
					){

						collision=-1;
						//

				}
			}

			if(collision != -1){
				_char->positionOnGridX = gridPos.x.pInt;
				_char->positionOnGridY = gridPos.y.pInt;


				//set position
				_char->x = intendedMove.x.pFloat;
				_char->y = intendedMove.y.pFloat;
			}
		}

		//add jump value
		if(_char->inAir){
			_char->z = _char->zOrigin+bl_movez;
		}

		//check if character is on the ground
		if(_char->z < _data->bmpData[_data->bmpWidth*_char->positionOnGridY+_char->positionOnGridX].Height){
			_char->inAir = 0;
			_char->z = _data->bmpData[_data->bmpWidth*_char->positionOnGridY+_char->positionOnGridX].Height;
		}

		//check if player moved from a higher to a lower hexagon / is falling
		if(!_char->inAir && _char->z > _data->bmpData[_data->bmpWidth*_char->positionOnGridY+_char->positionOnGridX].Height){

			_char->inAir = 1;
			_char->airTime = 0;
			_char->zVelocity = 0;
			_char->zOrigin = _char->z;
		}


		//update camera
		_char->camera->Position.X = _char->x;
		_char->camera->Position.Z = _char->y;
		_char->camera->Position.Y = _char->z + _char->height;

}


int pointHexagonCollision(float _hexacenterx, float _hexacentery, float _size, float _pointx, float _pointy){
	int collision = 0, current, next, i;
	double angle;
	bl_Point vertices[6];
	bl_Point vertices_current, vertices_next;

	//calculate collision box vertices

	for(i = 0; i < 6; i++){
		angle = M_PI / 180 * (60*i + 30);

		vertices[i].x.pFloat = _hexacenterx+_size * (float)(cos(angle));
		vertices[i].y.pFloat = _hexacentery+_size * (float)(sin(angle));

	}

	//

	next = 0;
	for(current = 0; current < 6; current++){
		next = current+1;
		if(next == 6) {next = 0;}

		vertices_current = vertices[current];
		vertices_next = vertices[next];

		//magic
		if (((vertices_current.y.pFloat > _pointy && vertices_next.y.pFloat < _pointy) || (vertices_current.y.pFloat < _pointy && vertices_next.y.pFloat > _pointy)) &&
			(_pointx < (vertices_next.x.pFloat-vertices_current.x.pFloat)*(_pointy-vertices_current.y.pFloat) / (vertices_next.y.pFloat-vertices_current.y.pFloat)+vertices_current.x.pFloat)) {
				collision = 1-collision;
		}

	}
	return collision;
}


bl_Point convertPosition(bl_Character *_char, int _pos){
	bl_Point point;

	//y row of current position is independent of odd/even row
	if(_pos == 3){
		point.x.pInt = _char->positionOnGridX;
		point.y.pInt = _char->positionOnGridY;
	}else if(_pos == 2){
		point.x.pInt = _char->positionOnGridX-1;
		point.y.pInt = _char->positionOnGridY;
	}else if(_pos == 4){
		point.x.pInt = _char->positionOnGridX+1;
		point.y.pInt = _char->positionOnGridY;
	}else{
		//odd/even row for top and bottom hexagons
		if(_char->positionOnGridY % 2 == 0){
			switch(_pos){
			case 0: point.x.pInt = _char->positionOnGridX; point.y.pInt = _char->positionOnGridY-1; break;
			case 1: point.x.pInt = _char->positionOnGridX+1; point.y.pInt = _char->positionOnGridY-1; break;
			case 5: point.x.pInt = _char->positionOnGridX; point.y.pInt = _char->positionOnGridY+1; break;
			case 6: point.x.pInt = _char->positionOnGridX+1; point.y.pInt = _char->positionOnGridY+1; break;
			}
		}else{
			switch(_pos){
			case 0: point.x.pInt = _char->positionOnGridX-1; point.y.pInt = _char->positionOnGridY-1; break;
			case 1: point.x.pInt = _char->positionOnGridX; point.y.pInt = _char->positionOnGridY-1; break;
			case 5: point.x.pInt = _char->positionOnGridX-1; point.y.pInt = _char->positionOnGridY+1; break;
			case 6: point.x.pInt = _char->positionOnGridX; point.y.pInt = _char->positionOnGridY+1; break;
			}
		}
	}

	return point;
}