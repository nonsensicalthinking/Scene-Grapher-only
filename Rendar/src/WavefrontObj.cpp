/*
 * WavefrontObj.cpp
 *
 *  Created on: Feb 7, 2012
 *      Author: forb
 */
#include "WavefrontObj.h"
#include <cmath>

//#define OBJDEBUG
//#define OBJMATDEBUG

extern void Con_print(const char* fmt, ...);
extern string* getCvarAddress_S(string s);


ObjModel::ObjModel()	{
	x_max = x_min = y_max = y_min = z_max = z_min = 0;
	polygonList = NULL;
	polygonListCount = 0;
}

ObjModel::~ObjModel()	{

}

bool ObjModel::loadObjFile(string s)	{
	FILE* fp;
	if( !(fp=fopen(s.c_str(), "r")) )	{
		error = "OBJ file not found";
		Con_print("Obj: File not found - %s", s.c_str());
		return false;
	}

	while( !feof(fp) )	{
		char in[MAX_OBJ_LINE_LEN];
		fgets(in, MAX_OBJ_LINE_LEN, fp);
#ifdef OBJDEBUG
		cout << "OBJ Line: " << in << endl;
#endif
		if( in[0] == '#' )
			continue;

		processOBJLine(in);
	}

	fclose(fp);

	return true;
}

// perform these checks so we can create a bounding box elsewhere.
void ObjModel::doMaxCheck(float val, char axis)	{
	switch(axis)	{
	case 'x':
		if( val > x_max )
			x_max = val;

		if( val < x_min )
			x_min = val;

		break;
	case 'y':
		if( val > y_max )
			y_max = val;

		if( val < y_min )
			y_min = val;

		break;
	case 'z':
		if( val > z_max )
			z_max = val;

		if( val < z_min )
			z_min = val;

		break;
	}
}

