#include "Game.h"

Game::Game()	{
	unusedEnts = NULL;
	activeEntities = NULL;
	nextGameId = 0;
}


Game::~Game()	{

}

void Game::init()	{

}

void Game::advance(float dSec)        {

//	Con_print("Calling advance...[%.2f]", dSec);
//	Con_print("Max DT...........[%.2f]", *maxPossible_dt);

	// Time work, used for Simulation work
	// dt Is The Time Interval (As Seconds) From The Previous Frame To The Current Frame.
	// dt Will Be Used To Iterate Simulation Values Such As Velocity And Position Of Masses.


	// commenting this out shaved off 1 fps
	// before you un-comment, use multiplication instead!
	//              dt /= slowMotionRatio;          // Divide dt By slowMotionRatio And Obtain The New dt

	timeElapsed += dSec;                                                                                    // Iterate Elapsed Time
																															// This Is Needed So We Do Not Pass Over A Non Precise dt Value

	// test comment, this had +1 added to it...
	int numOfIterations = (int)(dSec / *maxPossible_dt);             // Calculate Number Of Iterations To Be Made At This Update Depending On maxPossible_dt And dt

	//              int numOfIterations = (dSec * 125);

	//              cout << "** numOfIterations: " << numOfIterations << "\n";
	//              cout << "** New numOfIterations: " << testNumOfIterations << "\n";

	if (numOfIterations != 0)	// numOfIterations should never be zero, but just in case
			dSec = dSec / numOfIterations;	// calc dt per iteration

//	Con_print("numOfIterations[%d]", numOfIterations);

	for(int iteration=0; iteration < numOfIterations; iteration++)	{

		// TODO remove all dynamic entities from bsp tree for calc/reinsertion

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

				// TODO Do something
				e->mass->operate(dSec);
//				Con_print("e->mass->pos: [%.2f, %.2f, %.2f]", e->mass->pos[0], e->mass->pos[1], e->mass->pos[2]);
				e = e->next;

				// TODO Place entity back into BSP Tree for collisions
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
}

void Game::printGameInfo()	{

}

void Game::processNormalKeys(unsigned char key, int x, int y)	{

}

void Game::processSpecialKeys(int key, int x, int y)	{

}

