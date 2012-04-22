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
#include "CvarRegister.h"
#include "Game.h"
#include "entity.h"
#include <GL/glext.h>
#include <sys/time.h>

#ifdef _WIN32
#include <direct.h>
#endif
#ifdef linux
#include <unistd.h>
#include <dlfcn.h>
#endif


#define MOUSELOOK



using namespace std;


/* From Quake 3 Arena
================
Sys_Milliseconds
================
*/
/* base time in seconds, that's our origin
   timeval:tv_sec is an int:
   assuming this wraps every 0x7fffffff - ~68 years since the Epoch (1970) - we're safe till 2038
   using unsigned long data type to work right with Sys_XTimeToSysTime */
unsigned long sys_timeBase = 0;
/* current time in ms, using sys_timeBase as origin
   NOTE: sys_timeBase*1000 + curtime -> ms since the Epoch
     0x7fffffff ms - ~24 days
   although timeval:tv_usec is an int, I'm not sure wether it is actually used as an unsigned int
     (which would affect the wrap period) */
long Sys_Milliseconds (void)
{
	struct timeval tp;
	long curtime;

	gettimeofday(&tp, NULL);

	if (!sys_timeBase)
	{
		sys_timeBase = tp.tv_sec;
		return tp.tv_usec*0.001f;
	}

	curtime = (tp.tv_sec - sys_timeBase)*1000 + (tp.tv_usec*0.001f);

	return curtime;
}






Rendar* rendarar;
Console* con;
CvarRegister* cvarReg;
Game* g;

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
		cout << line << endl;
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

