#include "Game.h"
#include <stdio.h>

#define WHITESPACE " \t\n\r"

Game::Game()	{
	unusedEnts = NULL;
	activeEntities = NULL;
	nextGameId = 0;
	bspRootNode = NULL;
}


Game::~Game()	{

}

void Game::init()	{

}

void Game::advance(float dSec)        {

	// Time work, used for Simulation work
	// dt Is The Time Interval (As Seconds) From The Previous Frame To The Current Frame.
	// dt Will Be Used To Iterate Simulation Values Such As Velocity And Position Of Masses.

	timeElapsed += dSec;	// Iterate Elapsed Time
							// This Is Needed So We Do Not Pass Over A Non Precise dt Value
	// test comment, this had +1 added to it...
	int numOfIterations = (int)(dSec / *maxPossible_dt);             // Calculate Number Of Iterations To Be Made At This Update Depending On maxPossible_dt And dt

	if (numOfIterations != 0)	// numOfIterations should never be zero, but just in case
			dSec = dSec / numOfIterations;	// calc dt per iteration

	for(int iteration=0; iteration < numOfIterations; iteration++)	{

		// TODO remove all dynamic entities from bsp tree for calc/reinsertion
		clearBSPEntityReferences(bspRootNode);


		entity_t* removalList = NULL;
		entity_t* e = activeEntities;

		// NOTES: this while loop is sticky.  in the if statement for perishable and checkTTL()
		// we need to increment the pointer first so we don't ref a deleted portion of memory.
		// This means that we also need to increment the pointer in the else statement.
		while( e != NULL )	{
			if( e->perishable && e->checkTTL() )	{
				entity_t* rem = e;
				e = e->next;

				activeEntities = unlinkEntity(activeEntities, rem);
				// TODO properly re-initialize ent to be reused (rem)
				unusedEnts = reuseEntity(unusedEnts, rem);
			}
			else	{
				e->mass->operate(dSec);
				insertEntityIntoBSPTree(e);
				e = e->next;
			}
		}
	}

	perFramePostPhysics();

}


void Game::perFramePostPhysics()	{

}

void Game::setBulkCallBacks(void* funcs[])	{
	int x=0;
	Con_print = (void (*)(const char*, ...))funcs[x++];
	LoadMap = (void (*)(string))funcs[x++];
	LoadModel = (void (*)(string))funcs[x++];
	RegisterEntityWithScene = (void (*)(string, entity_t*))funcs[x++];
	setAnimation = (void (*)(entity_t*, string))funcs[x++];
	getBSPTree = (bsp_node_t* (*)())funcs[x++];
	getCvarAddress_I = (int* (*)(string))funcs[x++];
	getCvarAddress_D = (double* (*)(string))funcs[x++];
	getCvarAddress_S = (string* (*)(string))funcs[x++];
	getCvarAddress_F = (float* (*)(string))funcs[x++];
	registerCvar = (void (*)(string, string, int))funcs[x++];
	registerCommand = (void (*)(string, void (*func)()))funcs[x++];
	registerCommandWithArgs = (void (*)(string, void (*)(string), bool))funcs[x++];
	getCameraPos = (void (*)(vec3_t))funcs[x++];
	getCameraFacing = (void (*)(vec3_t))funcs[x++];
	screenPrint = (void (*)(int, int, const char*, ...))funcs[x++];

	registerCvar("g_max_dt", "0.008", 6);	//float cvar
	maxPossible_dt = getCvarAddress_F("g_max_dt");

}

void Game::setName(string name)	{
	gameName = name;
}

string Game::getName()	{
	return gameName;
}

int Game::getNextGameId()	{
	return nextGameId++;
}

void Game::newPacket()	{
	cout << "Original New Packet Functionality." << endl;
	loadMapFile("desert-field.map");
}

void Game::printGameInfo()	{

}

void Game::processNormalKeys(unsigned char key, int x, int y)	{

}

void Game::processSpecialKeys(int key, int x, int y)	{

}

void Game::processMapFileLine(string line)	{

	char* token;
	char str[512];

	strncpy(str, line.c_str(), 512);
	token = strtok(str, WHITESPACE);

	if( token == NULL )
		return;

	if( !strcmp(token, "map") )	{
		token = strtok(NULL, WHITESPACE);
		LoadMap(token);
		Con_print("MAP: Loading OBJ Map: %s", token);
	}
	else if( !strcmp(token, "md2") )	{
		token = strtok(NULL, WHITESPACE);
		LoadModel(token);
		Con_print("MAP: Loading MD2 Model: %s", token);
	}

}

void Game::loadMapFile(string map)	{
	string* mapPath = getCvarAddress_S("r_mapPath");
	string canonicalPath = *mapPath + map;

	FILE* fp;
	if( !(fp=fopen(canonicalPath.c_str(), "r")) )	{
		Con_print("Map: File not found - %s", canonicalPath.c_str());
		return;
	}

	while( !feof(fp) )	{
		char in[512];
		fgets(in, 512, fp);

		if( in[0] == '#' )
			continue;

		processMapFileLine(in);

		in[0] = '\0';
	}

	fclose(fp);

	bspRootNode = getBSPTree();

	Con_print("Map load complete -- %s loaded.", map.c_str());

}


bool Game::testAABBCollision(entity_t* a, entity_t* b)
{
    if(abs(a->mass->pos[0] - b->mass->pos[0]) > (a->mass->xyzRadii[0] + b->mass->xyzRadii[0]))
    		return false;
    if(abs(a->mass->pos[1] - b->mass->pos[1]) > (a->mass->xyzRadii[1] + b->mass->xyzRadii[1]))
    	return false;
    if(abs(a->mass->pos[2] - b->mass->pos[2]) > (a->mass->xyzRadii[2] + b->mass->xyzRadii[2]))
    	return false;

    // We have an overlap
    return true;
};

void Game::collideWithPolygons(entity_t* ent, polygon_t* list)	{
	polygon_t* curPoly = list;

	while( curPoly )	{
		entPolyCollision(ent, curPoly);
		curPoly = curPoly->next;
	}
}

void Game::collideWithEntities(entity_t* ent, entity_t* list)	{
	entity_t* curEnt = list;

	while( curEnt )	{
		entEntCollision(ent, curEnt);
		curEnt = curEnt->bspNext;
	}
}

void Game::insertEntityIntoBSPTree(entity_t* ent)	{
	// TODO make a version of findBSPLeaf that searches a circular area
	bsp_node_t* leaf = findBSPLeaf(bspRootNode, ent->mass->pos);


//	collideWithPolygons(ent, leaf->getPolygonList());
	collideWithEntities(ent, leaf->entityList);

	leaf->addEntity(ent);
}

void Game::entPolyCollision(entity_t* ent, polygon_t* p)	{
	// TODO implement box/poly collision







}

void Game::entEntCollision(entity_t* e1, entity_t* e2)	{
	if( testAABBCollision(e1, e2) )	{
		Con_print("AABB Collision: ent1[id:%d] ent2[id:%d]", e1->gameID, e2->gameID);
		// collision between e1, e2
		// TODO call notifiers for collision
	}
}




