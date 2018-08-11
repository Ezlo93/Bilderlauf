// Bilderlauf.h
#ifndef BILDERLAUF_H
#define BILDERLAUF_H

#include "Windows.h"
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include <GL/glut.h>

#include "bl_bmp.h"
#include "bl_camera.h"
#include "bl_character.h"

#define MVERSION 0
#define SVERSION 9

#define FILENAMEBUFFER 100

#define ESC        27
#define SPACE	   32

#define CAMERAMAX 1
#define DRAWDISTANCE 112.f
#define DRAWDISTANCE_BLEND_DISTANCE 16.f
#define FPS 60.f

#define HEXAGONOFFSET 0.1f
#define HEXAGONSIZE 2.5f
#define HEXAGONMAXHEIGHT 10.f

enum {X=0, Y=1, Z=2, W=3};
enum {DRAW_HEXAGON=0};


int main(int argc, char **argv);

void createHexagonVertices(float _size, float _height);
void drawHexagon(int _x, int _y, bl_BMPData *_data, int _mode, float _opacity);
void draw(void);

void key(unsigned char _key, int _x, int _y);
void releaseKey(unsigned char _key, int _x, int _y);
void mouseMove(int _x, int _y);

void timer(int _a);
void ExtractFrustum();
int SphereInFrustum( float _x, float _y, float _z, float _radius);

#endif