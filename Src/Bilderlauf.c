// Bilderlauf.c

#include "Bilderlauf.h"
#include "debug.h"

/*Globale Variablen: */

int drawMode = DRAW_HEXAGON;
int init_time, final_time, frame_count=0;
int deltaTime, deltaTimeStart, deltaTimeStartOld;
float fps_limit;
int input[5];
float frustum[6][4];

int wireframemode = 0;
int edgecoloring = 1;
int anaglyph = 0;

//Window Settings
float    eyez=10., diag=0., viewScale=1., angle[3]={0.,0.,0.};
int      backF=1, persp=1, winWidth=1636, winHeight=920; 
GLdouble nah=1., fern=10000.;

char *title;

//Hexagon Variables
float bl_hexasize = HEXAGONSIZE, bl_hexaheight, bl_hexawidth, bl_hexavert;
GLfloat hexa_vertices[12][3];
GLfloat hexa_vertices_sw[12][3];

float height_scale = HEXAGONMAXHEIGHT;

//Mouse Variables
GLfloat deltaAngle = .0f;
int xOrigin = -1;

//Player
bl_Character* bl_player;

//Camera
int cameraCurrent = 0;
bl_Camera *cameras[CAMERAMAX];
float rotation_speed = M_PI/180*0.1f;
int cameraReverseX = 1, cameraReverseY = -1;


//BMP Load Structures
BL_BITMAPINFOHEADER bitmapInfoHeader;
unsigned char *bitmapData;
bl_BMPData *bl_PictureData;



//Calculates vertices/properties of a hexagon 
void createHexagonVertices(float _size, float _height){

	int i;
	double angle_rad;

	//reduce size temporarily to draw slightly smaller hexagons
	//in order to create a little space between them so that the
	//camera can't glitch into them
	_size -= HEXAGONOFFSET;

	for(i = 0; i < 6; i++){
		angle_rad = M_PI / 180 * (60*i + 30);


		hexa_vertices[i][0] = _size * cos(angle_rad);
		hexa_vertices[i+6][0] = hexa_vertices[i][0];

		hexa_vertices[i][1] = _size * sin(angle_rad);
		hexa_vertices[i+6][1] = hexa_vertices[i][1];

		hexa_vertices[i][2] = 0;
		hexa_vertices[i+6][2] = _height;

		//switched hexagons
		hexa_vertices_sw[i][0] = hexa_vertices[i][0];
		hexa_vertices_sw[i+6][0] = hexa_vertices[i+6][0];

		hexa_vertices_sw[i][1] = hexa_vertices[i][2];
		hexa_vertices_sw[i+6][1] = hexa_vertices[i+6][2];

		hexa_vertices_sw[i][2] = hexa_vertices[i][1];
		hexa_vertices_sw[i+6][2] = hexa_vertices[i+6][1];

	}

	_size += HEXAGONOFFSET;

	bl_hexaheight = _size*2; 
	bl_hexawidth = sqrt(3.f) * _size;
	bl_hexavert = bl_hexaheight * 3/4;
}

