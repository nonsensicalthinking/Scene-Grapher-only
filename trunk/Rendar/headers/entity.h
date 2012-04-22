#ifndef ENTITY_H_
#define ENTITY_H_

#include "shared.h"
#include "Mass.h"

//////////////// ENTITY & SUPPORT FUNCS ////////////////
static long rendarTimeStamp;

typedef struct entity_s {
	int gameID;
	string name;
	model_t* model;
	Mass* mass;

	bool perishable;
	long expirationTime;

	bool checkTTL()	{
		if( rendarTimeStamp >= expirationTime )
			return true;

		return false;
	}



	struct entity_s* prev;
	struct entity_s* next;
}entity_t;

inline entity_t* createEntity()	{
	entity_t* e = new entity_t;

	e->gameID = -1;
	e->name = "";
	e->model = NULL;
	e->mass = NULL;
	e->perishable = false;

	return e;
}

// prepends the new entity to the linked list pointed to by first
// returns new front of linked list
inline entity_t* doublyLinkEntities(entity_t* first, entity_t* newEnt)	{

	if( first != NULL )	{
		first->prev = newEnt;
		newEnt->next = first;
		newEnt->prev = NULL;
	}
	else	{
		newEnt->prev = NULL;
		newEnt->next = NULL;
	}

	return newEnt;
}

// appends entity to linked list (slower than doublyLinkEntities, avoid during rendering)
// returns front of list
inline entity_t* appendEntityToLinkedList(entity_t* list, entity_t* ent)	{

	if( list != NULL )	{
		entity_t* cur = list;

		while( cur->next != NULL )	// goto end of ll
			cur = cur->next;

		cur->next = ent;
		ent->prev = cur;
		ent->next = NULL;

		return list;
	}
	else	{
		ent->prev = NULL;
		ent->next = NULL;

		return ent;
	}

}

// returns new front of list
// returns e when e is the front of the list and no other items
// returns NULL on error
inline entity_t* unlinkEntity(entity_t* list, entity_t* e)	{

	entity_t* curEnt = list;

	while(curEnt != NULL)	{
		if( curEnt->gameID == e->gameID )	{
			entity_t* newNextEnt = e->next;
			entity_t* newPrevEnt = e->prev;

			if( newNextEnt != NULL && newPrevEnt != NULL )	{
				newNextEnt->prev = newPrevEnt;
				newPrevEnt->next = newNextEnt;
				return list;
			}
			else if( newNextEnt == NULL )	{	// item was at end of list and has prev
				newPrevEnt->next = NULL;
				return list;
			}
			else if( newPrevEnt == NULL )	{	// item was at front of list and has a next
				newNextEnt->prev = NULL;
				return newNextEnt;	// new front of list
			}
			else	{	// They're both NULL!
				return e;	// no other items in list, return p to indicate that it was front
			}
		}

		curEnt = curEnt->next;
	}

	return NULL;	// polygon not found in list
}

// returns new front of unusedList
// IMPORTANT:
// **free all memory within the entity before using this function**
// just re-initializes the struct and places
// it at the front of the unusedList provided.
inline entity_t* reuseEntity(entity_t* unusedList, entity_t* ent)	{
	ent->gameID = 0;
	ent->model = NULL;
	ent->name = "";
	ent->perishable = false;

	if( ent->mass )
		delete ent->mass;

	ent->next = NULL;
	ent->prev = NULL;

	return doublyLinkEntities(unusedList, ent);
}



//////////////// END ENTITY SUPPORT FUNCS ////////////////

#endif

