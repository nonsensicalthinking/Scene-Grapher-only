/*
 * rendar.cpp
 *
 *  Created on: Jan 30, 2012
 *      Author: brof
 */

#include <iostream>
#include <string>
#include <stdlib.h>
#include "Rendar.h"
#include "shared.h"
#include "WavefrontObj.h"
#include "md2model.h"

#define MOUSELOOK

using namespace std;

extern long Sys_Milliseconds();
extern void drawConsole();
extern void Con_print(const char* fmt, ...);
extern int* getCvarAddress_I(string s);
extern double* getCvarAddress_D(string s);
extern string* getCvarAddress_S(string s);
extern void registerCvar(string name, string value, int typeFlag);
extern string getCWD();
extern void registerCommand(string name, void (*func)());
extern void registerCommandWithArgs(string name, void (*func)(string), bool hasArgs);
extern string getProgramPath();

double* r_modelAdvRate;

void Rendar::printTextures()	{
	matsMgr->printTextureList();
}


Rendar::Rendar(string windowTitle) {
	cachedPolygonCount = 0;

	// register cvars
	registerCvar("r_wireframepolys", "0", 1);
	registerCvar("r_showFPS", "1", 1);

	// get addresses for newly registered cvars
	r_wireFramePolys = getCvarAddress_I("r_wireframePolys");
	r_width = getCvarAddress_I("r_width");
	r_height = getCvarAddress_I("r_height");
	r_full = getCvarAddress_I("r_full");
	r_fov = getCvarAddress_I("r_fov");
	r_showFPS = getCvarAddress_I("r_showFPS");
	zNear = getCvarAddress_D("z_nearClip");
	zFar = getCvarAddress_D("z_farClip");
	modelPath = getCvarAddress_S("r_modelPath");
	imagePath = getCvarAddress_S("r_imagePath");

	// TODO REMOVE, THIS IS DEBUGGING FOR MD2 MODELS
	registerCvar("r_modelAdvRate", "0.01", 2);
	r_modelAdvRate = getCvarAddress_D("r_modelAdvRate");
	// END TODO REMOVE

	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(0,0);
	glutInitWindowSize(*r_width, *r_height);

	matsMgr = new MaterialManager();
	modelMgr = new ModelManager();
	winPtr = glutCreateWindow(windowTitle.c_str());

	gl_Init();

	cam = new Camera();
	bspRoot = NULL;
	dynamicModels = NULL;

}

Rendar::~Rendar() {

	glutLeaveMainLoop();
	cout << "Shutdown: Main loop exited." << endl;

	glutDestroyWindow(winPtr);
	cout << "Shutdown: GLWindow destroyed." << endl;

	if( matsMgr )	{
		delete matsMgr;
		cout << "Shutdown: Material Manager Unloaded." << endl;
	}

	if( modelMgr )	{
		delete modelMgr;
		cout << "Shutdown: Model Manager Unloaded." << endl;
	}

	if( bspRoot )	{
		deleteTree(bspRoot);
		cout << "Shutdown: BSP Tree & All Polygons Unloaded." << endl;
	}

}

void Rendar::cacheSky()	{
	sky = gluNewQuadric();
	gluQuadricTexture(sky, true);
	gluQuadricOrientation(sky, GLU_INSIDE);
	matsMgr->loadBitmap(SKY_TEXTURE);

	skyCacheID = glGenLists(1);
	glNewList(skyCacheID, GL_COMPILE);
	glPushMatrix();
	matsMgr->enableSphereMapping();
	matsMgr->bindTexture(SKY_TEXTURE);
	gluSphere(sky, 350, 10, 5);
	matsMgr->disableSphereMapping();
	glPopMatrix();
	glEndList();
}

void Rendar::gl_Init()	{

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glClearColor(0.75, 0.75, 0.75, 0.0);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
	glEnable(GL_CULL_FACE);

#ifdef MOUSELOOK
	glutSetCursor(GLUT_CURSOR_NONE);
#endif

	cacheSky();

	//load basic textures for fonts and console

	// Used to call user code to load game here.
}



void Rendar::run(void)	{
	// this cannot be created in the constructor.
	screenPrinter = new Font(1024,768);
	glutMainLoop();
}

void Rendar::lighting()	{
	GLfloat df = 1.0;
	GLfloat amb[]=	{0.9, 0.9, 0.9, 0};   		//global ambient

	GLfloat amb2[]=	{1, 1, 1, 1};  		//ambiance of light source
	GLfloat diff[]=	{1.0, 1.0, 1.0, 0};	// diffuse light
	GLfloat spec[]=	{1.0, 1.0, 1.0, 0};      	//sets specular highlight
	GLfloat posl[]=	{0, 5, 0, 0};            //position of light source

//	GLfloat posL1[] = {0, 5, 0};
//	GLfloat spotDir[] = {0, -1, 0};



	glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
//	glMaterialfv(GL_FRONT, GL_SHININESS, &df);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	// global ambiance
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, amb);
	glShadeModel(GL_SMOOTH);

	// Light 0
	glLightfv(GL_LIGHT0, GL_AMBIENT, amb2);
