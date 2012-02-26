#include "Game.h"


Game::Game()	{

}


Game::~Game()	{

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


