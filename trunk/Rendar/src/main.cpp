//============================================================================
// Name        : rendar.cpp
// Author      : Derek Brooks
// Version     :
// Copyright   : 
// Description : Hello World in C, Ansi-style
//============================================================================

#include <iostream>
#include <stdlib.h>
#include <string>
#include <stdarg.h>
#include "Rendar.h"
#include "Console.h"
#include "keys.h"
#include "shared.h"
#include "strtools.h"
#include "CvarRegister.h"
#include <GL/glext.h>


#define MOUSELOOK


#ifdef _WIN32
#include <direct.h>
#endif
#ifdef linux
#include <unistd.h>
#endif

using namespace std;

Rendar* rendarar;
Console* con;
CvarRegister* cvarReg;

string programPath;

string getProgramPath()	{
	return programPath;
}

string getCWD()	{
	char path[1024];
	size_t size;
	getcwd(path,size);
	string s = string(path);
	return s;
}

void drawConsole()	{
	if( con->consoleActive )
		con->Draw();
}

void Con_print( const char* fmt, ... )
{
	va_list args;
	va_start(args,fmt);
	char str[1024];
	vsprintf(str, fmt, args);
	va_end(args);

	string line = str;
	if( con )
		con->con_print(line);
	else
		printf("%s", line.c_str());
}

void shutdown()	{
	delete rendarar;
	delete con;
	delete cvarReg;
}

MaterialManager* getMaterialManager()	{
	return rendarar->matsMgr;
}

int* getCvarAddress_I(string name)	{
	return cvarReg->getCvarAddress_I(name);
}

double* getCvarAddress_D(string name)	{
	return cvarReg->getCvarAddress_D(name);
}

string* getCvarAddress_S(string name)	{
	return cvarReg->getCvarAddress_S(name);
}

void printCvar(string args)	{
	cvarReg->setCvar(args);
}

void setCvar(string args)	{
	cvarReg->setCvar(args);
}

bool cvarRegistered(string name)	{
	return cvarReg->cvarRegistered(name);
}

map<string,cvar_t*>::iterator cvarLowerBound(string str)	{
	return cvarReg->lower_bound(str);
}

map<string,cvar_t*>::iterator cvarUpperBound(string str)	{
	return cvarReg->upper_bound(str);
}

void registerCvar(string name, string value, int typeFlag)	{
	cvarReg->registerCvar(name, value, typeFlag);
}

void registerCommand(string name, void (*func)())	{
	con->registerCommand(name, func);
}

void registerCommand(string name, void (*func)(string), bool hasArgs)	{
	con->registerCommand(name, func, hasArgs);
}

void processMouse(int button, int state, int x, int y)	{
	// TODO forward to game
}


void tempCamControl(unsigned char key, int x, int y)	{
	switch(key)	{
	case 'a':
		rendarar->getCamera()->moveCameraLeft(1);
		break;
	case 's':
		rendarar->getCamera()->moveCameraBack(1);
		break;
	case 'd':
		rendarar->getCamera()->moveCameraRight(1);
		break;
	case 'w':
		rendarar->getCamera()->moveCameraForward(1);
		break;
	case 'q':
		rendarar->getCamera()->moveCameraUp(1);
		break;
	case 'e':
		rendarar->getCamera()->moveCameraDown(1);
		break;
	}
}



void processNormalKeys(unsigned char key, int x, int y)	{

	if( con->consoleActive )	{	// send key input to console
		switch(key)	{
			case CONSOLE_KEY:
				con->consoleActive = !con->consoleActive;
				break;
			case ESC_KEY:
				con->clearInput();
				break;
			case 9:
				con->autoComplete();
				break;
			default:	// add to input line
				con->appendToInput(key);
				break;
		}
	}
	else	{	// Don't send key input to console

		if( key == CONSOLE_KEY )
			con->consoleActive = !con->consoleActive;

		tempCamControl(key, x, y);

		// TODO forward to game
//		keyPressed(key, x, y);
	}

}

