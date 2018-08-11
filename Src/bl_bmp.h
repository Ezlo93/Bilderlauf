#ifndef BL_BMP_H
#define BL_BMP_H

#include "Windows.h"
#include <stdio.h>
#include <stdlib.h>

#define MAX_COLOR 1.f
#define MIN_COLOR 0.f

typedef struct{
	float R,G,B;
	float Height;
} bl_Color;

typedef struct {
	bl_Color minColor;
	bl_Color maxColor; 
	bl_Color *bmpData;
	int bmpWidth, bmpHeight;
} bl_BMPData;


/* BMP Structs*/

//pragma pack = no padding in data structure
#pragma pack(push, 1)

typedef struct bl_tagBITMAPFILEHEADER
{
    WORD bfType;  //specifies the file type
    DWORD bfSize;  //specifies the size in bytes of the bitmap file
    WORD bfReserved1;  //reserved; must be 0
    WORD bfReserved2;  //reserved; must be 0
    DWORD bOffBits;  //species the offset in bytes from the bitmapfileheader to the bitmap bits
}BL_BITMAPFILEHEADER;

#pragma pack(pop)	

#pragma pack(push, 1)

typedef struct bl_tagBITMAPINFOHEADER
{
    DWORD biSize;  //specifies the number of bytes required by the struct
    LONG biWidth;  //specifies width in pixels
    LONG biHeight;  //species height in pixels
    WORD biPlanes; //specifies the number of color planes, must be 1
    WORD biBitCount; //specifies the number of bit per pixel
    DWORD biCompression;//spcifies the type of compression
    DWORD biSizeImage;  //size of image in bytes
    LONG biXPelsPerMeter;  //number of pixels per meter in x axis
    LONG biYPelsPerMeter;  //number of pixels per meter in y axis
    DWORD biClrUsed;  //number of colors used by th ebitmap
    DWORD biClrImportant;  //number of colors that are important
}BL_BITMAPINFOHEADER;

#pragma pack(pop)





unsigned char *LoadBitmapFile(char *_filename, BL_BITMAPINFOHEADER *_bitmapInfoHeader);
bl_BMPData *CreateBMPData(unsigned char *_bmpData, BL_BITMAPINFOHEADER *_bitmapInfoHeader);
int ColorCompare(bl_Color* _a, bl_Color* _b);
void calculateHeight(bl_BMPData *_data, float _scale);
void printBMPData(bl_BMPData *_data);

#endif