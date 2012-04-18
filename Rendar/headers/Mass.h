#ifndef MASS_H_
#define MASS_H_


#include <iostream>
#include "shared.h"

class Mass	{
public:
	int moveType;
	float m;					// The mass value
	float rotationSpeed;		// In RPM?
	float instantSpeed;			// instantaneousSpeed

	vec3_t prevPos;				// previous position
	vec3_t pos;					// Position in space
	vec3_t vel;					// Velocity
	vec3_t force;				// Force applied on this mass at an instance
	vec3_t rotationAxis;		// Axis of rotation


	Mass(float m)	{
		this->m = m;
	}

	void applyForce(vec3_t force)	{
		VectorAdd(this->force, force, this->force);
	}

	void init()	{
		force[0] = 0;
		force[1] = 0;
		force[2] = 0;
	}

	void simulate(float dt)	{
		vec3_t velocityDelta;

		VectorDivide(force, m, velocityDelta);
		VectorMA(vel, velocityDelta, dt, vel);

		VectorCopy(pos, prevPos);		// save old position

		VectorMA(pos, vel, dt, pos);	// get new one!
	}

};


#endif