//	glLightfv(GL_LIGHT0, GL_DIFFUSE, diff);
	glLightfv(GL_LIGHT0, GL_SPECULAR, spec);
	glLightfv(GL_LIGHT0, GL_POSITION, posl);

//	glEnable(GL_COLOR_MATERIAL);
//	glColorMaterial(GL_FRONT, GL_AMBIENT);

	// Light 1
//	glLightfv(GL_LIGHT1, GL_AMBIENT, amb2);
//	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spotDir);
//	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 15.f);
//	glLightfv(GL_LIGHT1, GL_SPECULAR, spec);
//	glLightfv(GL_LIGHT1, GL_POSITION, posL1);
//	glEnable(GL_LIGHT1);

}

void Rendar::tabulateFrameRate()	{
	static int frameCount = 0;
	static long lastFrameTime = Sys_Milliseconds();
	static long frameStamp = lastFrameTime;
	long curFrameTime = Sys_Milliseconds();

	frameCount++;
	lastFrameTime = curFrameTime;

	if( frameStamp <= curFrameTime )	{
		this->frameRate = frameCount;
		frameCount = 0;
		frameStamp = curFrameTime+1000;
	}
}

void Rendar::drawFPS()	{
	screenPrint(0, 0, "FPS: %d", frameRate);
}


void Rendar::draw(void)	{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(	cam->origin[0], cam->origin[1], cam->origin[2],		// camera origin
				cam->dir[0], 	cam->dir[1], 	cam->dir[2],		// eye looking @ this vertex
				cam->up[0], 	cam->up[1], 	cam->up[2]);		// up direction

#ifdef MOUSELOOK
	glutWarpPointer(400, 300);	// center mouse
#endif

	lighting();

	// draw sky
	GLfloat emis[] = {0.8, 0.8, 0.8};
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emis);
	glCallList(skyCacheID);

	renderBSPTree(bspRoot);

	renderDynamicModels(*r_modelAdvRate);


	if( *r_showFPS == 1 )	{
		tabulateFrameRate();
		drawFPS();
	}

	glDisable(GL_LIGHTING);
	drawConsole();		// render the console

}




void Rendar::changeSize(int width, int height)	{
	*r_width = width;
	*r_height = height;

	double asratio;

    if(height == 0)
    	height = 1; //to avoid divide-by-zero

    asratio = width / (double) height;

    glViewport(0, 0, width, height); //adjust GL viewport

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(*r_fov, asratio, *zNear, *zFar); //adjust perspective
    glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(	cam->origin[0], cam->origin[1], cam->origin[2],		// camera origin
				cam->dir[0], 	cam->dir[1], 	cam->dir[2],		// eye looking @ this vertex
				cam->up[0], 	cam->up[1], 	cam->up[2]);		// up direction


}




void Rendar::drawPolygon(polygon_t* poly)	{
	if( poly->glCached && *r_wireFramePolys != 1 )	{
		glCallList(poly->glCacheID);
	}
	else	{
		glPushMatrix();
		if( *r_wireFramePolys == 1 )	{	// draw wire frame for polygons
			glBegin(GL_LINE_STRIP);
			for(int x=0; x < poly->numPoints; x++)	{
				glVertex3f(poly->points[x][0], poly->points[x][1], poly->points[x][2]);
			}
			glEnd();
		}
		else	{
			if( poly->hasMaterial )
				matsMgr->enableMaterial(poly->materialName);

			glBegin(GL_POLYGON);
			for(int x=0; x < poly->numPoints; x++)	{
				if( poly->hasNormals )
					glNormal3f(poly->normpts[x][0], poly->normpts[x][1], poly->normpts[x][2] );

				if( poly->isTextured )
					glTexCoord2f(poly->texpts[x][0], poly->texpts[x][1]);

				glVertex3f(poly->points[x][0], poly->points[x][1], poly->points[x][2]);
			}
			glEnd();

			if( poly->hasMaterial )
				matsMgr->disableMaterial(poly->materialName);
		}

		glPopMatrix();
	}
}

void Rendar::renderPolygonList(polygon_t* polygonList)	{

	polygon_t* curPoly = polygonList;

	while( curPoly != NULL )	{
		drawPolygon(curPoly);
		curPoly = curPoly->next;
	}

}

void Rendar::renderBSPTree(bsp_node_t* tree)	{

	if( tree == NULL )	{
//		Con_print("NULL tree pointer in renderBSPTree()");
		return;
	}

	if( tree->isLeaf() )	{
		renderPolygonList(tree->getPolygonList());
	}
	else	{
		// perform render back to front
		renderBSPTree(tree->back);
		renderBSPTree(tree->front);
	}
}

