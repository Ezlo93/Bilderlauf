#include "bl_bmp.h"
#include "debug.h"

//Creates a usable bl_BMPData structure from bmpData and infoheader
bl_BMPData *CreateBMPData(unsigned char *bmpData, BL_BITMAPINFOHEADER *bitmapInfoHeader){

	int i,j;
	bl_Color min, max;
	bl_BMPData* data;
	int pad = 0;

	//Allocate memory for BMPData
	data = (bl_BMPData*) malloc(sizeof(bl_BMPData));
	if(data == NULL){
		perror("Allocating memory failed!");
		exit(10);
	}

	//Init min, max Color
	min.R = MAX_COLOR; min.G = MAX_COLOR; min.B = MAX_COLOR;
	max.R = MIN_COLOR; max.G = MIN_COLOR; max.B = MIN_COLOR; 

	//Set Width, Height
	data->bmpHeight = bitmapInfoHeader->biHeight;
	data->bmpWidth = bitmapInfoHeader->biWidth;


	//Allocate memory for color array
	data->bmpData = (bl_Color*) malloc(data->bmpHeight * data->bmpWidth * sizeof(bl_Color));
	if(data->bmpData == NULL){
		perror("Allocating memory failed!");
		exit(10);
	}

	//Fill the array
	if ((bitmapInfoHeader->biWidth * 3) % 4 != 0) pad = 4 - ((bitmapInfoHeader->biWidth * 3) % 4);

	for(i = 0; i < data->bmpHeight; i++){
		for(j = 0; j < data->bmpWidth; j++){
			data->bmpData[i*data->bmpWidth+j].R = (float)bmpData[(i*data->bmpWidth+j)*3+(i*pad)]/255.f;
			data->bmpData[i*data->bmpWidth+j].G = (float)bmpData[(i*data->bmpWidth+j)*3+(i*pad)+1]/255.f;
			data->bmpData[i*data->bmpWidth+j].B = (float)bmpData[(i*data->bmpWidth+j)*3+(i*pad)+2]/255.f;
			data->bmpData[i*data->bmpWidth+j].Height = 0.f;

			//Update min/max if necessary
			if( ColorCompare(&data->bmpData[i*data->bmpWidth+j], &max) == 0) max = data->bmpData[i*data->bmpWidth+j];
			if( ColorCompare(&data->bmpData[i*data->bmpWidth+j], &min) == 1) min = data->bmpData[i*data->bmpWidth+j];

		}
	}

	//assign final min/max values
	data->maxColor = max;
	data->minColor = min;


#if DEBUG > 1
	//print all pixels
	printf("Final Color Data:\n");
	for(i = 0; i < data->bmpWidth*data->bmpHeight; i++){
		printf("%d:\t%f %f %f\n", i, data->bmpData[i].R*255, data->bmpData[i].G*255, data->bmpData[i].B*255);
	}

#endif


	return data;
}



//Prints info about the BMPData structure
void printBMPData(bl_BMPData *data){

	printf("\nBMP DATA\n");
	printf("WIDTH: %d, HEIGHT: %d\nMIN: %f:%f:%f\nMAX: %f:%f:%f\n", data->bmpWidth,data->bmpHeight, 
		data->minColor.R,data->minColor.G,data->minColor.B,data->maxColor.R,data->maxColor.G, data->maxColor.B);

	printf("SIZE: %.1f kb\n", (sizeof(bl_BMPData) + data->bmpHeight * data->bmpWidth * sizeof(bl_Color))/1024.f);

}


//Calculates the individual height of the hexagons/cubes
void calculateHeight(bl_BMPData *data, float scale){
	int i;
	float c;

	float max = (data->maxColor.R + data->maxColor.G + data->maxColor.B)/3;
	float min = (data->minColor.R + data->minColor.G + data->minColor.B)/3;

	for(i = 0; i < data->bmpWidth*data->bmpHeight; i++){
		//Current color value
		c = (data->bmpData[i].R +  data->bmpData[i].G + data->bmpData[i].B)/3;
		//scale the value so min -> 0 height, max -> scale height
		data->bmpData[i].Height = (c - min) / (max-min) * scale;

	}

}

//Adds all components of the colors and returns 0 if first is greater, 1 if second is greater
int ColorCompare(bl_Color* a, bl_Color* b){

	return (a->R + a->B + a->G) > (b->R + b->B + b->G) ? 0 : 1;

}