void processSpecialKeys(int key, int x, int y) {
	if( con->consoleActive ) 	{
		switch(key)	{
			case GLUT_KEY_F1:
				con->consoleActive = !con->consoleActive;
				break;
			case GLUT_KEY_UP:
				con->previousCommand();
				break;
			case GLUT_KEY_DOWN:
				con->nextCommand();
				break;
			case GLUT_KEY_PAGE_UP:
				con->scrollUp();
				break;
			case GLUT_KEY_PAGE_DOWN:
				con->scrollDown();
				break;
		}
	}
	else	{
		switch(key)	{
			case GLUT_KEY_F1:
				con->consoleActive = !con->consoleActive;
				break;
			default:
				// send to game
				break;
		}
	}
}

#ifdef MOUSELOOK
void passiveMouseMove(int x, int y)	{
	float dx = 400-x;
	float dy = 300-y;

	if( dx == 0 && dy == 0 )
		return;

	dx *= 0.00025;
	dy *= 0.00025;

	rendarar->getCamera()->rotateAboutX(dy);	// Pitch
	rendarar->getCamera()->rotateAboutY(-dx);	// Yaw
}
#endif

void changeSize(int w, int h)	{
	if( rendarar )
		rendarar->changeSize(w,h);
}


void draw(void)	{
	if( rendarar )
		rendarar->draw();
}

void vid_restart()	{

	int* width = getCvarAddress_I("scr_width");
	int* height = getCvarAddress_I("scr_height");

	changeSize(*width, *height);

}

void echo(string args)	{
	char parse[128];
	strncpy(parse, args.c_str(), 128);
	char* cmd = strtok(parse, " \t\n\r");
	char* str = strtok(NULL, "\n");
	Con_print("%s\0", str);
}


void printGLInfo()	{

	cout << "Listing extensions..." << endl;
	const char* gl_vendor = (const char*)glGetString(GL_VENDOR);
	const char* gl_renderer = (const char*)glGetString(GL_RENDERER);
	const char* gl_version = (const char*)glGetString(GL_VERSION);
	const char* gl_shading = (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);

	Con_print("___OpenGL Version Information___");
	Con_print("Vendor: %s", gl_vendor);
	Con_print("Renderer: %s", gl_renderer);
	Con_print("OpenGL Version: %s", gl_version);
	Con_print("Shader Version: %s", gl_shading);

}

void LoadModel(string path)	{
	if( rendarar )
		rendarar->LoadModel(path);
}

void LoadMap(string name)	{
	if( rendarar )	{
		Con_print("Loading map: %s", name.c_str());
		rendarar->bspFromObjModel(name);
	}
}

void polygonCount()	{
	if( rendarar )
		Con_print("Cached polygons in scene: %d", rendarar->getCachedPolygonCount());
}

int main(int argc, char** argv) {

	programPath = getCWD();

	cvarReg = new CvarRegister();
	registerCvar("name", 			"defaultPlayer", 		STRING_CVAR);
	registerCvar("scr_width", 		"800", 					INT_CVAR);
	registerCvar("scr_height", 		"600", 					INT_CVAR);
	registerCvar("scr_full", 		"0", 					INT_CVAR);
	registerCvar("scr_fov",			"45",					INT_CVAR);
	registerCvar("g_gravity", 		"9.8", 					DOUBLE_CVAR);
	registerCvar("cwd",				getCWD(),				STRING_CVAR);
	registerCvar("r_modelPath",		getCWD()+"/models/",	STRING_CVAR);
	registerCvar("r_imagePath", 	getCWD()+"/images/",	STRING_CVAR);


	glutInit(&argc, argv);

	rendarar = new Rendar("Rendaar 0.0.3a");

	glutDisplayFunc(draw);
	glutIdleFunc(draw);
	glutReshapeFunc(changeSize);
	glutMouseFunc(processMouse);
	glutKeyboardFunc(processNormalKeys);
	glutSpecialFunc(processSpecialKeys);
	glutPassiveMotionFunc(passiveMouseMove);

	con = new Console(1024,768);

	registerCommand("quit", shutdown);
	registerCommand("echo", echo, true);
	registerCommand("s", setCvar, true);
	registerCommand("p", printCvar, true);
	registerCommand("vid_restart", vid_restart);
	registerCommand("gl_info", printGLInfo);
	registerCommand("gl_loadmodel", LoadModel, true);
	registerCommand("gl_loadmap", LoadMap, true);
	registerCommand("gl_polycount", polygonCount);

	rendarar->run();



	cout << "!@#$%^&*Unreachable code" << endl;

	return 0;
}
