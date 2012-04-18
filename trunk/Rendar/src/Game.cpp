#include "Game.h"

Game::Game()	{

}


Game::~Game()	{

}

void Game::init()	{

}

void Game::advance(float dSec)        {

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

	if (numOfIterations != 0)                                               // Avoid Division By Zero
			dSec = dSec / numOfIterations;                                  // dt Should Be Updated According To numOfIterations


	if( physics != NULL )	{
		for(int iteration=0; iteration < numOfIterations; iteration++)	{

			// TODO remove all dynamic entities from bsp tree for calc/reinsertion

			entity_t* removalList = NULL;

			for(entity_t* e=activeEntities; e != NULL; e=e->next)	{
				if( e->parishable && e->checkTTL() )	{
					activeEntities = unlinkEntity(activeEntities, e);
					// TODO properly re-initialize ent to be reused (e)
					unusedEnts = reuseEntity(unusedEnts, e);
				}
				else	{
					// TODO Do something
					switch(e->mass->moveType)	{
						case STATIONARY:
							break;
						case MOBILE:
							physics->operate(dSec, e->mass);
							break;
					}


					// TODO Place entity back into BSP Tree for collisions
				}

			}



		}
	}



/*
	// Simulation work from here down
	if( bbPhys != NULL )    {

		for (int a = 0; a < numOfIterations; ++a)       // We Need To Iterate Simulations "numOfIterations" Times
		{
			// we don't need to remove all ents, when re-adding it will re-classify (I think, investigate this)
	//                      removeEntitiesFromBSPTree();

			// for number of masses do
			vector<entity_t*> removalList;

			for( list<entity_t*>::iterator itr = entityList.begin(); itr != entityList.end(); itr++)        {
				if( (*itr)->parishable && (*itr)->checkTTL() )  {
					removalList.push_back((*itr));
				}
				else    {
					switch((*itr)->mass->moveType)  {
						case MOVE_TYPE_AT_REST:
								break;
						case MOVE_TYPE_BASEBALL:
								bbPhys->operate(dSec, (*itr)->mass);    // iterate the entity by dt seconds
								break;
					}

					// TODO Insert entity once its been operated instead of calling another
					// function to call a loop to do it.
				}
			}

			for( int x=0; x < removalList.size(); x++)      {
				bbPhys->release(removalList[x]->mass);
				entity_t* cur = removalList[x];
				cur->hasExpired = true;
				entityList.remove(cur);
				recycleEntity(cur);                                     // make entity available for use again.
			}

			insertEntitiesIntoBSPTree();
		}


	}
*/
}


void Game::setBulkCallBacks(void* funcs[])	{
	int x=0;
	Con_print = (void (*)(const char*, ...))funcs[x++];
	LoadMap = (void (*)(string))funcs[x++];
	LoadModel = (void (*)(string))funcs[x++];
	RegisterEntityWithScene = (entity_t* (*)(string, vec3_t, vec3_t, int))funcs[x++];
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

	registerCvar("g_max_dt", "0.008", 6);	//float cvar
	maxPossible_dt = getCvarAddress_F("g_max_dt");

}

void Game::setName(string name)	{
	gameName = name;
}

string Game::getName()	{
	return gameName;
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


