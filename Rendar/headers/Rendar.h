/*
 * rendar.h
 *
 *  Created on: Jan 30, 2012
 *      Author: brof
 */



#ifndef RENDAR_H_
#define RENDAR_H_
#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <string>
#include "MaterialManager.h"

using namespace std;

#define Z_NEAR 	0.1
#define Z_FAR 	700



class Rendar {

private:
	int winPtr;	// glut window id
	int* scr_width;
	int* scr_height;
	int* scr_full;
	int* scr_fov;

public:
	MaterialManager* matsMgr;

	Rendar(string windowTitle);
	virtual ~Rendar();
	void init();
	void run(void);
	void draw(void);
	void changeSize(int w, int h);
};

#endif /* RENDAR_H_ */
