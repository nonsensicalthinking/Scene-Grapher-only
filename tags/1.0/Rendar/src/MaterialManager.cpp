/*
 * MaterialManager.cpp
 *
 *  Created on: Feb 1, 2012
 *      Author: brof
 */

#include "MaterialManager.h"
#include <string.h>

extern void Con_print(const char* fmt, ...);

extern string* getCvarAddress_S(string s);


MaterialManager::MaterialManager()	{
	loadBitmap("not-found.bmp");
}

MaterialManager::~MaterialManager()	{
	unloadAllTextures();
}


bool MaterialManager::bindTexture(string texturePath)	{
	map<string,GLuint>::iterator tex = textures.find(texturePath);

	if( tex != textures.end() )	{
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, tex->second);
		return true;
	}

	return false;
}

void MaterialManager::enableSphereMapping()	{
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
}

void MaterialManager::disableSphereMapping()	{
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
}

bool MaterialManager::enableMaterial(string matName)	{
	material_t* mat = materials[matName];

	if( !mat )	{
		Con_print("Material not found: %s", matName.c_str());
		return false;
	}


	bindTexture(mat->map_Kd);

	// TODO GET THE MATERIAL PROPERTIES WORKING!!
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat->Ka);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat->Kd);
	// TODO use mat->illum to define how specular is used
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat->Ks);
	// TODO implement light emission materials
//		glMaterialfv(GL_FRONT, GL_EMISSION, mat-> );
	glMaterialf(GL_FRONT, GL_SHININESS, mat->Ns);

	return true;
}

void MaterialManager::disableMaterial(string matName)	{
	material_t* mat = materials[matName];

	if( !mat )
		return;

	if( strcmp(mat->map_Kd, "(null)") )	{	// If "(null)" isn't the string
		glDisable(GL_TEXTURE_2D);
	}

}


bool MaterialManager::loadBitmap(string str)	{
	Bitmap* bmp = new Bitmap();

	string* imagePath = getCvarAddress_S("r_imagePath");

	string canonicalPath = *imagePath + str;

	if( !bmp->loadBMP(canonicalPath) )	{
		Con_print("Couldn't load bitmap (%s): %s", canonicalPath.c_str(), bmp->error.c_str());
		return false;
	}
	else	{
		Con_print("BITMAP LOADED: %s", canonicalPath.c_str());
	}

	GLuint tid;
	glGenTextures(1, &tid);
	glBindTexture(GL_TEXTURE_2D, tid);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // GL_LINEAR_MIPMAP_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	gluBuild2DMipmaps( GL_TEXTURE_2D, 3, bmp->width, bmp->height, GL_RGB, GL_UNSIGNED_BYTE, bmp->data );

	textures[str] = tid;

	delete bmp;
	return true;
}


GLuint MaterialManager::getGLTexID(string texName)	{
	return textures[texName];
}

void MaterialManager::unloadTexture(string path)	{
	textures.erase(path);
}

void MaterialManager::unloadAllTextures()	{

	map<string,material_t*>::iterator itr;

	for(itr=materials.begin(); itr!=materials.end(); itr++)	{
		cout << "Deleting material: " << (*itr).first << endl;
		material_t* mat = (*itr).second;
		delete mat;
	}

	textures.clear();
}

void MaterialManager::addMaterial(string matName, material_t* mat)	{
	materials[matName] = mat;
}

void MaterialManager::removeMaterial(string matName)	{
	material_t* mat;
	mat = materials[matName];
	materials.erase(matName);
	delete mat;
}

void MaterialManager::purgeMaterials()	{
	map<string,material_t*>::iterator itr;
	for(itr=materials.begin(); itr != materials.end(); itr++)	{
		material_t* mat = (*itr).second;	// itr is key,val pair
		delete mat;
	}
	materials.clear();
}

bool MaterialManager::hasMaterial(string matName)	{
	if( !materials[matName] )
		return false;

	return true;
}

void MaterialManager::printTextureList()	{
	map<string,GLuint>::iterator itr;
	for(itr=textures.begin(); itr != textures.end(); itr++)	{
		Con_print("Tex: %s", (*itr).first.c_str());
	}
}

