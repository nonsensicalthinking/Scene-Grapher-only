/*
 * BSPTree.h
 *
 *  Created on: Jun 4, 2010
 *      Author: Derek Brooks
 */


#ifndef BSPTREE_H_
#define BSPTREE_H_
#include <list>
#include "entity.h"

#define BSP_RECURSION_DEPTH		11

#define		PLANE_NORMAL_X		0
#define 	PLANE_NORMAL_Y		1
#define		PLANE_NORMAL_Z		2

//#define BSPDEBUG



typedef struct bsp_node_s	{
public:
#ifdef BSPDEBUG
	int nodeNumber;
#endif
	bool root;
	plane_t* partition;
	bsp_node_s* parent;
	bsp_node_s* front;
	bsp_node_s* back;

	polygon_t* polygonList;
	int polygonListSize;
	entity_t* entityList;


	// node helpers

	bool isLeaf()	{
		if(front)
			return false;

		return true;
	}


	void addEntity(entity_t* ent)	{
		ent->bspNext = NULL;

		if( entityList == NULL )	{
			entityList = ent;
			return;
		}

		entity_t* cur = ent;
		while(cur->bspNext)	{
			cur = cur->bspNext;
		}

		cur->bspNext = ent;
	}


	// Encapsulate the polygon list so we aren't
	// tempted to modify it elsewhere.

	void addPolygon(polygon_t* poly)	{
		polygonList = doublyLinkPolygons(polygonList, poly);
		polygonListSize++;
	}

	void removePolygon(polygon_t* poly)	{

		polygon_t* p = unlinkPolygon(polygonList, poly);

		if( p == NULL )	{
			cout << "In func call removePolygon: Polygon not found." << endl;
			return;
		}

		// p is the only item in the polygonList
		if( p == poly )	{
			polygonList = NULL;
		}
		else	{	// p is the new front
			polygonList = p;
		}

		polygonListSize--;
	}

	void clearNode()	{
		if( partition )
			delete partition;

		polygon_t* curPoly = polygonList;
		polygon_t* freePoly;

		while( curPoly != NULL )	{
			freePoly = curPoly;
			curPoly = curPoly->next;
			delete freePoly;
		}

		entityList = NULL;
	}

	polygon_t* getPolygonList()	{
		return polygonList;
	}

	void setPolygonList(polygon_t* polyList)	{
		polygonList = polyList;

		if( polyList == NULL )	{
			polygonListSize = 0;
		}
		else	{
			polygon_t* curPoly = polygonList;

			while( curPoly != NULL )	{
				polygonListSize++;
				curPoly = curPoly->next;
			}

#ifdef BSPDEBUG
			cout << "bspNode->setPolygonList(): Polygon count: " << polygonListSize++ << endl;
#endif
		}
	}

	int getPolygonCount()	{
		return polygonListSize;
	}

}bsp_node_t;

// splitPolygon is only used in bsptree.cpp, hence it's lack of visiblity here
bsp_node_t* getNewBSPNode();
void buildTree(bsp_node_t*);
void bspInOrderBackToFront(bsp_node_t* tree);
void bspInOrderFrontToBack(bsp_node_t* tree);
void deleteTree(bsp_node_t* bspRoot);
void generateBSPTree(bsp_node_t* root, polygon_t* polygonList, float initialDiameter);
bsp_node_t* findBSPLeaf(bsp_node_t* bspRoot, const vec3_t pos);
void clearBSPEntityReferences(bsp_node_t* bspNode);


#endif /* BSPTREE_H_ */
