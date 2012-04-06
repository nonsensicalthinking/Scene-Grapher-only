/*
 * MaterialManager.h
 *
 *  Created on: Jun 22, 2010
 *      Author: Derek Brooks
 *
 *
 *
 *  TODO For this file
 *	TODO Make loadJPEG function so we can use alpha layers
 *	TODO Make loadPNG function so we can use alpha layers
 *
 *	TODO Get Material Properties working, check the enableMaterial()
 *
 *
 *
 */
#include <iostream>
#include <map>
#include <utility>
#include <string>
#include <GL/gl.h>
#include <GL/glu.h>

#include "shared.h"
#include "Bitmap.h"

#ifndef MATERIAL_MANAGER_H_
#define MATERIAL_MANAGER_H_

#define MAX_MAT_NAME_LEN	128

using namespace std;

typedef struct material_s	{
	char materialName[MAX_MAT_NAME_LEN];

	vec3_t Ka;
	vec3_t Kd;
	vec3_t Ks;

	float d;	// Alpha
	float Ns;
	float Ni;
	int illum;

	char map_Kd[MAX_FILE_LENGTH];
}material_t;



class MaterialManager	{

	map<string,GLuint> textures;
	map<string,material_t*> materials;


public:
	MaterialManager();
	~MaterialManager();
	bool bindTexture(string texturePath);
	void enableSphereMapping();
	void disableSphereMapping();
	bool enableMaterial(string matName);
	void disableMaterial(string matName);
	bool loadBitmap(string str);
	GLuint getGLTexID(string texName);
	void unloadTexture(string path);
	void unloadAllTextures();
	void addMaterial(string matName, material_t* mat);
	void removeMaterial(string matName);
	void purgeMaterials();
	bool hasMaterial(string matName);

	void printTextureList();
};


#endif /* TEXTURE_H_ */
