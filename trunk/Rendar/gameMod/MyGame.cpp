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
	registerCvar("g_maxthrust", "2000.0", CVAR_DOUBLE);
	registerCvar("g_thrustinterval", "0.5", CVAR_DOUBLE);
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
		e->mass = new Helicopter(1);
		getCameraPos(pos);
		getCameraFacing(facing);
		VectorMA(pos, facing, 10, pos);
		VectorCopy(pos, e->mass->prevPos);
		VectorCopy(pos, e->mass->pos);
//		VectorCopy(facing, e->facing);
		RegisterEntityWithScene("greenalien.md2", e);	// make entity visible
		activeEntities = doublyLinkEntities(activeEntities, e);			// make entity active!
		setAnimation(e, "stand_");
		flyer = e;
		break;

	case 'x':
		Helicopter* m = (Helicopter*)flyer->mass;

		if( m->upwardThrust < *g_maxThrust )
			m->upwardThrust += *g_thrustInterval;

		Con_print("Flyer Thrust: %.2f", m->upwardThrust);

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
		Helicopter* h = (Helicopter*)flyer->mass;

		screenPrint(20, 20, "Upward Thrust: %.4f", h->upwardThrust);
		screenPrint(20, 36, "X, Y, Z: %.4f, %.4f, %.4f", h->pos[0], h->pos[1], h->pos[2]);
		screenPrint(20, 52, "Velocity: %.4f, %.4f, %.4f", h->vel[0], h->vel[1], h->vel[2]);
	}
}
