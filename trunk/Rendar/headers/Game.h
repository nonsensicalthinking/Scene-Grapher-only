#include <iostream>

using namespace std;

extern void Con_print(const char* fmt, ...);

class Game	{

protected:
	string gameName;

public:
	Game();
	virtual ~Game();

	virtual void newPacket();
	void setName(string name);
	string getName();
};
