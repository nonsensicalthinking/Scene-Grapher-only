/*
 * ModelManager.cpp
 *
 *  Created on: Feb 19, 2012
 *      Author: forb
 */


#include "ModelManager.h"
#include <string>
#include <map>

#include <unistd.h>

using namespace std;

extern void Con_print(const char* fmt, ...);
extern string getProgramPath();

ModelManager::ModelManager()	{

}

ModelManager::~ModelManager()	{

	cout << "Model Manager: Unloading models..." << endl;

	map<string, model_t*>::iterator itr;

	for(itr=modelMap.begin(); itr!=modelMap.end();itr++)	{
		model_t* temp = (*itr).second;
		delete temp;
	}

	cout << "Model Manager: All models have been unloaded." << endl;

}


/*
 * Returns false if model was unable to be found/added
 */
bool ModelManager::addModel(string name)	{

	int dotIndex = name.find_last_of('.', name.length());
	string extension = name.substr(dotIndex+1);

	string canonicalPath = getProgramPath();

	if( extension == "md2" )	{
		canonicalPath += "/models/md2/" + name;
		Con_print("Model Manager: Loading model - %s", name.c_str());
		Con_print("File: %s", canonicalPath.c_str());

		model_t* mod = new model_t;
		mod->isTemplate = true;	// this should only be set to true for original files
		mod->name = name;
		mod->md2 = MD2Model::load(canonicalPath.c_str());

		modelMap.insert(pair<string,model_t*>(name,mod));
		return true;
	}
	else	{
		Con_print("Model Manager: '%s' is an invalid model extension.", extension.c_str());
		Con_print("Model Manager: The currently supported model formats are: md2.");
	}

	return false;
}

bool ModelManager::removeModel(string name)	{

	return false;
}

model_t* ModelManager::cloneModel(string name)	{
	model_t* m = modelMap[name];

	if( m != NULL )	{
		model_t* clone = new model_t;
		clone->isTemplate = false;
		clone->cacheID = m->cacheID;
		clone->name = m->name;
		VectorCopy(m->dimensions, clone->dimensions);

		// must be written like this for implicit object copy
//		MD2Model* md2 = m->md2;
		clone->md2 = m->md2->clone();

		Con_print("MD2 Model Cloned Successfully!");
		return clone;
	}

	Con_print("Couldn't clone model: %s not found", name.c_str());

	return NULL;
}

/*
 * Returns positive value if model is cached and found
 */
int ModelManager::getModelCacheID(string name)	{

	return -1;
}

model_t* ModelManager::getModel(string name)	{
	model_t* m = modelMap[name];

	return m;
}























