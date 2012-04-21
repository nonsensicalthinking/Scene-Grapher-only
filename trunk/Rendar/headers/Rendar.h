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
#include "entity.h"

using namespace std;

#define SKY_TEXTURE "partly_cloudy.bmp"

class Rendar {

private:

	int winPtr;	// glut window id
	int cachedPolygonCount;
	int skyCacheID;

	// cvar pointers
	int* r_width;
	int* r_height;
	int* r_full;
	int* r_fov;
	int* r_showFPS;
	int* r_wireFramePolys;
	double* zNear;
	double* zFar;
	string *modelPath;
	string *imagePath;

	Camera* cam;
	Font* screenPrinter;

	GLUquadric* sky;

	entity_t* dynamicModels;

	map<int, entity_t*> gameModels;


//	"Polygon count:"

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
	void renderPolygonList(polygon_t* polygons);
	void drawPolygon(polygon_t* poly);
	void renderBSPTree(bsp_node_t* tree);
//	int countPolygonsInTree(bsp_node_t* tree);
	void bspFromObjModel(string modelName);
	void glCachePolygon(polygon_t* polygon);
	void nameAndCachePolygons(bsp_node_t* bspNode);
	void unCachePolygons(bsp_node_t* bspNode);
	void LoadModel(string path);
	int getCachedPolygonCount();
	void addEntityToScene(string modelName, entity_t* ent);
	void renderDynamicModels(float dt);
	Camera* getCamera();
	void setAnimation(entity_t* e, string animName);
	void getCameraPos(vec3_t v);
	void getCameraFacing(vec3_t v);
	void cacheSky();
	void screenPrint(int x, int y, const char* fmt, ...);
	void screenPrint(int x, int y, string str);


	void printTextures();

};

#endif /* RENDAR_H_ */