//Loads bmp file
unsigned char *LoadBitmapFile(char *filename, BL_BITMAPINFOHEADER *bitmapInfoHeader)
{
	FILE *filePtr;
	BL_BITMAPFILEHEADER bitmapFileHeader;
	unsigned char *bitmapImage;
	int imageIdx=0;
	unsigned char tempRGB;
	int bmpSize;
	int pad = 0, nextrow = 0, nextpadstep = 0;

	//open filename in read binary mode
	filePtr = fopen(filename,"rb");
	if (filePtr == NULL)
		return NULL;

	//read the bitmap file header
	fread(&bitmapFileHeader, sizeof(BL_BITMAPFILEHEADER),1,filePtr);

	//verify that this is a bmp file by checking bitmap id
	if (bitmapFileHeader.bfType !=0x4D42)
	{
		fclose(filePtr);
		return NULL;
	}

	//read the bitmap info header
	fread(bitmapInfoHeader, sizeof(BL_BITMAPINFOHEADER),1,filePtr);

	//move file point to the beginning of bitmap data
	fseek(filePtr, bitmapFileHeader.bOffBits, SEEK_SET);

	//allocate enough memory for the bitmap image data
	//CARE FOR PADDING (Lines must be % 32bits == 0 
	if ((bitmapInfoHeader->biWidth * 3) % 4 != 0) pad = 4 - ((bitmapInfoHeader->biWidth * 3) % 4);
	bmpSize = (bitmapInfoHeader->biHeight * bitmapInfoHeader->biWidth * 3) + (pad * bitmapInfoHeader->biHeight);
	bitmapImage = (unsigned char*)malloc(bmpSize);

	//verify memory allocation
	if (!bitmapImage)
	{
		free(bitmapImage);
		fclose(filePtr);
		return NULL;
	}

	//read in the bitmap image data
	fread(bitmapImage,bmpSize,1,filePtr);

	//make sure bitmap image data was read
	if (bitmapImage == NULL)
	{
		fclose(filePtr);
		return NULL;
	}

	//swap the r and b values to get RGB

#if DEBUG > 1
	for(imageIdx = 0; imageIdx < bmpSize; imageIdx++){
		printf("%d: %f\n", imageIdx, (float)bitmapImage[imageIdx]);
	}
#endif

	nextpadstep = bitmapInfoHeader->biWidth*3;
	for (imageIdx = 0;imageIdx < bmpSize;imageIdx+=3)
	{

		//MIND THE PADDING !!!
		if(imageIdx != 0 && (imageIdx+3) % nextpadstep == 0){
			imageIdx += pad;
			nextrow++;
			nextpadstep = (bitmapInfoHeader->biWidth*3*(nextrow+1) + nextrow*pad);
		}else{
			//if not at padstep swap colors
			tempRGB = bitmapImage[imageIdx];
			bitmapImage[imageIdx] = bitmapImage[imageIdx + 2];
			bitmapImage[imageIdx + 2] = tempRGB;
		}
	}


	fclose(filePtr);

#if DEBUG > 0
	printf("BitmapInfoHeader:\n");
	printf("\tbiSize:\t\t%d\n", bitmapInfoHeader->biSize);
	printf("\tbiWidth:\t%d\n", bitmapInfoHeader->biWidth);
	printf("\tbiHeight:\t%d\n", bitmapInfoHeader->biHeight);
	printf("\tbiPlanes:\t%d\n", bitmapInfoHeader->biPlanes);
	printf("\tbiBitcount:\t%d\n", bitmapInfoHeader->biBitCount);	
	printf("\tbiCompression:\t%d\n", bitmapInfoHeader->biCompression);
	printf("\tbiSizeImage:\t%d\n", bitmapInfoHeader->biSizeImage);
	printf("\tbiXPelsPerMeter:%d\n", bitmapInfoHeader->biXPelsPerMeter);
	printf("\tbiYPelsPerMeter:%d\n", bitmapInfoHeader->biYPelsPerMeter);
	printf("\tbiClrUsed:\t%d\n", bitmapInfoHeader->biClrUsed);
	printf("\tbiClrImportant:\t%d\n", bitmapInfoHeader->biClrImportant);
	printf("\tbmpSize:\t%d\n", bmpSize);
	printf("\tPadding:\t%d\n", pad);
#endif

	return bitmapImage;
}