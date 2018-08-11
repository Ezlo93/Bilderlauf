#include "bl_camera.h"
#include "debug.h"

bl_Camera *CreateCamera(bl_CameraPosition _cameraPosition, int _mode){
	bl_Camera* _camera;

	//Allocate memory for _camera
	_camera = (bl_Camera*) malloc(sizeof(bl_Camera));
	if(_camera == NULL){
		perror("Allocating memory for camera failed!");
		exit(10);
	}

	_camera->Position = _cameraPosition;

	//Set Mode, Topview is default
	switch(_mode){
		case 0: _camera->Mode = BL_CAM_TOP; break;
		case 1: _camera->Mode = BL_CAM_FPP; break;
		default: printf("Unsupported camera mode %d!\n", _mode);
				 _camera->Mode = BL_CAM_TOP;
				 break;
	}

	//Set LookAt depending on mode
	_camera->LookAt = _camera->Position;

	_camera->pitch = 0;
	_camera->yaw = 0;

	return _camera;
}

//Creates a bl_CameraPosition struct with X Y Z
bl_CameraPosition CreateCameraPosition(float _x, float _y, float _z){
	bl_CameraPosition tmp;
	tmp.X = _x; tmp.Y = _y; tmp.Z = _z;
	return tmp;
}


void bl_CameraInfo(bl_Camera *_camera){
	printf("Camera Info:\n");
	printf("Mode: %d\n", _camera->Mode);
	printf("Position: %f %f %f\n", _camera->Position.X, _camera->Position.Y, _camera->Position.Z);
	printf("LookAt: %f %f %f\n", _camera->LookAt.X, _camera->LookAt.Y, _camera->LookAt.Z);
	printf("Pitch: %f\nYaw: %f\n", _camera->pitch, _camera->yaw);
}


void bl_CameraUpdate(bl_Camera *_camera){
	_camera->LookAt.X = cos(_camera->yaw) * cos(_camera->pitch);
	_camera->LookAt.Y = sin(_camera->pitch);
	_camera->LookAt.Z = sin(_camera->yaw) * cos(_camera->pitch);

}


//Up/Down Rotation of the _camera
void bl_CameraRotatePitch(bl_Camera *_camera, float _angle){
	static float limit = 89.9f * (float)M_PI / 180.f;

	_camera->pitch += _angle;

    if(_camera->pitch < -limit)
        _camera->pitch = -limit;

    if(_camera->pitch > limit)
        _camera->pitch = limit;

}

//Left/Right Rotation of the _camera
void bl_CameraRotateYaw(bl_Camera *_camera, float _angle){
	
	_camera->yaw = (float)fmod(_camera->yaw+_angle,(float)(2.f*M_PI));


}