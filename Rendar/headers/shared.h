/*
 * shared.h
 *
 *	This header contains frequently used Vector Math functions
 *	and some ray tracing stuff.
 *
 *
 *  Created on: Jun 4, 2010
 *      Author: Derek Brooks
 */


#include <iostream>
#include <string>
#include <math.h>
#include "md2model.h"


using namespace std;

#ifndef SHARED_H_
#define SHARED_H_

#define	BACK				-1
#define	SPANNING			0
#define	FRONT				1


#define EPSILON				0.0000001


#define MAX_POLY_POINTS		10		// Max number of points in a polygon
#define MAX_FILE_LENGTH		64		// Max length for a filename

#define PI					3.14159265
#define PI_DIV_BY_180		PI / 180.0
#define ONE_RADIAN			PI_DIV_BY_180
#define TWO_PI_DIV_BY_60	(2.0 * PI) / 60.0
#define ONE_MILLISECOND		0.001


typedef float vec_t;
typedef vec_t vec2_t[2];
typedef vec_t vec3_t[3];
typedef vec_t vec4_t[4];

const vec3_t NORMAL_X = {1.0, 0.0, 0.0};
const vec3_t NORMAL_Y = {0.0, 1.0, 0.0};
const vec3_t NORMAL_Z = {0.0, 0.0, 1.0};

const vec3_t ZERO_VECTOR = {0.0, 0.0, 0.0};


typedef struct plane_s	{
	float diameter;
	vec3_t origin;
	vec3_t normal;
}plane_t;

typedef struct model_s	{
	bool isTemplate;
	GLint cacheID;
	string name;		// path?
	string action;		// animation to play, run_, walk_, death3_...
	vec3_t dimensions;	// bounding box dimensions
	MD2Model* md2;
}model_t;




// Begin functions

inline float degToRad(float deg)	{
	return deg * PI_DIV_BY_180;
}

// This is more or less what quake iii uses
// After testing it myself, it performs on average,
// twice as fast as its standard library counter part
// sqrt()
inline float FastSqrt(float x){
	float xhalf = 0.5f * x;
	float save = x;
	int i = *(int*)&x; // store floating-point bits in integer
	i = 0x5f3759d5 - (i >> 1); // initial guess for Newton's method
	x = *(float*)&i; // convert new bits into float
	x = x*(1.5f - xhalf*x*x); // One round of Newton's method
	return (save*x);
}



// Begin Vector Functions
inline void VectorPrint(const vec3_t v)	{
	cout << "[" << v[0] << ", " << v[1] << ", " << v[2] << "]";
}

inline void VectorPrint2f(const vec2_t v)	{
	cout << "[" << v[0] << ", " << v[1] << "]";
}

inline void VectorInit(vec3_t vec)	{
	vec[0] = 0;
	vec[1] = 0;
	vec[2] = 0;
}

inline void VectorCopy(const vec3_t a, vec3_t b)	{
	b[0] = a[0];
	b[1] = a[1];
	b[2] = a[2];
}

inline void VectorCopy2f(const vec2_t a, vec2_t b)	{
	b[0] = a[0];
	b[1] = a[1];
}

// subtracts vec3_t b from vec3_t a
inline void VectorSubtract(const vec3_t a, const vec3_t b, vec3_t result)	{
	result[0] = a[0] - b[0];
	result[1] = a[1] - b[1];
	result[2] = a[2] - b[2];
}

inline void VectorSubtract2f(const vec2_t a, const vec2_t b, vec2_t result)	{
	result[0] = a[0] - b[0];
	result[1] = a[1] - b[1];
}

inline void VectorAdd(const vec3_t a, const vec3_t b, vec3_t result)	{
	result[0] = a[0] + b[0];
	result[1] = a[1] + b[1];
	result[2] = a[2] + b[2];
}

inline void VectorScale(const vec3_t a, const float scale, vec3_t result)	{
	result[0] = a[0] * scale;
	result[1] = a[1] * scale;
	result[2] = a[2] * scale;
}

inline void VectorScale2f(const vec2_t a, const float scale, vec2_t result)	{
	result[0] = a[0] * scale;
	result[1] = a[1] * scale;
}

// Doing division likes this gives us a roughly 100% speed increase over
// the standard method of dividing each value by the divisor.
inline void VectorDivide(const vec3_t a, const float divisor, vec3_t result)	{
	float fractValue = 1 / divisor;
	result[0] = a[0] * fractValue;
	result[1] = a[1] * fractValue;
	result[2] = a[2] * fractValue;
}

// multiplies b * scale, then adds the new vec to a
inline void VectorMA(const vec3_t a, const vec3_t b, const float scale, vec3_t result)	{
	result[0] = a[0] + (b[0] * scale);
	result[1] = a[1] + (b[1] * scale);
	result[2] = a[2] + (b[2] * scale);
}

inline void VectorMA2f(const vec2_t a, const vec2_t b, const float scale, vec2_t result)	{
	result[0] = a[0] + (b[0] * scale);
	result[1] = a[1] + (b[1] * scale);
}