float* getCvarAddress_F(string name)	{
	return cvarReg->getCvarAddress_F(name);
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

void registerCommandWithArgs(string name, void (*func)(string), bool hasArgs)	{
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

		if( g )
			g->processNormalKeys(key, x, y);
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
				if( g )
					g->processSpecialKeys(key, x, y);
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


float getTimeDelta()	{
	static long prevTime = Sys_Milliseconds();
	long curTime = Sys_Milliseconds();
	long timeDiff = curTime - prevTime;
	prevTime = curTime;
	return timeDiff * 0.001;
}

void draw(void)	{


	if( rendarar )
		rendarar->draw();

	if( g )
		g->advance(getTimeDelta());

	glutSwapBuffers();	// this is here to accommodate print/draw requests issued from the game object
}

void vid_restart()	{

	int* width = getCvarAddress_I("r_width");
	int* height = getCvarAddress_I("r_height");

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
//	const char* gl_shading = (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);

	Con_print("___OpenGL Version Information___");
	Con_print("Vendor: %s", gl_vendor);
	Con_print("Renderer: %s", gl_renderer);
	Con_print("OpenGL Version: %s", gl_version);
//	Con_print("Shader Version: %s", gl_shading);

}

void setAnimation(entity_t* e, string animName)	{
	if( rendarar )
		rendarar->setAnimation(e, animName);
}

void RegisterEntityWithScene(string model, entity_t* ent)	{
	if( rendarar )
		rendarar->addEntityToScene(model, ent);
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

void getCameraPos(vec3_t v)	{
	if( rendarar )
		rendarar->getCameraPos(v);
}

void getCameraFacing(vec3_t v)	{
	if( rendarar )
		rendarar->getCameraFacing(v);
}

bsp_node_t* getBSPTree()	{
	if( rendarar )
		return rendarar->bspRoot;

	return NULL;
}

void listCvars()	{
	cvarReg->listCvars();
}

void printTextures()	{
	if( rendarar )
		rendarar->printTextures();
}

void polygonCount()	{
	if( rendarar )
		Con_print("Cached polygons in scene: %d", rendarar->getCachedPolygonCount());
}

void screenPrint(int x, int y, const char* fmt, ...)	{
	va_list args;
	va_start(args,fmt);
	char str[1024];
	vsprintf(str, fmt, args);
	va_end(args);

	if( rendarar )
		rendarar->screenPrint(x, y, str);
}

void setGameCallBacks()	{
	void* funcs[] = {
			(void*)&Con_print,
			(void*)&LoadMap,
			(void*)&LoadModel,
			(void*)&RegisterEntityWithScene,
			(void*)&setAnimation,
			(void*)&getBSPTree,
			(void*)&getCvarAddress_I,
			(void*)&getCvarAddress_D,
			(void*)&getCvarAddress_S,
			(void*)&getCvarAddress_F,
			(void*)&registerCvar,
			(void*)&registerCommand,
			(void*)&registerCommandWithArgs,
			(void*)&getCameraPos,
			(void*)&getCameraFacing,
			(void*)&screenPrint
	};

	g->setBulkCallBacks(funcs);
}


#ifdef linux
void* gameLibHandle;
typedef Game* create_t();
typedef void destroy_t(Game*);

void loadGameLib(string soName)	{
	string canonicalPath = getProgramPath() + "/" + soName;
	cout << "Attempting to load game: " << canonicalPath << endl;
	gameLibHandle = dlopen(canonicalPath.c_str(), RTLD_LAZY);

	if( gameLibHandle == NULL )	{
		cerr << endl << "LoadGame: NULL " << dlerror() << endl;
		return;
	}


	create_t* GetGame = (create_t*) dlsym(gameLibHandle,"maker");

	g = GetGame();
}

void unloadGameLib()	{

	if( gameLibHandle == NULL )	{
		cerr << "Unload Game: No game loaded." << endl;
		return;
	}

	destroy_t* DeleteGame = (destroy_t*) dlsym(gameLibHandle,"destroyer");

	DeleteGame(g);

	dlclose(gameLibHandle);

	cout << "GameLib closed." << endl;
}

#endif

#ifdef _WIN32
#include <windows.h>
typedef Game* create_t();
typedef void destroy_t(Game*);

HINSTANCE winDLLHandle;

void loadGameDLL(string dllName)	{
	Con_print("Loading DLL...");
	winDLLHandle = LoadLibrary(dllName.c_str());

	if( winDLLHandle != NULL )	{
		create_t* getGame = (create_t*) GetProcAddress(winDLLHandle, "maker");
		g = getGame();
		Con_print("DLL Loaded: %s", g->getName().c_str());
	}
	else	{
		cout << "Couldn't load DLL: " << GetLastError() << endl;
	}

}

void unloadGameDLL()	{

	if( winDLLHandle == NULL )	{
		cout << "Unload Game: No game loaded." << endl;
		return;
	}

	destroy_t* freeGame = (destroy_t*) GetProcAddress(winDLLHandle, "destroyer");

	freeGame(g);

	FreeLibrary(winDLLHandle);
	Con_print("DLL Unloaded");

}
#endif

void loadGame(string gameLib)	{
#ifdef linux
	loadGameLib(gameLib);
#endif

#ifdef _WIN32
	loadGameDLL(gameLib);
#endif

	setGameCallBacks();
	g->init();

	g->newPacket();
}

void unloadGame()	{
#ifdef linux
	unloadGameLib();
#endif

#ifdef _WIN32
	unloadGameDLL();
#endif

	Con_print("Game Library Unloaded.");
}


void printEngineTime()	{
	Con_print("Engine Time: %d", rendarTimeStamp);
}


void shutdown()	{
	unloadGame();
	delete rendarar;
	delete con;
	delete cvarReg;
}

int main(int argc, char** argv) {

	programPath = getCWD();

	cvarReg = new CvarRegister();
	registerCvar("cwd",				getCWD(),				STRING_CVAR);
	registerCvar("name", 			"defaultPlayer", 		STRING_CVAR);
	registerCvar("r_width", 		"800", 					INT_CVAR);
	registerCvar("r_height", 		"600", 					INT_CVAR);
	registerCvar("r_full", 			"0", 					INT_CVAR);
	registerCvar("r_fov",			"45",					INT_CVAR);
	registerCvar("r_modelPath",		getCWD()+"/models/",	STRING_CVAR);
	registerCvar("r_imagePath", 	getCWD()+"/images/",	STRING_CVAR);
	registerCvar("z_nearClip", 		"0.1",					DOUBLE_CVAR);
	registerCvar("z_farClip", 		"500",					DOUBLE_CVAR);

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

	registerCommand(		"quit", shutdown);
	registerCommandWithArgs("echo", echo, true);
	registerCommand(		"vid_restart", vid_restart);

	registerCommand(		"gl_info", printGLInfo);
	registerCommandWithArgs("gl_loadmodel", LoadModel, true);
	registerCommandWithArgs("gl_loadmap", LoadMap, true);
	registerCommand(		"gl_polycount", polygonCount);

	registerCommandWithArgs("game_load", loadGame, true);
	registerCommand(		"game_unload", unloadGame);

	registerCommandWithArgs("loadmd2", LoadModel, true);

	registerCommand(		"tex_list", printTextures);
	registerCommand(		"cvarList", listCvars);

	registerCommand(		"et",		printEngineTime);

	rendarar->run();



	cout << "!@#$%^&*Unreachable code" << endl;

	return 0;
}
