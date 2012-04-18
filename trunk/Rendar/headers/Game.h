#include <iostream>
#include "shared.h"
#include "bsptree.h"
#include "entity.h"
#include "physics.h"


#define CVAR_UNUSED		0
#define CVAR_INT 		1
#define CVAR_DOUBLE 	2
#define CVAR_STRING 	4


using namespace std;



class Game	{

protected:
	string gameName;

	float timeElapsed;	// in seconds (?)
	float* maxPossible_dt;

	Simulation* physics;

	entity_t* activeEntities;
	entity_t* unusedEnts;




	void (*Con_print)(const char*, ...);
	void (*LoadMap)(string);
	void (*LoadModel)(string);
	entity_t* (*RegisterEntityWithScene)(string, vec3_t, vec3_t, int);
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

public:
	Game();
	virtual ~Game();
	virtual void init();
	void setBulkCallBacks(void* funcs[]);
	virtual void newPacket();
	void setName(string name);
	string getName();
	virtual void printGameInfo();
	virtual void processNormalKeys(unsigned char key, int x, int y);
	virtual void processSpecialKeys(int key, int x, int y);
	virtual void advance(float dt);
};

