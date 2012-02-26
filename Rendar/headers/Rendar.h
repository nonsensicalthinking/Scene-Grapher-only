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
#include <list>
#include "MaterialManager.h"
#include "ModelManager.h"
#include "Camera.h"
#include "bsptree.h"

using namespace std;

#define Z_NEAR 	0.1
#define Z_FAR 	700



class Rendar {

private:
	int winPtr;	// glut window id
	int* r_width;
	int* r_height;
	int* r_full;
	int* r_fov;
	int* r_wireFramePolys;
	Camera* cam;	// camera

	string *modelPath;
	string *imagePath;

	bsp_node_t* bspRoot;

	int cachedPolygonCount;

public:
	MaterialManager* matsMgr;
	ModelManager* modelMgr;

	Rendar(string windowTitle);
	virtual ~Rendar();
	void gl_Init();
	void run(void);
	void lighting(void);
	void draw(void);
	void changeSize(int w, int h);
	void renderPolygonList(list<polygon_t*> polygons);
	void drawPolygon(polygon_t* poly);
	void renderBSPTree(bsp_node_t* tree);
	int countPolygonsInTree(bsp_node_t* tree);
	void bspFromObjModel(string modelName);
	void glCachePolygon(polygon_t* polygon);
	void nameAndCachePolygons(bsp_node_t* bspNode);
	void unCachePolygons(bsp_node_t* bspNode);
	void LoadModel(string path);
	int getCachedPolygonCount();
	Camera* getCamera();
};

#endif /* RENDAR_H_ */
