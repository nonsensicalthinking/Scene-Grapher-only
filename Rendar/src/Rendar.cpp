/*
 * rendar.cpp
 *
 *  Created on: Jan 30, 2012
 *      Author: brof
 */

#include <iostream>
#include <string>
#include "Rendar.h"

using namespace std;

extern void drawConsole();
extern void Con_print(const char* fmt, ...);
extern int* getCvarAddress_I(string s);
extern double* getCvarAddress_D(string s);
extern string* getCvarAddress_S(string s);

Rendar::Rendar(string windowTitle) {

	scr_width = getCvarAddress_I("scr_width");
	scr_height = getCvarAddress_I("scr_height");
	scr_full = getCvarAddress_I("scr_full");
	scr_fov = getCvarAddress_I("src_fov");

	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(0,0);
	glutInitWindowSize(*scr_width, *scr_height);

	matsMgr = new MaterialManager();
	winPtr = glutCreateWindow(windowTitle.c_str());
	init();

}

Rendar::~Rendar() {

	glutLeaveMainLoop();
	cout << "Shutdown: Main loop exited." << endl;

	glutDestroyWindow(winPtr);
	cout << "Shutdown: Window destroyed." << endl;

	if( matsMgr )	{
		delete matsMgr;
		cout << "Shutdown: Material Manager Unloaded." << endl;
	}



}

void Rendar::init()	{

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
	glEnable(GL_CULL_FACE);
	glutSetCursor(GLUT_CURSOR_NONE);	// hide the mouse cursor

	//load basic textures for fonts and console

	// Used to call user code to load game here.
}


void Rendar::run(void)	{
	glutMainLoop();
}


void Rendar::draw(void)	{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// position camera
	gluLookAt(	0.0, 0.0, 0.0,	// camera origin
				0.0, 0.0, -1.0,		// eye looking @ this vertex
				0.0, 1.0, 0.0);	// up direction

//	glutWarpPointer(400, 300);	// center mouse





	// render the console
	drawConsole();

	glutSwapBuffers();

}

void Rendar::changeSize(int width, int height)	{
	*scr_width = width;
	*scr_height = height;

	double asratio;

    if(height == 0)
    	height = 1; //to avoid divide-by-zero

    asratio = width / (double) height;

    glViewport(0, 0, width, height); //adjust GL viewport

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(*scr_fov, asratio, Z_NEAR, Z_FAR); //adjust perspective
    glMatrixMode(GL_MODELVIEW);
}


