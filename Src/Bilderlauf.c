// Bilderlauf.c

#include "Bilderlauf.h"
#include "debug.h"

#define ESC        27

enum {X=0, Y=1, Z=2, W=3};
enum {DRAW_HEXAGON=0,DRAW_CUBE=1};

/*Globale Variablen: */

int drawMode = DRAW_HEXAGON;


//Window Settings
float    eyez=10., diag=0., viewScale=1., angle[3]={0.,0.,0.};
int      backF=1, persp=1, winWidth=1636, winHeight=920; 
GLdouble nah=1., fern=10000.;

clock_t fr_begin, fr_end; char *title;

//Hexagon Variables
float bl_hexasize = 1.f, bl_hexaheight, bl_hexawidth, bl_hexavert;
GLfloat xOffset = .2, yOffset = .2;
GLfloat hexa_vertices[12][3];
float height_scale = 1.f;

//Cube Variables
float bl_cubesize = 1.f;

//Mouse Variables
GLfloat deltaAngle = .0f;
int xOrigin = -1;

//Camera
int cameraCurrent = 0;
bl_Camera *cameras[2];

//BMP Load Structures
BL_BITMAPINFOHEADER bitmapInfoHeader;
unsigned char *bitmapData;
bl_BMPData *bl_PictureData;


//Calculates vertices/properties of a hexagon 
void createHexagonVertices(float size, float height){

	int i;
	double angle_rad;

	for(i = 0; i < 6; i++){
		angle_rad = M_PI / 180 * (60*i + 30);

		hexa_vertices[i][0] = size * cos(angle_rad);
		hexa_vertices[i+6][0] = hexa_vertices[i][0];

		hexa_vertices[i][1] = size * sin(angle_rad);
		hexa_vertices[i+6][1] = hexa_vertices[i][1];

		hexa_vertices[i][2] = 0;
		hexa_vertices[i+6][2] = height;
	}

	bl_hexaheight = size*2;
	bl_hexawidth = sqrt(3.f)/2* bl_hexaheight;
	bl_hexavert = height*2 * 3/4;
}

//Draws a hexagon at x,y position
/*************************************************************************/
void drawHexagon(int x, int y, bl_BMPData *data)
	/*************************************************************************/
{ 
	GLfloat transX, transY, transZ;
	GLint i,j;

	static GLint side_faces[6][4] =
	{
		{0, 6, 7, 1},
		{1, 7, 8, 2},
		{2, 8, 9, 3},
		{3, 9, 10, 4},
		{4, 10, 11, 5},
		{5, 11, 6, 0}
	};

	//Position
	transX = bl_hexawidth * x;
	transY = bl_hexavert * y;
	transZ = data->bmpData[data->bmpWidth*y+x].Height;


	//translate x of odd rows
	if (y % 2 == 1){
		transX -= bl_hexawidth / 2;
	}

	//Additional offset
	transX += xOffset * x;
	transY += yOffset * y;

	//Translation & Color of Hexagon
	glTranslatef(transX, transY, transZ);
	glColor3f(data->bmpData[data->bmpWidth*y+x].R,data->bmpData[data->bmpWidth*y+x].G,data->bmpData[data->bmpWidth*y+x].B);

	//Top and bottom face
	for(i = 0; i < 2; i++){
		glBegin(GL_POLYGON);
		for(j = 0; j < 6; j++){
			glVertex3fv(hexa_vertices[j+i*6]); }
		glVertex3fv(hexa_vertices[i*6]);
		glEnd();
	}

	//Side faces
	for (i = 0; i < 6; i++) {
		glBegin(GL_QUADS);
		glVertex3fv(hexa_vertices[side_faces[i][0]]);
		glVertex3fv(hexa_vertices[side_faces[i][1]]);
		glVertex3fv(hexa_vertices[side_faces[i][2]]);
		glVertex3fv(hexa_vertices[side_faces[i][3]]);
		glEnd();
	}

}

//Draws a cube at x, y position
void drawCube(int x, int y, bl_BMPData *data){
	GLfloat transX, transY, transZ;

	//Position
	transX = bl_cubesize * x;
	transY = bl_cubesize * y;
	transZ = data->bmpData[data->bmpWidth*y+x].Height;

	//Additional offset
	transX += xOffset * x;
	transY += yOffset * y;

	glTranslatef(transX, transY, transZ);
	glColor3f(data->bmpData[data->bmpWidth*y+x].R,data->bmpData[data->bmpWidth*y+x].G,data->bmpData[data->bmpWidth*y+x].B);

	glutSolidCube(bl_cubesize);
}

//Draw routine
/*************************************************************************/
void draw(void)
	/*************************************************************************/
{ 
	int i,j;

	fr_begin = clock();

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluPerspective( 80, winWidth / winHeight, 0.1, fern );

	gluLookAt
		( 
		cameras[cameraCurrent]->Position.X, cameras[cameraCurrent]->Position.Y, cameras[cameraCurrent]->Position.Z,
		cameras[cameraCurrent]->LookAt.X,cameras[cameraCurrent]->LookAt.Y,cameras[cameraCurrent]->LookAt.Z,
		0, 1, 0
		);

#if DEBUG > 0
	bl_CameraInfo(cameras[cameraCurrent]);
#endif

	glMatrixMode( GL_MODELVIEW );

	for(i = 0; i < bl_PictureData->bmpHeight; i++){
		for(j = 0; j < bl_PictureData->bmpWidth; j++){
			glPushMatrix();
			if(drawMode == DRAW_HEXAGON){
				drawHexagon(j,i, bl_PictureData);
			}else if(drawMode == DRAW_CUBE){
				drawCube(j,i, bl_PictureData);
			}
			glPopMatrix();
		}
	}

	fr_end = clock();

	glutSwapBuffers();

}

