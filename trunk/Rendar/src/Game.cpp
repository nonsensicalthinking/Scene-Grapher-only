#include "Game.h"


Game::Game()	{

}


Game::~Game()	{

}

void Game::init()	{

}

void Game::setBulkCallBacks(void* funcs[])	{
	Con_print = (void (*)(const char*, ...))funcs[0];
	LoadMap = (void (*)(string))funcs[1];
	getCvarAddress_I = (int* (*)(string))funcs[2];
	getCvarAddress_D = (double* (*)(string))funcs[3];
	getCvarAddress_S = (string* (*)(string))funcs[4];
	registerCvar = (void (*)(string, string, int))funcs[5];
	registerCommand = (void (*)(string, void (*func)()))funcs[6];
	registerCommandWithArgs = (void (*)(string, void (*)(string), bool))funcs[7];

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

