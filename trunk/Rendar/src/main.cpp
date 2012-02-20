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
#include "Rendar.h"
#include "Console.h"
#include "keys.h"
#include "shared.h"
#include "strtools.h"
#include <stdarg.h>
#include <GL/glext.h>

#ifdef _WIN32
#include <direct.h>
#endif
#ifdef linux
#include <unistd.h>
#endif

#define MAX_CVARS 1024

using namespace std;



Rendar* rendarar;
Console* con;

cvar_t configVars[MAX_CVARS];

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
}

void shutdown()	{
	delete rendarar;
	delete con;
}

MaterialManager* getMaterialManager()	{
	return rendarar->matsMgr;
}

int* getCvarAddress_I(string name)	{
	int hash = generateHash(name);
	return &(configVars[hash].i);
}

double* getCvarAddress_D(string name)	{
	int hash = generateHash(name);
	return &(configVars[hash].d);
}

string* getCvarAddress_S(string name)	{
	int hash = generateHash(name);
	return &(configVars[hash].s);
}


void printCvar(string args)	{
	char parse[512];
	strncpy(parse, args.c_str(), 512);
	char* cvar = strtok(parse," \t\n\r");
	string s = cvar;
	string_tolower(s);


	int hash = generateHash(s);

	if( configVars[hash].typeFlag != 0 )	{
		switch(configVars[hash].typeFlag)	{
		case INT_CVAR:
			Con_print("%s = %d", configVars[hash].name.c_str(), configVars[hash].i);
			break;
		case DOUBLE_CVAR:
			Con_print("%s = %f", configVars[hash].name.c_str(), configVars[hash].d);
			break;
		case STRING_CVAR:
			Con_print("%s = %s", configVars[hash].name.c_str(), configVars[hash].s.c_str());
			break;
		}

	}
	else	{
		Con_print("%s, No such cvar registered.", s.c_str());
	}


}

void setCvar(string args)	{
	char parse[512];
	strncpy(parse, args.c_str(), 512);
	char* cvar = strtok(parse, " \t\n\r");
	char* value = strtok(NULL, " \t\n\r");
	string s = cvar;
	string_tolower(s);


	int hash = generateHash(s);

	if( configVars[hash].typeFlag != 0 )	{
		switch(configVars[hash].typeFlag)	{
		case INT_CVAR:
			configVars[hash].i = atoi(value);
			Con_print("%s = %d", configVars[hash].name.c_str(), configVars[hash].i);
			break;
		case DOUBLE_CVAR:
			configVars[hash].d = atof(value);
			Con_print("%s = %f", configVars[hash].name.c_str(), configVars[hash].d);
			break;
		case STRING_CVAR:
			configVars[hash].s = value;
			Con_print("%s = %s", configVars[hash].name.c_str(), configVars[hash].s.c_str());
			break;
		}
	}
	else	{
		Con_print("%s, No such cvar registered.", s.c_str());
	}
}

void initCvars()	{
	for(int x=0; x < MAX_CVARS; x++)
		configVars[x].typeFlag = UNUSED_CVAR;
}

void registerCvar(string name, string value, int typeFlag)	{
	int hash = generateHash(name);

	Con_print("CVAR REGISTERED: [%s] Len: [%d] Hash: [%d]", name.c_str(), name.length(), hash);

	configVars[hash].name = name;
	configVars[hash].typeFlag = typeFlag;

	switch(typeFlag)	{
	case INT_CVAR:
		configVars[hash].i = atoi(value.c_str());
		break;
	case DOUBLE_CVAR:
		configVars[hash].d = atof(value.c_str());
		break;
	case STRING_CVAR:
		configVars[hash].s = value;
		break;
	}


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

void processNormalKeys(unsigned char key, int x, int y)	{

	if( con->consoleActive )	{	// send key input to console
		switch(key)	{
			case ESC_KEY:
				con->clearInput();
				break;
			default:	// add to input line
				con->appendToInput(key);
				break;
		}
	}
	else	{	// Don't send key input to console
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

void passiveMouseMove(int x, int y)	{
	// TODO forward to game
}

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

	initCvars();
	registerCvar("name", 			"defaultPlayer", 	STRING_CVAR);
	registerCvar("scr_width", 		"800", 				INT_CVAR);
	registerCvar("src_height", 		"600", 				INT_CVAR);
	registerCvar("src_full", 		"0", 				INT_CVAR);
	registerCvar("scr_fov",			"45",				INT_CVAR);
	registerCvar("g_gravity", 		"9.8", 				DOUBLE_CVAR);
	registerCvar("cwd",				getCWD(),			STRING_CVAR);
	registerCvar("r_modelPath",		getCWD()+"/models/",STRING_CVAR);
	registerCvar("r_imagePath", 	getCWD()+"/images/",STRING_CVAR);

	glutInit(&argc, argv);

	rendarar = new Rendar("Rendaar 0.0.3a");

	glutDisplayFunc(draw);
	glutIdleFunc(draw);
	glutReshapeFunc(changeSize);
	glutMouseFunc(processMouse);
	glutKeyboardFunc(processNormalKeys);
	glutSpecialFunc(processSpecialKeys);
	glutPassiveMotionFunc(passiveMouseMove);

	con = new Console(800,600);
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