void ObjModel::processOBJLine(char line[MAX_OBJ_LINE_LEN])	{
	char *token;
	char linecpy[256];

	vector<float> tmpV;
	vector<float> tmpVT;
	vector<float> tmpVN;

	strcpy(linecpy, line);

	token = strtok(line, WHITESPACE);

	if( token == NULL )
		return;

	if( !strcmp(token, "v") )	{
		vec3_t input;
		sscanf(linecpy, "v %f %f %f", &input[0], &input[1], &input[2]);
		tmpV.push_back(input[0]);
		tmpV.push_back(input[1]);
		tmpV.push_back(input[2]);
		v.push_back(tmpV);
		// do bounding box check
		doMaxCheck(input[0], 'x');
		doMaxCheck(input[1], 'y');
		doMaxCheck(input[2], 'z');
	}
	else if( !strcmp(token, "vt") )	{
		vec2_t input;
		sscanf(linecpy, "vt %f %f", &input[0], &input[1]);
		tmpVT.push_back(input[0]);
		tmpVT.push_back(input[1]);
		vt.push_back(tmpVT);
	}
	else if( !strcmp(token, "vn") )	{
		vec3_t input;
		sscanf(linecpy, "vn %f %f %f", &input[0],& input[1], &input[2]);
		tmpVN.push_back(input[0]);
		tmpVN.push_back(input[1]);
		tmpVN.push_back(input[2]);
		vn.push_back(tmpVN);
	}
	else if( !strcmp(token, "f") )	{
		bool hasTex = false;

		// check face for textures
		if( !strstr(linecpy, "//") )	// if we don't find "//", then we have textures
			hasTex = true;

		switch(countCharOccur(linecpy, '/'))	{
			case 6:	// Polygons with 3 points
				vec3_t face[3];

				if( hasTex )	{
					sscanf(linecpy, "f %f/%f/%f %f/%f/%f %f/%f/%f",
							&face[0][0], &face[0][1], &face[0][2],
							&face[1][0], &face[1][1], &face[1][2],
							&face[2][0], &face[2][1], &face[2][2]);
				}
				else	{
					sscanf(linecpy, "f %f//%f %f//%f %f//%f",
							&face[0][0], &face[0][2],
							&face[1][0], &face[1][2],
							&face[2][0], &face[2][2]);

					// set to -1 so if we even try to index
					// we'll get an out of bounds here instead of
					// indexing the 0th place
					face[0][1] = -1;
					face[1][1] = -1;
					face[2][1] = -1;
				}

#ifdef OBJDEBUG
				cout << "3 pointed polygon (makeFace())" << endl;
				VectorPrint(face[0]);
				VectorPrint(face[1]);
				VectorPrint(face[2]);
				cout << "\n------------------------------" << endl;
#endif

				makeFace(face, 3, hasTex);
				break;

			case 8:	// Polygons with 4 points
				vec3_t sface[4];

				if( hasTex )	{
					sscanf(linecpy, "f %f/%f/%f %f/%f/%f %f/%f/%f %f/%f/%f",
						&sface[0][0], &sface[0][1], &sface[0][2],
						&sface[1][0], &sface[1][1], &sface[1][2],
						&sface[2][0], &sface[2][1], &sface[2][2],
						&sface[3][0], &sface[3][1], &sface[3][2]);

				}
				else	{
					sscanf(linecpy, "f %f//%f %f//%f %f//%f %f//%f",
							&sface[0][0], &sface[0][2],
							&sface[1][0], &sface[1][2],
							&sface[2][0], &sface[2][2],
							&sface[3][0], &sface[3][2]);


					// set to -1 so if we even try to index
					// we'll get an out of bounds here instead of
					// indexing the 0th place
					sface[0][1] = -1;
					sface[1][1] = -1;
					sface[2][1] = -1;
					sface[3][1] = -1;
				}

#ifdef OBJDEBUG
				cout << "4 pointed polygon (makeFace())" << endl;
				VectorPrint(sface[0]);
				VectorPrint(sface[1]);
				VectorPrint(sface[2]);
				VectorPrint(sface[3]);
				cout << "\n------------------------------" << endl;
#endif

				makeFace(sface, 4, hasTex);
				break;
		}
	}
	else if( !strcmp(token, "s") )	{
		// TODO Find out what this does
		// argument to this token is a string
		// "on" or "off" it appears.
	}
	else if( !strcmp(token, "usemtl") )	{
		token = strtok(NULL, WHITESPACE);
		strcpy(curMat, token);
	}
	else if( !strcmp( token, "mtllib") )	{
		token = strtok(NULL, WHITESPACE);
		loadMTLFile(token);
	}
	else if( !strcmp( token, "o") )	{
		// TODO implement face associate with an "object"
	}
	else	{
		cout << "Unrecognized OBJ Token: " << token << endl;
	}
}

void ObjModel::makeFace(vec3_t face[], int numPts, bool isTextured)	{
	polygon_t* poly = createPolygon();

	// set the material name
	strcpy(poly->materialName, curMat);

	if( strcmp(curMat, "(null)") )
		poly->hasMaterial = true;
	else
		poly->hasMaterial = false;

	poly->isTextured = isTextured;

	// FIXME Account for models without normals
	poly->hasNormals = true;


	// IMPORTANT: Face values aren't zero based, they begin at 1
	// this poses the -1 onto the face index, we store in zero based vector
	poly->numPoints = numPts;
	for(int x=0; x < numPts; x++)	{
		poly->points[x][0] = v[(face[x][0]-1)][0];
		poly->points[x][1] = v[(face[x][0]-1)][1];
		poly->points[x][2] = v[(face[x][0]-1)][2];

		if( isTextured )	{
			poly->texpts[x][0] = vt[(face[x][1]-1)][0];
			poly->texpts[x][1] = vt[(face[x][1]-1)][1];
			poly->texpts[x][2] = vt[(face[x][1]-1)][2];
		}

		poly->normpts[x][0] = vn[(face[x][2]-1)][0];
		poly->normpts[x][1] = vn[(face[x][2]-1)][1];
		poly->normpts[x][2] = vn[(face[x][2]-1)][2];
	}

	polygonListCount++;
	polygonList = doublyLinkPolygons(polygonList, poly);

#ifdef OBJDEBUG
	int tempCount = 0;

	polygon_t* t = polygonList;
	while( t )	{
		tempCount++;
		t = t->next;
	}
	Con_print("Running polygon count: %d", polygonListCount);
	Con_print("Confirmed polygon count: %d", tempCount);
#endif
}

