#include "Game.h"


#ifndef MYGAME_H_
#define MYGAME_H_


class MyGame : public Game	{
	int nextGameId;
	entity_t* flyer;
	double* g_maxThrust;
	double* g_thrustInterval;

public:
	MyGame();
	virtual ~MyGame();
	void init();
	void printGameInfo();
	void newPacket();
	void processNormalKeys(unsigned char key, int x, int y);
	void perFramePostPhysics();
};

#endif
