#include "Game.h"


#ifndef MYGAME_H_
#define MYGAME_H_


class MyGame : public Game	{
public:
	MyGame();
	virtual ~MyGame();
	void init();
	void printGameInfo();
	void newPacket();
};

#endif
