#include <iostream>
#include <map>
#include "shared.h"

#define UNUSED_CVAR	0
#define INT_CVAR	1
#define DOUBLE_CVAR	2
#define STRING_CVAR	4

typedef struct configVariable_s	{
	string name;
	int typeFlag;

	string s;
	int i;
	double d;
}cvar_t;



class CvarRegister	{

	map<string, cvar_t*> registeredCvars;

public:

	CvarRegister();
	~CvarRegister();

	void freeRegisteredCvars();
	bool cvarRegistered(string name);
	void registerCvar(string name, string value, int type);
	void setCvar(string args);
	int* getCvarAddress_I(string name);
	double* getCvarAddress_D(string name);
	string* getCvarAddress_S(string name);
	map<string,cvar_t*>::iterator lower_bound(string str);
	map<string,cvar_t*>::iterator upper_bound(string str);
};
