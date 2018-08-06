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

int main(int argc, char **argv);

void createHexagonVertices(float size, float height);
void drawHexagon(int x, int y, bl_BMPData *data);
void drawCube(int x, int y, bl_BMPData *data);
bl_CameraPosition calculateTopCameraPosition();
void draw(void);

void key(unsigned char key, int x, int y);
void releaseKey(unsigned char key, int x, int y);
void mouseMove(int x, int y);

void timer(int a);

#endif