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
	registerCvar("g_maxthrust", "12.0", CVAR_DOUBLE);
	registerCvar("g_thrustinterval", "0.05", CVAR_DOUBLE);
	g_maxThrust = getCvarAddress_D("g_maxthrust");
	g_thrustInterval = getCvarAddress_D("g_thrustinterval");
	LoadModel("tall-alien.md2");
	LoadModel("greenalien.md2");
	LoadMap("simple-demo.obj");

}

void MyGame::processNormalKeys(unsigned char key, int x, int y)	{
	vec3_t pos;
	vec3_t facing;
	entity_t* e;

	switch(key)	{
	case 'l':
		e = createEntity();
		e->mass = new Mass(5);
		getCameraPos(pos);
		getCameraFacing(facing);
		VectorMA(pos, facing, 10, pos);
		VectorCopy(pos, e->mass->pos);
		VectorCopy(facing, e->facing);
		RegisterEntityWithScene("tall-alien.md2", e);	// make entity visible!
		activeEntities = doublyLinkEntities(activeEntities, e);			// make entity active!
		setAnimation(e, "run_");
		break;

	case 'k':
		e = createEntity();
		flyer = new Helicopter(1);
		e->mass = flyer;
		getCameraPos(pos);
		getCameraFacing(facing);
		VectorMA(pos, facing, 10, pos);
		VectorCopy(pos, e->mass->prevPos);
		VectorCopy(pos, e->mass->pos);
//		VectorCopy(facing, e->facing);
		RegisterEntityWithScene("greenalien.md2", e);	// make entity visible
		activeEntities = doublyLinkEntities(activeEntities, e);			// make entity active!
		setAnimation(e, "stand_");
		break;

	case 'x':
		if( flyer->upwardThrust < *g_maxThrust )
			flyer->upwardThrust += *g_thrustInterval;

		break;

	case 'c':
		if( flyer->upwardThrust > 0 )
			flyer->upwardThrust -= *g_thrustInterval;

		break;
	}
}


void MyGame::newPacket()	{
	cout << "Replaced new packet functionality." << endl;
}

void MyGame::printGameInfo()	{
	Con_print("MyGame loading...%s", "haha");
}

void MyGame::perFramePostPhysics()	{

	if( flyer )	{
		screenPrint(0, 100, "Upward Thrust: %.4f", flyer->upwardThrust);
		screenPrint(0, 84, "X, Y, Z: %.4f, %.4f, %.4f", flyer->pos[0], flyer->pos[1], flyer->pos[2]);
		screenPrint(0, 68, "Velocity: %.4f, %.4f, %.4f", flyer->vel[0], flyer->vel[1], flyer->vel[2]);
	}
}
