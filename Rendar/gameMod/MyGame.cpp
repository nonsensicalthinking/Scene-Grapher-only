#include "MyGame.h"


extern "C" Game* maker()	{
	return new MyGame();
}

extern "C" void destroyer(Game* g)	{
	delete g;
	cout << "Game Unloaded" << endl;
}

MyGame::MyGame() : Game()	{
	Con_print("MyGame loading...");
	setName("MyGame!");
}


MyGame::~MyGame()	{

}

void MyGame::newPacket()	{
	cout << "Replaced new packet functionality." << endl;
}




