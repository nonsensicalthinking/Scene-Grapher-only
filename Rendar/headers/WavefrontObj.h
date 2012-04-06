/*
 * objloader.h
 *
 *  Created on: Jul 4, 2010
 *      Author: Derek Brooks
 *
 *
 *      This file is for loading Wavefront OBJ files into the scene.
 *      It also relies on the rest of the package to do its job.
 */

#include "shared.h"
#include "strtools.h"
#include "MaterialManager.h"
#include <iostream>
#include <vector>
#include <stdio.h>

// now defined in strtools.h
//#define WHITESPACE " \t\n\r"


using namespace std;

// WARNING: Both texture manager and scene need to
// be instantiated before calling this function
// unpredicted things happen otherwise and the resulting
// errors will be difficult to track down.
// Needed to load textures into the GL instance
extern MaterialManager* getMaterialManager();

//#define OBJDEBUG


#ifndef OBJLOADER_CPP_
#define OBJLOADER_CPP_

#define MAX_OBJ_LINE_LEN	256


class ObjModel	{
	vector<vector<float> > v;
	vector<vector<float> > vt;
	vector<vector<float> > vn;

	float x_max;
	float x_min;
	float y_max;
	float y_min;
	float z_max;
	float z_min;

	char curMat[MAX_MAT_NAME_LEN];

public:
	string error;
	polygon_t* polygonList;
	int polygonListCount;

	ObjModel();
	~ObjModel();
	bool loadObjFile(string s);
	void doMaxCheck(float val, char axis);
	void processOBJLine(char line[MAX_OBJ_LINE_LEN]);
	void makeFace(vec3_t face[], int numPts, bool isTextured);
	void initializeMaterial(material_t* mat);
	bool loadMTLFile(const char* fileName);
	void getDimensions(vec3_t dim);
};



#endif /* OBJLOADER_CPP_ */