//Draws a hexagon at x,y position
/*************************************************************************/
void drawHexagon(int _x, int _y, bl_BMPData *_data, int _mode, float _opacity)
	/*************************************************************************/
{ 
	GLfloat transX, transY, transZ;
	GLint i,j;

	//Vertices of the side faces
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
	transX = bl_hexawidth * _x;
	transY = _data->bmpData[_data->bmpWidth*_y+_x].Height * _opacity;
	transZ = bl_hexavert * _y;


	//translate x of odd rows
	if (_y % 2 == 1){
		transX -= bl_hexawidth / 2;
	}

	//Translation & Color of Hexagon
	glTranslatef(transX, transY, transZ);
	glColor4f(_data->bmpData[_data->bmpWidth*_y+_x].R,_data->bmpData[_data->bmpWidth*_y+_x].G,_data->bmpData[_data->bmpWidth*_y+_x].B, _opacity);


	//Change height of hexagon

	for(i = 0; i < 6; i++){
		hexa_vertices_sw[i][1] = -transY;
	}

	//if not wireframe mode draw hexagon

	if(_mode != 2){

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glLineWidth(1.f);

		//Top and bottom face
		for(i = 0; i < 2; i++){
			glBegin(GL_POLYGON);

			for(j = 5; j >= 0; j--){
				glVertex3fv(hexa_vertices_sw[j+i*6]); }
			glEnd();
		}

		//Side faces
		for (i = 0; i < 6; i++) {
			glBegin(GL_QUADS);
			glVertex3fv(hexa_vertices_sw[side_faces[i][0]]);
			glVertex3fv(hexa_vertices_sw[side_faces[i][1]]);
			glVertex3fv(hexa_vertices_sw[side_faces[i][2]]);
			glVertex3fv(hexa_vertices_sw[side_faces[i][3]]);
			glEnd();
		}
	}

	//color of the edge
	if(_mode == 2){
		glColor4f(_data->bmpData[_data->bmpWidth*_y+_x].R,_data->bmpData[_data->bmpWidth*_y+_x].G,_data->bmpData[_data->bmpWidth*_y+_x].B, _opacity);
	}else if(_mode == 0 || _mode == 3){
		glColor4f(0,0,0,_opacity);
	}else{
		return;
	}

	//draw edge
	glLineWidth(2.f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


	for(i = 0; i < 2; i++){
		glBegin(GL_POLYGON);

		for(j = 5; j >= 0; j--){
			glVertex3fv(hexa_vertices_sw[j+i*6]); }
		glEnd();
	}

	for (i = 0; i < 6; i++) {
		glBegin(GL_QUADS);
		glVertex3fv(hexa_vertices_sw[side_faces[i][0]]);
		glVertex3fv(hexa_vertices_sw[side_faces[i][1]]);
		glVertex3fv(hexa_vertices_sw[side_faces[i][2]]);
		glVertex3fv(hexa_vertices_sw[side_faces[i][3]]);
		glEnd();
	}
}

//Draw routine
/*************************************************************************/
void draw(void)
	/*************************************************************************/
{ 
	int i,j,k, mode = 0, draw_c = 0;
	int drawborder_left, drawborder_right, drawborder_top, drawborder_bottom;
	float cullx, cully, cullz, opacity = 1.f, distance;
	//double left, right, top, bottom;
	double eyeOffset = 0.15, eyeX, eyeY;

	/*
	mode:
	- 0 filled hexagons and edgecoloring
	- 1 only filled hexagons
	- 2 colored wireframe
	- 3 anaglyph
	*/

	if(wireframemode){
		mode = 2;
	}else if(!edgecoloring){
		mode = 1;
	}
	
	if(anaglyph){
		mode = 3;
	}

	//run render once or twice, depends on stereoscopic
	for(k = 0; k < (anaglyph?2:1);k++){

		//setup for red/blue render
		if(anaglyph){

			if(k == 0){
				glDrawBuffer(GL_BACK);
				glReadBuffer(GL_BACK);
				glClear(GL_ACCUM_BUFFER_BIT);
			}

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
			if (k == 0) {
				glColorMask(GL_TRUE, GL_FALSE, GL_FALSE, GL_TRUE);
			}
			else {
				glDrawBuffer(GL_BACK);
				glColorMask(GL_FALSE, GL_FALSE, GL_TRUE, GL_TRUE);
			}

			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();

			gluPerspective(FOV, winWidth / winHeight, 0.1, fern);


			if(k == 0){
				eyeX = (float)(cos(cameras[cameraCurrent]->yaw + (M_PI * 1.5f))) * eyeOffset;
				eyeY = (float)(-sin(cameras[cameraCurrent]->yaw + (M_PI * 1.5f))) * eyeOffset;
				glTranslatef(eyeX, 0.0, eyeY);
			}else{
				eyeX = (float)(cos(cameras[cameraCurrent]->yaw + (M_PI * 0.5f))) * eyeOffset;
				eyeY = (float)(-sin(cameras[cameraCurrent]->yaw + (M_PI * 0.5f))) * eyeOffset;
				glTranslatef(eyeX, 0.0, eyeY);
			}

			bl_CameraUpdate(cameras[cameraCurrent]);

			gluLookAt
				(
				cameras[cameraCurrent]->Position.X, cameras[cameraCurrent]->Position.Y, cameras[cameraCurrent]->Position.Z,
				cameras[cameraCurrent]->Position.X + cameras[cameraCurrent]->LookAt.X,
				cameras[cameraCurrent]->Position.Y + cameras[cameraCurrent]->LookAt.Y,
				cameras[cameraCurrent]->Position.Z + cameras[cameraCurrent]->LookAt.Z,
				0, 1, 0
				);

			glMatrixMode(GL_MODELVIEW);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glLineWidth(1.f);
			glFlush();



			//setup for normal render
		}else{


			glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

			glMatrixMode( GL_PROJECTION );
			glLoadIdentity();
			gluPerspective( FOV, winWidth / winHeight, 0.1, fern );

			bl_CameraUpdate(cameras[cameraCurrent]);

			gluLookAt
				( 
				cameras[cameraCurrent]->Position.X, cameras[cameraCurrent]->Position.Y, cameras[cameraCurrent]->Position.Z,
				cameras[cameraCurrent]->Position.X+cameras[cameraCurrent]->LookAt.X,		
				cameras[cameraCurrent]->Position.Y+cameras[cameraCurrent]->LookAt.Y,
				cameras[cameraCurrent]->Position.Z+cameras[cameraCurrent]->LookAt.Z,
				0, 1, 0
				);

			glMatrixMode( GL_MODELVIEW );
		}


		//actual drawing of the hexagons
		ExtractFrustum();


		drawborder_top = (bl_player->positionOnGridY - DRAWDISTANCE_GRID) < 0 ? 0 : (bl_player->positionOnGridY - DRAWDISTANCE_GRID);
		drawborder_bottom = (bl_player->positionOnGridY + DRAWDISTANCE_GRID) > bl_PictureData->bmpHeight ? bl_PictureData->bmpHeight : (bl_player->positionOnGridY + DRAWDISTANCE_GRID);

		drawborder_left = (bl_player->positionOnGridX - DRAWDISTANCE_GRID) < 0 ? 0 : (bl_player->positionOnGridX - DRAWDISTANCE_GRID);
		drawborder_right = (bl_player->positionOnGridX + DRAWDISTANCE_GRID) > bl_PictureData->bmpWidth ? bl_PictureData->bmpWidth : (bl_player->positionOnGridX + DRAWDISTANCE_GRID);

		for(i = drawborder_top;	i < drawborder_bottom; i++){

			for (j = drawborder_left; j < drawborder_right;	j++) {

				//position of hexagon in question
				cullx = bl_hexawidth * j;
				cully = bl_PictureData->bmpData[bl_PictureData->bmpWidth*i+j].Height;
				cullz = bl_hexavert * i;

				//translate x of odd rows
				if (i % 2 == 1){
					cullx -= bl_hexawidth / 2;
				}

				//draw distance
				distance = (float)(sqrt(pow((double)(cullx-bl_player->x),2) + (pow((double)(cullz-bl_player->y),2))));

				if(distance > DRAWDISTANCE){
					continue;
				}else if(distance > (DRAWDISTANCE - DRAWDISTANCE_BLEND_DISTANCE)){
					opacity = (DRAWDISTANCE-distance) / DRAWDISTANCE_BLEND_DISTANCE;
				}else{
					opacity = 1.f;
				}

				if(SphereInFrustum(cullx,cully,cullz,bl_hexasize*HEXAGONMAXHEIGHT/2)){

					glPushMatrix();
					if(drawMode == DRAW_HEXAGON){
						drawHexagon(j,i, bl_PictureData,mode,opacity);
						draw_c++;
					}
					glPopMatrix();

				}
			}
		}
		if(anaglyph){glAccum(GL_ACCUM, 1.0);}
	}

	if(anaglyph){
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glAccum(GL_RETURN, 1.0);
	}

	glutSwapBuffers();
	frame_count++;
	final_time = time(NULL);
	if(final_time-init_time > 0){
#if DEBUG > 0
		printf("FPS: %d\n", frame_count /(final_time - init_time));
#endif
		frame_count = 0;
		init_time = time(NULL);
	}
}



//Input


void key(unsigned char _key, int _x, int _y)

{ 
	switch (_key)
	{ 
	case ESC: exit(0); break;
	case 'w': input[0] = 1; break;
	case 's': input[1] = 1; break;
	case 'a': input[2] = 1; break;
	case 'd': input[3] = 1; break;
	case SPACE: input[4] = 1; break;

#if DEBUG > 0
		//case 'r': drawMode = !drawMode; cameraCurrent = (cameraCurrent + 1) % CAMERAMAX;break;
	case 'p': bl_CameraInfo(cameras[cameraCurrent]);
		printf("Player @%d,%d\n", bl_player->positionOnGridX, bl_player->positionOnGridY); 
		printf("Height: %f\n", bl_PictureData->bmpData[bl_PictureData->bmpWidth*bl_player->positionOnGridY+bl_player->positionOnGridX].Height);break;
#endif

	}

	return;
}


void releaseKey(unsigned char _key, int _x, int _y){

	switch(_key){
	case 'w': input[0] = 0; break;
	case 's': input[1] = 0; break;
	case 'a': input[2] = 0; break;
	case 'd': input[3] = 0; break;
	case SPACE: input[4] = 0; break;

	case '1': wireframemode = !wireframemode;break;
	case '2': edgecoloring = !edgecoloring;break;
	case '3': anaglyph = !anaglyph;break;
	case '4': bl_player->isRunning = !bl_player->isRunning; break;
	case '5': 
		if (fps_limit == FPS_30) {
			fps_limit = FPS_60;
		}
		else if (fps_limit == FPS_60) {
			fps_limit = FPS_144;
		}
		else {
			fps_limit = FPS_30;
		}break;
			  
	}

}




void mouseMove(int _x, int _y){

	static int just_warped = 0;
	int dx = _x - winWidth/2;
	int dy = _y - winHeight/2;

	if(just_warped) {
		just_warped = 0;
		return;
	}

	if(dx) {
		bl_CameraRotateYaw(cameras[cameraCurrent],rotation_speed*dx*cameraReverseX);
	}

	if(dy) {
		bl_CameraRotatePitch(cameras[cameraCurrent],rotation_speed*dy*cameraReverseY);
	}

	glutWarpPointer(winWidth/2, winHeight/2);

	just_warped = 1;

}



//Update Loop
void timer(int _a) {
	unsigned int dtime = 0;

	deltaTimeStart = glutGet(GLUT_ELAPSED_TIME);
	deltaTime = deltaTimeStart - deltaTimeStartOld;
	deltaTimeStartOld = deltaTimeStart; 

	bl_UpdateCharacter(bl_player, &input, deltaTime/1000.f, bl_PictureData, bl_hexasize);
	glutPostRedisplay();
	glutTimerFunc(1000 / fps_limit, timer,0);

}


void reshaping(int _width, int _height) {
	glViewport(0, 0, _width, _height);
	winWidth = _width;
	winHeight = _height;
}


/*************************************************************************/
int main(int argc, char **argv)
	/*************************************************************************/
{ 

	char prgNameBuffer[50];
	char filePath[FILENAMEBUFFER], filePath2[FILENAMEBUFFER];
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
		printf("\n\nBilderlauf %d.%d\n", MVERSION, SVERSION);
		printf("Nicolai Sehrt, Johannes Hublitz\n\n\n");

		do{
			printf("---------------------------------\n");
			printf("Full path of BMP file: (h for help; test or thm for example)\n");
			fgets(filePath, FILENAMEBUFFER, stdin);
			strtok(filePath, "\n");

			if(strcmp(filePath, "h") == 0){
				printf("---------------------------------\n");
				printf("W A S D\t\tMovement\n");
				printf("Spacebar\tJump\n");
				printf("1\t\tWireframe\n");
				printf("2\t\tOutline edges\n");
				printf("3\t\tActivate stereoscopic mode (anaglyph)\n");
				printf("4\t\tIncrease speed and jump velocity\n");
				printf("5\t\tSwitch between 30/60/144 fps\n");
				printf("p\t\tPrint position information\n\n");
			}

		}while( strcmp(filePath, "h") == 0);

		//DEBUG
#if DEBUG > 0
		if( strcmp(filePath, "-") == 0)
			strcpy(filePath, "C://VSProjects//test.bmp");

		printf("\n");
#endif
	}

	//Start timer
	cl_begin = clock();

	//Check if input is a valid file
	file = fopen(filePath, "r");

	if(!file) {

		sprintf(filePath2, "bmp//%s.bmp", filePath);
		file = fopen(filePath2, "r");

		if(!file){
			printf("Invalid path!\n"); getchar();
			exit(1);
		}else{
			sprintf(filePath, "%s", filePath2);
		}
	}

	fclose(file);


	//Start GLUT Window, hide console
#if DEBUG == 0
	FreeConsole();
#endif

	//Load BMP File
	bitmapData = LoadBitmapFile(filePath,&bitmapInfoHeader);
	if(bitmapData == NULL){
		perror("Failed to load BMP file!\n"); getchar();
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
	cameras[0] = CreateCamera(CreateCameraPosition(0,2,0), BL_CAM_FPP);


	//Character init
	bl_player =	bl_CreateCharacter(0,0,bl_PictureData->bmpData[0].Height, BL_CHARACTER_HEIGHT , cameras[0]);

	fps_limit = FPS_60;

	//Glut Init
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE |  GLUT_DEPTH);

	//glFrustum (-1.0, 1.0, -1.0, 1.0, nah, fern); 
	glutInitWindowSize(winWidth, winHeight);
	glutCreateWindow(prgNameBuffer);

	glDisable(GL_DITHER);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//enable face culling
	glEnable  (GL_CULL_FACE);
	glCullFace(GL_BACK);

	glutDisplayFunc(draw);
	glutReshapeFunc(reshaping);

	//Glut input
	glutMotionFunc(mouseMove);
	glutPassiveMotionFunc(mouseMove);
	

	glutSetCursor(GLUT_CURSOR_NONE);

	glutKeyboardFunc(key); 
	glutIgnoreKeyRepeat(1);
	glutKeyboardUpFunc(releaseKey);

	deltaTimeStart = glutGet(GLUT_ELAPSED_TIME);
	deltaTimeStartOld = deltaTimeStart;
	init_time = time(NULL);


	timer(1);
	glutMainLoop();
	return 0;
}


