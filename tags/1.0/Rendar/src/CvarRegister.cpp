#include "CvarRegister.h"
#include <stdlib.h>
#include <string.h>
#include "strtools.h"

extern void Con_print(const char* fmt, ...);

CvarRegister::CvarRegister()	{

}


CvarRegister::~CvarRegister()	{
	freeRegisteredCvars();

}


void CvarRegister::freeRegisteredCvars()	{
	map<string, cvar_t*>::iterator itr;
	for(itr=registeredCvars.begin();itr!=registeredCvars.end();itr++)	{
		cvar_t* cvar = (*itr).second;
		delete cvar;
	}
}


bool CvarRegister::cvarRegistered(string name)	{
	string_tolower(name);

	if( registeredCvars.find(name) == registeredCvars.end() )
		return false;

	return true;
}

void CvarRegister::registerCvar(string name, string value, int type)	{
	string preservedName = name;
	string_tolower(name);


	if( registeredCvars.find(name)!=registeredCvars.end() )	{
		Con_print("Cvar naming conflict: [%s] already exists as a cvar.", name.c_str());
		return;
	}

	cvar_t* cvar = new cvar_t;

	Con_print("CVAR REGISTERED: [%s] Value: [%s]", preservedName.c_str(), value.c_str());

	cvar->typeFlag = type;
	cvar->name = preservedName;

	switch(type)	{
		case INT_CVAR:
			cvar->i = atoi(value.c_str());
			break;
		case DOUBLE_CVAR:
			cvar->d = atof(value.c_str());
			break;
		case STRING_CVAR:
			cvar->s = value;
			break;
	}

	registeredCvars[name] = cvar;
}


void CvarRegister::setCvar(string args)	{
	char parse[512];
	strncpy(parse, args.c_str(), 512);
	char* s = strtok(parse, " \t\n\r");
	char* value = strtok(NULL, " \t\n\r");
	string cvar = s;
	string_tolower(cvar);

	map<string,cvar_t*>::iterator itr;
	itr = registeredCvars.find(cvar);
	cvar_t* c = (*itr).second;

	if( args.find_first_of(" ") == string::npos )	{
		switch(c->typeFlag)	{
			case INT_CVAR:
				Con_print("%s = %d", c->name.c_str(), c->i);
				break;
			case DOUBLE_CVAR:
				Con_print("%s = %f", c->name.c_str(), c->d);
				break;
			case STRING_CVAR:
				Con_print("%s = %s", c->name.c_str(), c->s.c_str());
				break;
		}
	}
	else	{
		if( itr != registeredCvars.end() )	{
			switch(c->typeFlag)	{
				case INT_CVAR:
					c->i = atoi(value);
					Con_print("%s = %d", c->name.c_str(), c->i);
					break;
				case DOUBLE_CVAR:
					c->d = atof(value);
					Con_print("%s = %f", c->name.c_str(), c->d);
					break;
				case STRING_CVAR:
					c->s = value;
					Con_print("%s = %s", c->name.c_str(), c->s.c_str());
					break;
			}
		}
		else	{
			Con_print("%s, No such cvar registered.", cvar.c_str());
		}
	}
}


int* CvarRegister::getCvarAddress_I(string name)	{
	map<string,cvar_t*>::iterator itr;

	string_tolower(name);
	itr = registeredCvars.find(name);

	if( itr == registeredCvars.end() )
		return NULL;

	cvar_t* cvar = (*itr).second;
	return &(cvar->i);
}

double* CvarRegister::getCvarAddress_D(string name)	{
	map<string,cvar_t*>::iterator itr;

	string_tolower(name);
	itr = registeredCvars.find(name);

	if( itr == registeredCvars.end() )
		return NULL;

	cvar_t* cvar = (*itr).second;
	return &(cvar->d);
}

string* CvarRegister::getCvarAddress_S(string name)	{
	map<string,cvar_t*>::iterator itr;

	string_tolower(name);
	itr = registeredCvars.find(name);

	if( itr == registeredCvars.end() )
		return NULL;

	cvar_t* cvar = (*itr).second;
	return &(cvar->s);
}

map<string,cvar_t*>::iterator CvarRegister::lower_bound(string str)	{
	return registeredCvars.lower_bound(str);
}

map<string,cvar_t*>::iterator CvarRegister::upper_bound(string str)	{
	map<string,cvar_t*>::iterator itr;
	itr = registeredCvars.lower_bound(str);

	for(;itr!=registeredCvars.end(); itr++)	{
		cvar_t* cvar = (*itr).second;

		int found = cvar->name.find(str);

		if( found != 0 )
			return itr;
	}

	return itr;
}






