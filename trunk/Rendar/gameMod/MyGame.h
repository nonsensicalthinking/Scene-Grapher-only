#include "Game.h"


#ifndef MYGAME_H_
#define MYGAME_H_

typedef struct gent_s	{
	entity_t* sceneEnt;
}gent_t;


class MyGame : public Game	{
public:
	MyGame();
	virtual ~MyGame();
	void init();
	void printGameInfo();
	void newPacket();
	void processNormalKeys(unsigned char key, int x, int y);
};

#endif