inline float VectorLength(const vec3_t a)	{
//	return sqrtf( ((a[0]*a[0]) + (a[1]*a[1]) + (a[2]*a[2])) );
	return FastSqrt( ((a[0]*a[0]) + (a[1]*a[1]) + (a[2]*a[2])) );
}


inline float VectorUnitVector(const vec3_t a, vec3_t result)	{
	float length = VectorLength(a);
	VectorDivide(a, length, result);
	return length;
}

inline void VectorNegate(const vec3_t a, vec3_t result)	{
	result[0] = -a[0];
	result[1] = -a[1];
	result[2] = -a[2];
}

inline float DotProduct(const vec3_t a, const vec3_t b)	{
	return ( (a[0] * b[0]) + (a[1] * b[1]) + (a[2] * b[2]) );
}

inline void CrossProduct(const vec3_t a, const vec3_t b, vec3_t result)	{
	result[0] = (a[1] * b[2]) - (a[2] * b[1]);
	result[1] = (a[2] * b[0]) - (a[0] * b[2]);
	result[2] = (a[0] * b[1]) - (a[1] * b[0]);
}

inline float VectorDistance(const vec3_t a, const vec3_t b)	{
	int x = b[0]-a[0];
	int y = b[1]-a[1];
	int z = b[2]-a[2];

	return FastSqrt( (x*x)+(y*y)+(z*z) );
}

// End Vector Functions

//////////////// BEGIN POLY SUPPORT FUNCS ////////////////
// NOTICE: TO CREATE A NEW POLYGON YOU SHOULD USE
// THE createPolygon() FUNCTION.
typedef struct polygon_s	{
	unsigned int polyID;
	int numPoints;
	vec3_t points[MAX_POLY_POINTS];

	bool isTextured;
	vec2_t texpts[MAX_POLY_POINTS];

	bool hasNormals;
	vec3_t normpts[MAX_POLY_POINTS];

	bool glCached;
	unsigned int glCacheID;

	bool hasMaterial;
	char materialName[MAX_FILE_LENGTH];

	bool selected;
	vec3_t polygonDrawColor;

	struct polygon_s* prev;
	struct polygon_s* next;
}polygon_t;

// This should be used anytime we want to create a new polygon
inline polygon_t* createPolygon()	{
	polygon_t* poly = new polygon_t;

	// any initialization should be done here
	poly->polyID = 0;
	poly->numPoints = 0;
	poly->isTextured = false;
	poly->hasNormals = false;
	poly->glCached = false;

	for(int x=0; x < MAX_POLY_POINTS; x++)	{
		VectorCopy(ZERO_VECTOR, poly->points[x]);
		VectorCopy(ZERO_VECTOR, poly->texpts[x]);
		VectorCopy(ZERO_VECTOR, poly->normpts[x]);
	}

	poly->hasMaterial = false;
	poly->materialName[0] = '\0';

	poly->selected = false;

	VectorCopy(ZERO_VECTOR, poly->polygonDrawColor);

	poly->prev = NULL;
	poly->next = NULL;

	return poly;
}

inline polygon_t* copyPolygon(const polygon_t* p)	{
	polygon_t* poly = new polygon_t;

	// any initialization should be done here
	poly->polyID = p->polyID;
	poly->numPoints = p->numPoints;
	poly->isTextured = p->isTextured;
	poly->hasNormals = p->hasNormals;
	poly->glCached = p->glCached;

	for(int x=0; x < p->numPoints; x++)	{
		VectorCopy(p->points[x], poly->points[x]);
		VectorCopy(p->texpts[x], poly->texpts[x]);
		VectorCopy(p->normpts[x], poly->normpts[x]);
	}

	poly->hasMaterial = p->hasMaterial;
	strncpy(poly->materialName, p->materialName, MAX_FILE_LENGTH);

	poly->selected = p->selected;

	VectorCopy(p->polygonDrawColor, poly->polygonDrawColor);

	poly->prev = NULL;
	poly->next = NULL;

	return poly;
}

// Prepends new polygon to linked list pointed to by first
// returns new front of list
inline polygon_t* doublyLinkPolygons(polygon_t* first, polygon_t* newPoly)	{
	if( first != NULL )	{
		first->prev = newPoly;
		newPoly->prev = NULL;
		newPoly->next = first;
	}
	else	{
		newPoly->prev = NULL;
		newPoly->next = NULL;
	}


	return newPoly;
}

// appends entity to linked list
// returns front of list
inline polygon_t* appendPolygonToLinkedList(polygon_t* list, polygon_t* poly)	{

	if( poly == NULL )
		return NULL;

	if( list != NULL )	{
		polygon_t* cur = list;

		while( cur->next )	// goto end of ll
			cur = cur->next;

		cur->next = poly;
		poly->prev = cur;
		poly->next = NULL;

		return list;
	}
	else	{
		poly->prev = NULL;
		poly->next = NULL;

		return poly;
	}

}