void ObjModel::initializeMaterial(material_t* mat)	{
	strcpy(mat->materialName, "");

	// These values are apparently opengl defualts
	mat->Ns = 0;

	mat->Ka[0] = 0.2;
	mat->Ka[1] = 0.2;
	mat->Ka[2] = 0.2;

	mat->Kd[0] = 0.8;
	mat->Kd[1] = 0.8;
	mat->Kd[2] = 0.8;

	mat->Ks[0] = 1.0;
	mat->Ks[1] = 1.0;
	mat->Ks[2] = 1.0;

	mat->d = 1.0;
	mat->illum = 1;
}

bool ObjModel::loadMTLFile(const char* fileName)	{
	cout << "Loading material file: " << fileName << endl;

	FILE* fp;
	string* modelPath = getCvarAddress_S("r_modelPath");

	string canonicalPath = *modelPath + "obj/" + fileName;

	cout << "From file path: " << canonicalPath << endl;

	if( !(fp=fopen(canonicalPath.c_str(), "r")) )	{
		error = "OBJ Materials file not found";
		Con_print("Obj: File not found - %s", canonicalPath.c_str());
		return false;
	}

	MaterialManager* tm = getMaterialManager();

	material_t* mat;

	while( !feof(fp) )	{
		char in[MAX_OBJ_LINE_LEN];
		fgets(in, MAX_OBJ_LINE_LEN, fp);
		char incpy[MAX_OBJ_LINE_LEN];
#ifdef OBJMATDEBUG
		cout << "MAT Line: " << in << endl;
#endif
		strcpy(incpy, in);

		// if its a comment or whitespace skip it
		if( in[0] == '#' || in[0] == ' ' || in[0] == '\n' ||
				in[0] == '\r'  || in[0] == '\t')
			continue;
		else	{	// otherwise we need to process some data
			char* token = strtok(in, WHITESPACE);

			if( token == NULL )
				break;

			if( !strcmp(token, "newmtl") )	{
				material_t* newmat = new material_t;
				initializeMaterial(newmat);

				token = strtok(NULL, WHITESPACE);
				if( !tm->hasMaterial(token) )	{
					tm->addMaterial(token, newmat);
					mat = newmat;
#ifdef OBJMATDEBUG
					cout << "New material created: " << token << endl;
#endif
				}
				else	{
#ifdef OBJMATDEBUG
					cout << "MTL Error: Material redefinition: " << token << endl;
#endif
				}
			}
			else if( !strcmp(token, "Ns") )	{
				sscanf(incpy, "Ns %f", &mat->Ns);
			}
			else if( !strcmp(token, "Ka") )	{
				sscanf(incpy, "Ka %f %f %f", &mat->Ka[0], &mat->Ka[1], &mat->Ka[2]);
			}
			else if( !strcmp(token, "Kd") )	{
				sscanf(incpy, "Kd %f %f %f", &mat->Kd[0], &mat->Kd[1], &mat->Kd[2]);
			}
			else if( !strcmp(token, "Ks") )	{
				sscanf(incpy, "Ks %f %f %f", &mat->Ks[0], &mat->Ks[1],& mat->Ks[2]);
			}
			else if( !strcmp(token, "Ni") )	{
				sscanf(incpy, "Ni %f", &mat->Ni);
			}
			else if( !strcmp(token, "d") )	{
				sscanf(incpy, "d %f", &mat->d);
			}
			else if( !strcmp(token, "illum") )	{
				sscanf(incpy, "illum %d", &mat->illum);
			}
			else if( !strcmp(token, "map_Kd") )	{
				token = strtok(NULL, WHITESPACE);
				strcpy(mat->map_Kd, token);
#ifdef OBJMATDEBUG
				cout << "Loading texture: " << mat->map_Kd << endl;
#endif
				getMaterialManager()->loadBitmap(mat->map_Kd);
			}
		}
	}

	fclose(fp);

	return true;
}

void ObjModel::getDimensions(vec3_t dim)	{
	dim[0] = abs(x_max) + abs(x_min);
	dim[1] = abs(y_max) + abs(y_min);
	dim[2] = abs(z_max) + abs(z_min);

	VectorPrint(dim);
}
