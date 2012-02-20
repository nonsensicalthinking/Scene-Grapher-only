/*
 * ModelManager.h
 *
 *  Created on: Feb 19, 2012
 *      Author: forb
 */

#include "shared.h"
#include <GL/gl.h>
#include <map>

#ifndef MODELMANAGER_H_
#define MODELMANAGER_H_

using namespace std;

typedef struct model_s	{
	GLint cacheID;
	string name;		// path?
	vec3_t dimensions;	// bounding box dimensions
	//MD2Model* md2;
}model_t;



class ModelManager	{

	map<string,model_t*> modelMap;

public:
	ModelManager();
	~ModelManager();

	bool addModel(string name);
	bool removeModel(string name);
	int getModelCacheID(string name);
	model_t* getModel(string name);
};


#endif /* MODELMANAGER_H_ */
