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
#include "Font.h"

using namespace std;

#define Z_NEAR 	0.1
#define Z_FAR 	700


class Rendar {

private:
	int winPtr;	// glut window id
	int cachedPolygonCount;

	// cvar pointers
	int* r_width;
	int* r_height;
	int* r_full;
	int* r_fov;
	int* r_showFPS;
	int* r_wireFramePolys;
	string *modelPath;
	string *imagePath;

	Camera* cam;
	Font* screenPrinter;

	list<entity_t*> dynamicModels;

	map<int, entity_t*> gameModels;


public:
	bsp_node_t* bspRoot;

	int frameRate;

	MaterialManager* matsMgr;
	ModelManager* modelMgr;

	Rendar(string windowTitle);
	virtual ~Rendar();
	void gl_Init();
	void run(void);
	void lighting(void);
	void tabulateFrameRate();
	void drawFPS();
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
	void addEntityToScene(string modelName, vec3_t pos, vec3_t facing, int id);
	void renderDynamicModels(float dt);
	Camera* getCamera();
	void getCameraPos(vec3_t v);
	void getCameraFacing(vec3_t v);
};

#endif /* RENDAR_H_ */