/*Eingabe-Behandlung:*/
/*************************************************************************/
void key(unsigned char key, int x, int y)
	/*************************************************************************/
	/*Menue und Eingabe-Behandlung:*/
{ 
	switch (key)
	{ case ESC: exit(0); break;
	case 'q': cameras[0]->LookAt.X += 0.01; break;
	case 'a': cameras[0]->LookAt.X -= 0.01; break;
	case 'w': cameras[0]->LookAt.Y += 0.5; break;
	case 's': cameras[0]->LookAt.Y -= 0.5; break;
	case 'e': cameras[0]->LookAt.Z += 0.5; break;
	case 'd': cameras[0]->LookAt.Z -= 0.5; break;

	case 'r': drawMode = !drawMode;
	}
	glutPostRedisplay();
	return;
}

void idle(void){
	glutPostRedisplay();
}


void processSpecialKeys(int key, int xx, int yy) {
	/*
	float fraction = 0.1f;

	switch (key) {
	case GLUT_KEY_LEFT :
	cameraAngle -= 0.01f;
	cameraDirX = sin(cameraAngle);
	cameraDirZ = -cos(cameraAngle);
	break;
	case GLUT_KEY_RIGHT :
	cameraAngle += 0.01f;
	cameraDirX = sin(cameraAngle);
	cameraDirZ = -cos(cameraAngle);
	break;
	case GLUT_KEY_UP :
	cameraDirX += cameraDirX * fraction;
	cameraDirZ += cameraDirZ * fraction;
	break;
	case GLUT_KEY_DOWN :
	cameraDirX -= cameraDirX * fraction;
	cameraDirZ -= cameraDirZ * fraction;
	break;
	}*/
}

void mouseMove(int x, int y){

	if (xOrigin >= 0){

		deltaAngle = (x- xOrigin) * .001f;


	}


}




/*************************************************************************/
int main(int argc, char **argv)
	/*************************************************************************/
{ 

	char prgNameBuffer[50];
	char filePath[FILENAMEBUFFER];
	char fileName[64];
	char fileExt[8];
	FILE *file;
	clock_t cl_begin, cl_end;

	///
	srand(time(NULL));
	sprintf(prgNameBuffer, "Bilderlauf %d.%d", MVERSION, SVERSION);

	//Open with file
	if(argc>1){
		strcpy(filePath, argv[1]);
	}else{

		//Get picture from console input
		printf("\n\nBilderlauf %d.%d\n\n\n", MVERSION, SVERSION);
		printf("----------------------------------\n\n");
		printf("Path of BMP file:\n");

		fgets(filePath, FILENAMEBUFFER, stdin);
		strtok(filePath, "\n");

		//DEBUG
		if( strcmp(filePath, "-") == 0)
			strcpy(filePath, "C:\\VSProjects\\depp4.bmp");
		printf("\n");

	}

	//Start timer
	cl_begin = clock();

	//Check if input is a valid file
	file = fopen(filePath, "r");

	if(!file) {
		printf("Invalid path!\n"); getchar();
		exit(1);
	}

	fclose(file);
	_splitpath(filePath, NULL, NULL, fileName, fileExt);

	if(strcmp(fileExt, ".bmp") != 0){
		printf("%s is not a valid BMP file!\n", fileName); getchar();
		exit(1);
	}


	//Start GLUT Window, hide console
#if DEBUG == 0
	//FreeConsole();
#endif

	//Load BMP File
	bitmapData = LoadBitmapFile(filePath,&bitmapInfoHeader);
	if(bitmapData == NULL){
		perror("Failed to load BMP file!\n");
		exit(11);
	}

	bl_PictureData = CreateBMPData(bitmapData, &bitmapInfoHeader);
	free(bitmapData);

#if DEBUG > 0
	printBMPData(bl_PictureData);
#endif

	//Calculate height
	calculateHeight(bl_PictureData, height_scale);

	cl_end = clock();

	printf("Loaded \"%s\" in %.3f seconds\n", filePath,
		(float)(cl_end-cl_begin) / CLOCKS_PER_SEC);


	createHexagonVertices(bl_hexasize, 1.f);


	//Basecamera Position
	cameras[0] = CreateCamera(CreateCameraPosition(0,-5,3.5), BL_CAM_TOP);
	cameras[1] = CreateCamera(CreateCameraPosition(0,0,1), BL_CAM_FPP);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_MULTISAMPLE);

	glFrustum (-1.0, 1.0, -1.0, 1.0, nah, fern); 
	glutInitWindowSize(winWidth, winHeight);
	glutCreateWindow(prgNameBuffer);



	glEnable(GL_DEPTH_TEST);
	glClearColor(0,0.75f,0.75f,1.f);

	//glEnable (GL_POINT_SMOOTH); 
	//glEnable (GL_LINE_SMOOTH);  glEnable (GL_BLEND); 
	//glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	glutDisplayFunc(draw);

	glutKeyboardFunc(key); 
	glutSpecialFunc(processSpecialKeys);

	/*
	glutSpecialFunc(pressKey);

	// here are the new entries
	glutIgnoreKeyRepeat(1);
	glutSpecialUpFunc(releaseKey);
	*/

	glutMainLoop();
	return 0;
}