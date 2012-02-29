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
}

void MyGame::newPacket()	{
	cout << "Replaced new packet functionality." << endl;
}

void MyGame::printGameInfo()	{
	Con_print("MyGame loading...%s", "haha");
}