// returns new front of list
// returns p when p is the front of the list and no other items
// returns NULL on error
inline polygon_t* unlinkPolygon(polygon_t* list, polygon_t* p)	{

	polygon_t* curPoly = list;

	while(curPoly != NULL)	{
		if( curPoly->polyID == p->polyID )	{
			polygon_t* newNextPoly = p->next;
			polygon_t* newPrevPoly = p->prev;

			if( newNextPoly != NULL && newPrevPoly != NULL )	{
				newNextPoly->prev = newPrevPoly;
				newPrevPoly->next = newNextPoly;
				return list;
			}
			else if( newNextPoly == NULL && newPrevPoly == NULL )	{	// They're both NULL!
				return p;	// no other items in list, return p to indicate that it was front
			}
			else if( newNextPoly == NULL )	{	// item was at end of list and has prev
				newPrevPoly->next = NULL;

				return list;
			}
			else if( newPrevPoly == NULL )	{	// item was at front of list and has a next
				newNextPoly->prev = NULL;
				return newNextPoly;	// new front of list
			}
		}

		curPoly = curPoly->next;
	}

	return NULL;	// polygon not found in list
}


///////////////// END POLY SUPPORT FUNCS ///////////////////


// Begin Geometric Functions
inline bool isPointInPolygon(polygon_t* poly, vec3_t point)	{
	vec3_t p;
	vec3_t a;
	vec3_t b;
	VectorSubtract(point, poly->points[poly->numPoints-1], a);
	VectorSubtract(point, poly->points[0], b);
	CrossProduct(a, b, p);

	for(int i = 0; i < poly->numPoints - 1; i++)	{
		vec3_t q;
		vec3_t c;
		vec3_t d;

		VectorSubtract(point, poly->points[i], c);
		VectorSubtract(point, poly->points[i+1], d);
		CrossProduct(c, d, q);

		if( DotProduct(p, q) < 0 )
			return false;
	}

	return true;
}

// Which side of the plane is this point on?
inline float classifyPoint(const plane_t *plane, const vec3_t point)	{
	return DotProduct(plane->normal, point) - DotProduct(plane->normal, plane->origin);
}

// Which side of the plane is this polygon on? (relies on classifyPoint())
inline int classifyPolygon(const plane_t* partition, const polygon_t* poly)	{
	int x;
	bool hasFront = false;
	bool hasBack = false;


	for(x=0; x < poly->numPoints; x++)	{
		float classification = classifyPoint(partition, poly->points[x]);
		// we can do the returns below because if any point on the
		// polygon is on the opposite side there will be a split
		// we don't care to check every point, the splitting routines
		// will do that for us later.
		if( classification  >= 0 )	{
			hasFront = true;

			if( hasBack )
				return SPANNING;
		}
		else	{ // if( classification < 0 )
			hasBack = true;

			if( hasFront )
				return SPANNING;
		}
	}

	if( hasFront )
		return FRONT;

	if( hasBack )
		return BACK;

	return -99;	// Error of sorts happened.
}

/*
int TestIntersionPlane(plane_t* plane, const vec3_t position, const TVector& direction, double& lamda, TVector& pNormal)
{

    double DotProduct=direction.dot(plane._Normal);
	double l2;

    //determine if ray paralle to plane
    if ((DotProduct<ZERO)&&(DotProduct>-ZERO))
		return 0;

    l2=(plane._Normal.dot(plane._Position-position))/DotProduct;

    if (l2<-ZERO)
		return 0;

    pNormal=plane._Normal;
	lamda=l2;
    return 1;

}
*/



// For all intensive purposes this is a ray-plane intersection
inline int findLinePlaneIntersect(const plane_t *plane, const vec3_t pointA, const vec3_t pointB, vec3_t intersect, float *fractSect)	{
	vec3_t u;
	vec3_t w;

	VectorSubtract(pointB, pointA, u);
	VectorSubtract(pointA, plane->origin, w);

	float numerator = -DotProduct(plane->normal, w);
	float denominator = DotProduct(plane->normal, u);

	if( fabs(denominator) < EPSILON ) {          // segment is parallel to plane
		if (numerator == 0)	// segment is coincident with plane
			return 2;	// no intersection
		else
			return 0;	// no intersection
	}

	// at this point they are not parallel
	// compute intersect parameter
	(*fractSect) = numerator / denominator;

	if( (*fractSect) < 0 || (*fractSect) > 1 )
		return 0;	// no intersection

	VectorMA( pointA, u, (*fractSect), intersect);

	return 1;	// Indicate that we had an intersection
}

/*
// Returns reflection vector (of unit length) in result from given inputs
inline void VectorReflect(const vec3_t incident, const vec3_t surfNorm, vec3_t result)	{
	vec3_t r;
	VectorScale(surfNorm, 2, r);
	float f = DotProduct(incident, surfNorm);
	VectorScale(r, f, result);
	VectorSubtract(incident, result, result);
}
*/

// Returns reflection vector (of unit length) in result from given inputs
inline void VectorReflect(const vec3_t incident, const vec3_t surfNorm, vec3_t result)  {

        vec3_t neg;

        VectorNegate(incident, neg);
        double iA = DotProduct(neg, surfNorm);        // incident angle

        iA *= 2;

        VectorMA(incident, surfNorm, iA, result);

}






#endif /* SHARED_H_ */
