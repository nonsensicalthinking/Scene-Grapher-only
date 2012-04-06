/*
 * ent.h
 *
 *  Created on: Aug 3, 2010
 *      Author: brof
 */

#include "md2model.h"

#ifndef ENT_H_
#define ENT_H_

#define COLLISION_NONE		0x01	// 1
#define COLLISION_BOX		0x02	// 2
#define COLLISION_SPHERE	0x04	// 4
#define COLLISION_CYLINDER	0x08	// 8
#define COLLISION_SAME_TYPE	0x10	// 16

extern long timeStamp;	// our current time reference in milliseconds

typedef struct cylinder_s	{
	vec3_t centerAxis;
	vec3_t normal;	// will be used for telling which direction the bat is moving
	float radius;

}cylinder_t;

typedef struct entity_s	{
	unsigned int entID;
	bool parishable;
	int expired;
	bool hasExpired;

	vec3_t facing;

	string md2name;
	MD2Model* model;

	Mass* mass;	// Mass' current state

	// Collision Stuffs
	char collisionType;	// stores bit flag for type of collision
	float radius;	// for sphere collision
	cylinder_t* cylinder;	// constraints of cylinder
//	bbox_t*	boundingBox; 	// constraints of bounding box
	// end collision stuffs

	struct entity_s* prev;
	struct entity_s* next;

	// flags for gameplay

	// Returns TRUE if time is expired
	// FALSE otherwise
	bool checkTTL()	{
		if( timeStamp >= expired )
			return true;

		return false;
	}

	void setTTL(int ms)	{
		expired = timeStamp + ms;
	}

}entity_t;


inline entity_t* createEntity()	{
	static unsigned int entCount = 0;
	entity_t* ent = new entity_t;
	ent->entID = entCount++;
	ent->hasExpired = false;
	ent->expired = 0;
	ent->mass = NULL;
	ent->md2name = "";
	ent->model = NULL;
	ent->collisionType = COLLISION_NONE;

	ent->facing[0] = 1.0;
	ent->facing[1] = 0;
	ent->facing[2] = 0;

	ent->prev = NULL;
	ent->next = NULL;

	return ent;
}

inline void cleanEntity(entity_t* ent)	{
	ent->hasExpired = false;
	ent->expired = 0;
//	if( ent->mass )
//		delete ent->mass;
	ent->mass = NULL;
	ent->md2name = "";
//	if( ent->model )
//		delete ent->model;
	ent->model = NULL;
	ent->collisionType = COLLISION_NONE;
	ent->radius = 0;
	ent->prev = NULL;
	ent->next = NULL;
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

// returns new front of list
// returns e when e is the front of the list and no other items
// returns NULL on error
inline entity_t* unlinkEntity(entity_t* list, entity_t* e)	{

	entity_t* curEnt = list;

	while(curEnt != NULL)	{
		if( curEnt->entID == e->entID )	{
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
			else if( newPrevPoly == NULL )	{	// item was at front of list and has a next
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




#endif /* ENT_H_ */
