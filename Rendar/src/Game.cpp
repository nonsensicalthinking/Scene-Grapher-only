#include "Game.h"


Game::Game()	{

}


Game::~Game()	{

}

void Game::init()	{

}

void Game::setBulkCallBacks(void* funcs[])	{
	int x=0;
	Con_print = (void (*)(const char*, ...))funcs[x++];
	LoadMap = (void (*)(string))funcs[x++];
	LoadModel = (void (*)(string))funcs[x++];
	RegisterEntityWithScene = (void (*)(string, vec3_t, vec3_t, int))funcs[x++];
	getBSPTree = (bsp_node_t* (*)())funcs[x++];
	getCvarAddress_I = (int* (*)(string))funcs[x++];
	getCvarAddress_D = (double* (*)(string))funcs[x++];
	getCvarAddress_S = (string* (*)(string))funcs[x++];
	registerCvar = (void (*)(string, string, int))funcs[x++];
	registerCommand = (void (*)(string, void (*func)()))funcs[x++];
	registerCommandWithArgs = (void (*)(string, void (*)(string), bool))funcs[x++];
	getCameraPos = (void (*)(vec3_t))funcs[x++];
	getCameraFacing = (void (*)(vec3_t))funcs[x++];
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


