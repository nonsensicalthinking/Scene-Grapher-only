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
}


MyGame::~MyGame()	{

}

void MyGame::init()	{
	registerCvar("g_name", getName(), CVAR_STRING);
	LoadModel("tall-alien.md2");
	LoadModel("greenalien.md2");
	LoadMap("simple-demo.obj");

}

void MyGame::processNormalKeys(unsigned char key, int x, int y)	{
	vec3_t pos;
	vec3_t facing;

	getCameraPos(pos);
	getCameraFacing(facing);

	entity_t* e;

	switch(key)	{
	case 'l':
		e = RegisterEntityWithScene("tall-alien.md2", pos, facing, 1);
		setAnimation(e, "run_");
		break;
	case 'k':
		e = RegisterEntityWithScene("greenalien.md2", pos, facing, 1);
		setAnimation(e, "stand_");
		break;
	}
}


void MyGame::newPacket()	{
	cout << "Replaced new packet functionality." << endl;
}

void MyGame::printGameInfo()	{
	Con_print("MyGame loading...%s", "haha");
}