void ExtractFrustum()
{
	float   proj[16];
	float   modl[16];
	float   clip[16];
	float   t;

	/* Get the current PROJECTION matrix from OpenGL */
	glGetFloatv( GL_PROJECTION_MATRIX, proj );

	/* Get the current MODELVIEW matrix from OpenGL */
	glGetFloatv( GL_MODELVIEW_MATRIX, modl );

	/* Combine the two matrices (multiply projection by modelview) */
	clip[ 0] = modl[ 0] * proj[ 0] + modl[ 1] * proj[ 4] + modl[ 2] * proj[ 8] + modl[ 3] * proj[12];
	clip[ 1] = modl[ 0] * proj[ 1] + modl[ 1] * proj[ 5] + modl[ 2] * proj[ 9] + modl[ 3] * proj[13];
	clip[ 2] = modl[ 0] * proj[ 2] + modl[ 1] * proj[ 6] + modl[ 2] * proj[10] + modl[ 3] * proj[14];
	clip[ 3] = modl[ 0] * proj[ 3] + modl[ 1] * proj[ 7] + modl[ 2] * proj[11] + modl[ 3] * proj[15];

	clip[ 4] = modl[ 4] * proj[ 0] + modl[ 5] * proj[ 4] + modl[ 6] * proj[ 8] + modl[ 7] * proj[12];
	clip[ 5] = modl[ 4] * proj[ 1] + modl[ 5] * proj[ 5] + modl[ 6] * proj[ 9] + modl[ 7] * proj[13];
	clip[ 6] = modl[ 4] * proj[ 2] + modl[ 5] * proj[ 6] + modl[ 6] * proj[10] + modl[ 7] * proj[14];
	clip[ 7] = modl[ 4] * proj[ 3] + modl[ 5] * proj[ 7] + modl[ 6] * proj[11] + modl[ 7] * proj[15];

	clip[ 8] = modl[ 8] * proj[ 0] + modl[ 9] * proj[ 4] + modl[10] * proj[ 8] + modl[11] * proj[12];
	clip[ 9] = modl[ 8] * proj[ 1] + modl[ 9] * proj[ 5] + modl[10] * proj[ 9] + modl[11] * proj[13];
	clip[10] = modl[ 8] * proj[ 2] + modl[ 9] * proj[ 6] + modl[10] * proj[10] + modl[11] * proj[14];
	clip[11] = modl[ 8] * proj[ 3] + modl[ 9] * proj[ 7] + modl[10] * proj[11] + modl[11] * proj[15];

	clip[12] = modl[12] * proj[ 0] + modl[13] * proj[ 4] + modl[14] * proj[ 8] + modl[15] * proj[12];
	clip[13] = modl[12] * proj[ 1] + modl[13] * proj[ 5] + modl[14] * proj[ 9] + modl[15] * proj[13];
	clip[14] = modl[12] * proj[ 2] + modl[13] * proj[ 6] + modl[14] * proj[10] + modl[15] * proj[14];
	clip[15] = modl[12] * proj[ 3] + modl[13] * proj[ 7] + modl[14] * proj[11] + modl[15] * proj[15];

	/* Extract the numbers for the RIGHT plane */
	frustum[0][0] = clip[ 3] - clip[ 0];
	frustum[0][1] = clip[ 7] - clip[ 4];
	frustum[0][2] = clip[11] - clip[ 8];
	frustum[0][3] = clip[15] - clip[12];

	/* Normalize the result */
	t = sqrt( frustum[0][0] * frustum[0][0] + frustum[0][1] * frustum[0][1] + frustum[0][2] * frustum[0][2] );
	frustum[0][0] /= t;
	frustum[0][1] /= t;
	frustum[0][2] /= t;
	frustum[0][3] /= t;

	/* Extract the numbers for the LEFT plane */
	frustum[1][0] = clip[ 3] + clip[ 0];
	frustum[1][1] = clip[ 7] + clip[ 4];
	frustum[1][2] = clip[11] + clip[ 8];
	frustum[1][3] = clip[15] + clip[12];

	/* Normalize the result */
	t = sqrt( frustum[1][0] * frustum[1][0] + frustum[1][1] * frustum[1][1] + frustum[1][2] * frustum[1][2] );
	frustum[1][0] /= t;
	frustum[1][1] /= t;
	frustum[1][2] /= t;
	frustum[1][3] /= t;

	/* Extract the BOTTOM plane */
	frustum[2][0] = clip[ 3] + clip[ 1];
	frustum[2][1] = clip[ 7] + clip[ 5];
	frustum[2][2] = clip[11] + clip[ 9];
	frustum[2][3] = clip[15] + clip[13];

	/* Normalize the result */
	t = sqrt( frustum[2][0] * frustum[2][0] + frustum[2][1] * frustum[2][1] + frustum[2][2] * frustum[2][2] );
	frustum[2][0] /= t;
	frustum[2][1] /= t;
	frustum[2][2] /= t;
	frustum[2][3] /= t;

	/* Extract the TOP plane */
	frustum[3][0] = clip[ 3] - clip[ 1];
	frustum[3][1] = clip[ 7] - clip[ 5];
	frustum[3][2] = clip[11] - clip[ 9];
	frustum[3][3] = clip[15] - clip[13];

	/* Normalize the result */
	t = sqrt( frustum[3][0] * frustum[3][0] + frustum[3][1] * frustum[3][1] + frustum[3][2] * frustum[3][2] );
	frustum[3][0] /= t;
	frustum[3][1] /= t;
	frustum[3][2] /= t;
	frustum[3][3] /= t;

	/* Extract the FAR plane */
	frustum[4][0] = clip[ 3] - clip[ 2];
	frustum[4][1] = clip[ 7] - clip[ 6];
	frustum[4][2] = clip[11] - clip[10];
	frustum[4][3] = clip[15] - clip[14];

	/* Normalize the result */
	t = sqrt( frustum[4][0] * frustum[4][0] + frustum[4][1] * frustum[4][1] + frustum[4][2] * frustum[4][2] );
	frustum[4][0] /= t;
	frustum[4][1] /= t;
	frustum[4][2] /= t;
	frustum[4][3] /= t;

	/* Extract the NEAR plane */
	frustum[5][0] = clip[ 3] + clip[ 2];
	frustum[5][1] = clip[ 7] + clip[ 6];
	frustum[5][2] = clip[11] + clip[10];
	frustum[5][3] = clip[15] + clip[14];

	/* Normalize the result */
	t = sqrt( frustum[5][0] * frustum[5][0] + frustum[5][1] * frustum[5][1] + frustum[5][2] * frustum[5][2] );
	frustum[5][0] /= t;
	frustum[5][1] /= t;
	frustum[5][2] /= t;
	frustum[5][3] /= t;
}

int SphereInFrustum( float x, float y, float z, float radius )
{
	int p;

	for( p = 0; p < 6; p++ )
		if( frustum[p][0] * x + frustum[p][1] * y + frustum[p][2] * z + frustum[p][3] <= -radius )
			return 0;
	return 1;
}