#include <iostream>

#define CVAR_UNUSED		0
#define CVAR_INT 		1
#define CVAR_DOUBLE 	2
#define CVAR_STRING 	4


using namespace std;



class Game	{

protected:
	string gameName;

	void (*Con_print)(const char*, ...);
	void (*LoadMap)(string);
	int* (*getCvarAddress_I)(string);
	double* (*getCvarAddress_D)(string);
	string* (*getCvarAddress_S)(string);
	void (*registerCvar)(string, string, int);
	void (*registerCommand)(string, void (*func)());
	void (*registerCommandWithArgs)(string, void (*func)(string), bool);


public:
	Game();
	virtual ~Game();
	virtual void init();
	void setBulkCallBacks(void* funcs[]);
	virtual void newPacket();
	void setName(string name);
	string getName();
	virtual void printGameInfo();
};

