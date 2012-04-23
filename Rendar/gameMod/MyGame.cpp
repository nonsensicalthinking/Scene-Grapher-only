#include "MyGame.h"

extern "C" Game* maker()	{
	return new MyGame();
}

extern "C" void destroyer(Game* g)	{
	delete g;
	cout << "Game Unloaded" << endl;
}


double* modelAdvancerate;

MyGame::MyGame() : Game()	{
	setName("MyGame!");

	//TODO REMOVE FLYER OBJECT
	flyer = NULL;
}


MyGame::~MyGame()	{

}

void MyGame::init()	{
	registerCvar("g_name", getName(), CVAR_STRING);
	registerCvar("g_maxthrust", "2100.0", CVAR_DOUBLE);
	registerCvar("g_thrustinterval", "50.0", CVAR_DOUBLE);
	g_maxThrust = getCvarAddress_D("g_maxthrust");
	g_thrustInterval = getCvarAddress_D("g_thrustinterval");

	loadMapFile("desert-field.map");

}

void MyGame::processNormalKeys(unsigned char key, int x, int y)	{
	vec3_t pos;
	vec3_t facing;
	entity_t* e;

	switch(key)	{
	case 'l':
		e = createEntity();
		e->gameID = getNextGameId();
		e->mass = new Mass(5);
		getCameraPos(pos);
		getCameraFacing(facing);
		VectorMA(pos, facing, 10, pos);
		VectorCopy(pos, e->mass->pos);
		VectorCopy(facing, e->mass->facing);
		RegisterEntityWithScene("tall-alien.md2", e);	// make entity visible!
		activeEntities = doublyLinkEntities(activeEntities, e);			// make entity active!
		setAnimation(e, "run_");
		break;

	case 'k':
		e = createEntity();
		e->gameID = getNextGameId();
		flyer = new Helicopter(1000.0);	// mass in kg?
		e->mass = flyer;
		getCameraPos(pos);
		getCameraFacing(facing);
		VectorMA(pos, facing, 10, pos);
		VectorCopy(pos, e->mass->pos);
		VectorCopy(facing, e->mass->facing);
		RegisterEntityWithScene("hheli-sm.md2", e);	// make entity visible
		activeEntities = doublyLinkEntities(activeEntities, e);			// make entity active!
		setAnimation(e, "stand_");
		break;

	case 'x':
		if( flyer->power < *g_maxThrust )
			flyer->power += *g_thrustInterval;

		break;

	case 'c':
		if( flyer->power > 0 )
			flyer->power -= *g_thrustInterval;

		break;
	}
}

void MyGame::printGameInfo()	{
	Con_print("MyGame loading...%s", "haha");
}

void MyGame::perFramePostPhysics()	{

	if( flyer )	{
		screenPrint(0, 100, "Shaft Horsepower: %.4f", flyer->power);
		screenPrint(0, 84, "X, Y, Z: %.4f, %.4f, %.4f", flyer->pos[0], flyer->pos[1], flyer->pos[2]);
		screenPrint(0, 68, "Velocity: %.4f, %.4f, %.4f", flyer->vel[0], flyer->vel[1], flyer->vel[2]);
	}
}
