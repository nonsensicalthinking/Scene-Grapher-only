/*
 * rendar.cpp
 *
 *  Created on: Jan 30, 2012
 *      Author: brof
 */

#include <iostream>
#include <string>
#include "Rendar.h"
#include "shared.h"
#include "WavefrontObj.h"

using namespace std;

extern void drawConsole();
extern void Con_print(const char* fmt, ...);
extern int* getCvarAddress_I(string s);
extern double* getCvarAddress_D(string s);
extern string* getCvarAddress_S(string s);
extern void registerCvar(string name, string value, int typeFlag);
extern string getCWD();
extern void registerCommand(string name, void (*func)(string), bool hasArgs);
extern string getProgramPath();

Rendar::Rendar(string windowTitle) {
	// register cvars
	registerCvar("r_wireFramePolys", "0", INT_CVAR);

	// get addresses for newly registered cvars
	r_wireFramePolys = getCvarAddress_I("r_wireFramePolys");
	scr_width = getCvarAddress_I("scr_width");
	scr_height = getCvarAddress_I("scr_height");
	scr_full = getCvarAddress_I("scr_full");
	scr_fov = getCvarAddress_I("src_fov");
	modelPath = getCvarAddress_S("r_modelPath");
	imagePath = getCvarAddress_S("r_imagePath");


	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(0,0);
	glutInitWindowSize(*scr_width, *scr_height);

	matsMgr = new MaterialManager();
	modelMgr = new ModelManager();
	winPtr = glutCreateWindow(windowTitle.c_str());

	gl_Init();

	cam = new Camera();
	bspRoot = NULL;
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

	if( modelMgr )	{
		delete modelMgr;
		cout << "Shutdown: Model Manager Unloaded." << endl;
	}

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
	glutSetCursor(GLUT_CURSOR_NONE);	// hide the mouse cursor

	//load basic textures for fonts and console

	// Used to call user code to load game here.
}


void Rendar::run(void)	{
	glutMainLoop();
}

void Rendar::lighting()	{

}

void Rendar::draw(void)	{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(	cam->origin[0], cam->origin[1], cam->origin[2],		// camera origin
				cam->dir[0], 	cam->dir[1], 	cam->dir[2],		// eye looking @ this vertex
				cam->up[0], 	cam->up[1], 	cam->up[2]);		// up direction

//	glutWarpPointer(400, 300);	// center mouse

	glTranslated(0, 0, 10);
	glColor3f(0.0, 0.0, 0.0);

	renderBSPTree(bspRoot);

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
	glLoadIdentity();

	gluLookAt(	cam->origin[0], cam->origin[1], cam->origin[2],		// camera origin
				cam->dir[0], 	cam->dir[1], 	cam->dir[2],		// eye looking @ this vertex
				cam->up[0], 	cam->up[1], 	cam->up[2]);		// up direction


}




void Rendar::drawPolygon(polygon_t* poly)	{
	if( poly->glCached )	{
		glCallList(poly->glCacheID);
	}
	else	{
		glPushMatrix();
		if( r_wireFramePolys )	{	// draw wire frame for polygons
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

void Rendar::renderPolygonList(list<polygon_t*> polygons)	{

	for(list<polygon_t*>::iterator itr = polygons.begin(); itr != polygons.end(); itr++)
		drawPolygon((*itr));

}

void Rendar::renderBSPTree(bsp_node_t* tree)	{

	if( !tree )
		return;

	if( tree->isLeaf() )	{
		renderPolygonList(tree->getPolygonList());
	}
	else	{
		// perform render back to front
		renderBSPTree(tree->back);
		renderBSPTree(tree->front);
	}
}


void Rendar::bspFromObjModel(string modelName)	{
	string path = getProgramPath();
	path += "/models/obj/" + modelName;

	ObjModel* obj = new ObjModel();
	obj->loadObjFile(path);

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

	Con_print("obj->polygonList->count(): %d", obj->polygonList.size() );

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
	list<polygon_t*>::iterator itr;

	if( bspNode->isLeaf() )	{
		for(itr = bspNode->beginPolyListItr(); itr != bspNode->endPolyListItr(); itr++)	{
			glCachePolygon(*itr);
			(*itr)->polyID = (*itr)->glCacheID;
			cachedPolygonCount++;	// just for stats, may be removed later
		}
	}
	else	{
		nameAndCachePolygons(bspNode->front);
		nameAndCachePolygons(bspNode->back);
	}
}

void Rendar::unCachePolygons(bsp_node_t* bspNode)	{
	list<polygon_t*>::iterator itr;

	if( !bspNode )	{
#ifdef BSPDEBUG
		cout << "ERROR: unCachePolygons(): BSP NODE IS NULL DAMN YOU" << endl;
#endif
		return;
	}

	if( bspNode->isLeaf() )	{
#ifdef BSPDEBUG
		cout << "unCachePolygons(): Found a leaf uncaching " << bspNode->getPolygonList().size() << " polygons..." << endl;
#endif
		for(itr = bspNode->beginPolyListItr(); itr != bspNode->endPolyListItr(); itr++)	{
			int i = (*itr)->glCacheID;
			glDeleteLists(i, 1);
			cachedPolygonCount--;
#ifdef BSPDEBUG
			cout << "unCachePolygons(): glCacheID: " << (*itr)->glCacheID << " cleared." << endl;
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

void Rendar::LoadModel(string name)	{
	if( modelMgr->addModel(name) )	{
		model_t* model = modelMgr->getModel(name);

	}
	else	{
		Con_print("Model Manager: Couldn't load model: %s", name.c_str());
	}


}

int Rendar::getCachedPolygonCount()	{
	return cachedPolygonCount;
}