void Rendar::renderDynamicModels(float dt)	{
	entity_t* e = dynamicModels;

	while( e != NULL )	{

		// copy position out of mass if we have one, not all objects will have a mass
		if( e->mass )	{
			VectorCopy(e->mass->pos, e->pos);
		}

		if( e->model )	{
//			e->model->md2->setAnimation(e->model->action.c_str());
			e->model->md2->advance(dt);

			glPushMatrix();
			glTranslatef(e->pos[0], e->pos[1], e->pos[2]);
			GLfloat emis[] = {0.4, 0.4, 0.4};
//			glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emis);

			e->model->md2->draw();
			glPopMatrix();

		}

		e = e->next;
	}
}



void Rendar::bspFromObjModel(string modelName)	{
	string* path = getCvarAddress_S("r_modelPath");
	string canonicalPath = *path + "obj/" + modelName;

	ObjModel* obj = new ObjModel();
	obj->loadObjFile(canonicalPath);

	vec3_t mapDimensions;
	obj->getDimensions(mapDimensions);

	float width = 0;

	if( mapDimensions[0] > mapDimensions[2] )	{
		width = mapDimensions[0];
	}
	else	{
		width = mapDimensions[2];
	}

	cout << " Widest dimension: " << width << endl;

	bspRoot = new bsp_node_t;
	bspRoot->root = true;

	/*	// debugging stuffs
	polygon_t* p = obj->polygonList;
	int myPolyCount = 0;
	while( p )	{
		myPolyCount++;
		p = p->next;
	}

	Con_print("myPolyCount: %d", myPolyCount);
	*/

	generateBSPTree(bspRoot, obj->polygonList, width);
	nameAndCachePolygons(bspRoot);

	delete obj;
}

void Rendar::glCachePolygon(polygon_t* polygon)	{

	polygon->glCacheID = glGenLists(1);
    glNewList(polygon->glCacheID, GL_COMPILE);
    drawPolygon(polygon);
    glEndList();
    polygon->glCached = true;

}

void Rendar::nameAndCachePolygons(bsp_node_t* bspNode)	{
	polygon_t* itr = bspNode->getPolygonList();

	if( bspNode->isLeaf() )	{
		while(itr != NULL)	{
			glCachePolygon(itr);
			itr->polyID = itr->glCacheID;
			cachedPolygonCount++;	// just for stats, may be removed later
#ifdef BSPDEBUG
			Con_print("Cached polygon #%d", cachedPolygonCount);	// TODO remove me or set to debug
#endif
			itr = itr->next;
		}
	}
	else	{
		nameAndCachePolygons(bspNode->front);
		nameAndCachePolygons(bspNode->back);
	}
}

void Rendar::unCachePolygons(bsp_node_t* bspNode)	{
	polygon_t* itr = bspNode->getPolygonList();

	if( !bspNode )	{
#ifdef BSPDEBUG
		cout << "ERROR: unCachePolygons(): BSP NODE IS NULL DAMN YOU" << endl;
#endif
		return;
	}

	if( bspNode->isLeaf() )	{
#ifdef BSPDEBUG
		cout << "unCachePolygons(): Found a leaf uncaching " << bspNode->getPolygonCount() << " polygons..." << endl;
#endif
		while( itr != NULL )	{
			int i = itr->glCacheID;
			glDeleteLists(i, 1);
			cachedPolygonCount--;
			itr = itr->next;
#ifdef BSPDEBUG
			cout << "unCachePolygons(): glCacheID: " << itr->glCacheID << " cleared." << endl;
#endif
		}
	}
	else	{
#ifdef BSPDEBUG
		cout << "unCachePolygons(): Found node going front & back... following both" << endl;
#endif
		nameAndCachePolygons(bspNode->front);
		nameAndCachePolygons(bspNode->back);
	}
}

int Rendar::getCachedPolygonCount()	{
	return cachedPolygonCount;
}

void Rendar::LoadModel(string name)	{
	if( modelMgr->addModel(name) )	{
		model_t* model = modelMgr->getModel(name);

	}
	else	{
		Con_print("Model Manager: Couldn't load model: %s", name.c_str());
	}


}

void Rendar::addEntityToScene(string modelName, entity_t* ent)	{
//	entity_t* ent = new entity_t;

//	ent->gameID = id;
//	VectorCopy(pos, ent->pos);
//	VectorCopy(facing, ent->facing);
	ent->model = modelMgr->cloneModel(modelName);

	// add to data structures
	gameModels[ent->gameID] = ent;

	dynamicModels = doublyLinkEntities(dynamicModels, ent);
}


void Rendar::setAnimation(entity_t* e, string animName)	{
	e->model->md2->setAnimation(animName.c_str());
}

Camera* Rendar::getCamera()	{
	return cam;
}

void Rendar::getCameraPos(vec3_t v)	{
	VectorCopy(cam->origin, v);
}

void Rendar::getCameraFacing(vec3_t v)	{
	VectorCopy(cam->normDir, v);
}

void Rendar::screenPrint(int x, int y, const char* fmt, ...)	{
	va_list args;
	va_start(args,fmt);
	char str[1024];
	vsprintf(str, fmt, args);
	va_end(args);

	screenPrinter->glPrint(x, y, str, FONT_STYLE_NORMAL);
}
