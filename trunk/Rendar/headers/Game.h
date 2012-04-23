#include <iostream>
#include <math.h>
#include "shared.h"
#include "bsptree.h"
#include "entity.h"


#define CVAR_UNUSED		0
#define CVAR_INT 		1
#define CVAR_DOUBLE 	2
#define CVAR_STRING 	4
#define CVAR_FLOAT		6

using namespace std;



class Game	{
	int nextGameId;

protected:
	string gameName;

	float timeElapsed;	// in seconds (?)
	float* maxPossible_dt;

	entity_t* activeEntities;
	entity_t* unusedEnts;

	bsp_node_t* bspRootNode;



	void (*Con_print)(const char*, ...);
	void (*LoadMap)(string);
	void (*LoadModel)(string);
	void (*RegisterEntityWithScene)(string, entity_t*);
	void (*setAnimation)(entity_t*, string);
	bsp_node_t* (*getBSPTree)();
	int* (*getCvarAddress_I)(string);
	double* (*getCvarAddress_D)(string);
	string* (*getCvarAddress_S)(string);
	float* (*getCvarAddress_F)(string);
	void (*registerCvar)(string, string, int);
	void (*registerCommand)(string, void (*func)());
	void (*registerCommandWithArgs)(string, void (*func)(string), bool);
	void (*getCameraPos)(vec3_t);
	void (*getCameraFacing)(vec3_t);
	void (*screenPrint)(int, int, const char*, ...);

public:
	Game();
	virtual ~Game();
	virtual void init();
	void setBulkCallBacks(void* funcs[]);
	int getNextGameId();
	virtual void newPacket();
	void setName(string name);
	string getName();
	virtual void printGameInfo();
	virtual void processNormalKeys(unsigned char key, int x, int y);
	virtual void processSpecialKeys(int key, int x, int y);
	virtual void advance(float dt);
	virtual void perFramePostPhysics();
	void processMapFileLine(string line);
	void loadMapFile(string map);
	virtual void collideWithPolygons(entity_t* ent, polygon_t* list);
	virtual void collideWithEntities(entity_t* ent, entity_t* list);
	virtual void insertEntityIntoBSPTree(entity_t* ent);
	void entPolyCollision(entity_t* ent, polygon_t* p);
	void entEntCollision(entity_t* e1, entity_t* e2);
	bool testAABBCollision(entity_t* a, entity_t* b);
};

