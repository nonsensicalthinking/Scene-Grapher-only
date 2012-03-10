#include "MyGame.h"

extern "C" Game* maker()	{
	return new MyGame();
}

extern "C" void destroyer(Game* g)	{
	delete g;
	cout << "Game Unloaded" << endl;
}

MyGame::MyGame() : Game()	{
	setName("MyGame!");
}


MyGame::~MyGame()	{

}

void MyGame::init()	{
	registerCvar("g_name", getName(), CVAR_STRING);
	LoadModel("tallguy.md2");

}

void MyGame::processNormalKeys(unsigned char key, int x, int y)	{
	vec3_t pos;
	vec3_t facing;

	getCameraPos(pos);
	getCameraFacing(facing);

	switch(key)	{
	case 'l':
		RegisterEntityWithScene("tallguy.md2", pos, facing, 1);
		break;
	}
}


void MyGame::newPacket()	{
	cout << "Replaced new packet functionality." << endl;
}

void MyGame::printGameInfo()	{
	Con_print("MyGame loading...%s", "haha");
}


