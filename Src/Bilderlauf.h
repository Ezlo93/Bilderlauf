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

#define CAMERAMAX 2
#define DRAWDISTANCE 112.f
#define DRAWDISTANCE_BLEND_DISTANCE 16.f

#define HEXAGONOFFSET 0.1f
#define HEXAGONSIZE 2.5f
#define HEXAGONMAXHEIGHT 13.f

enum {X=0, Y=1, Z=2, W=3};
enum {DRAW_HEXAGON=0,DRAW_CUBE=1};


int main(int argc, char **argv);

void createHexagonVertices(float size, float height);
void drawHexagon(int x, int y, bl_BMPData *data, int mode, float _opacity);
void drawCube(int x, int y, bl_BMPData *data);
bl_CameraPosition calculateTopCameraPosition();
void draw(void);

void key(unsigned char key, int x, int y);
void releaseKey(unsigned char key, int x, int y);
void mouseMove(int x, int y);

void timer(int a);
void ExtractFrustum();
int SphereInFrustum( float x, float y, float z, float radius );

#endif