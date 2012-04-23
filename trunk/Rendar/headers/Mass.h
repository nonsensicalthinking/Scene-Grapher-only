#ifndef MASS_H_
#define MASS_H_


#include <iostream>
#include "shared.h"

#define 	MOVETYPE_STATIONARY		0
#define 	MOVETYPE_GRAVITY		1

const vec3_t EARTH_GRAV = {0.0, -9.8, 0.0};

class Mass	{
public:
	int moveType;
	float m;					// The mass value
	float rotationSpeed;		// In RPM?
	float instantSpeed;			// instantaneousSpeed

	vec3_t prevPos;				// previous position
	vec3_t pos;					// Position in space
	vec3_t facing;				// direction we're facing
	vec3_t up;					// up direction
	vec3_t vel;					// Velocity
	vec3_t force;				// Force applied on this mass at an instance
	vec3_t rotationAxis;		// Axis of rotation

	vec3_t xyzRadii;

	Mass(float mass)	{
		moveType = MOVETYPE_STATIONARY;
		m = mass;
		rotationSpeed = 0;
		instantSpeed = 0;
		VectorCopy(ZERO_VECTOR, prevPos);
		VectorCopy(ZERO_VECTOR, pos);
		VectorCopy(ZERO_VECTOR, vel);
		VectorCopy(ZERO_VECTOR, force);
		VectorCopy(ZERO_VECTOR, rotationAxis);
	}

	virtual ~Mass()	{

	}

	void applyForce(vec3_t force)	{
		VectorAdd(this->force, force, this->force);
	}

	void init()	{
		force[0] = 0;
		force[1] = 0;
		force[2] = 0;
	}

	virtual void solve()	{

	}

	void simulate(float dt)	{
		// this switch statement stands in as an if statement
		switch( moveType )	{
		case MOVETYPE_STATIONARY:
			break;
		default:
			vec3_t velocityDelta;

			VectorDivide(force, m, velocityDelta);
			VectorMA(vel, velocityDelta, dt, vel);

			VectorCopy(pos, prevPos);		// save old position

			VectorMA(pos, vel, dt, pos);	// get new one!
			break;
		}
	}

	void operate(float dt)	{
		init();
		solve();
		simulate(dt);
	}

	void makeAABB(const vec3_t xyzRad)	{
		VectorCopy(xyzRad, xyzRadii);
	}

};


class Gravity : public Mass	{
public:
	vec3_t gravitation;

	Gravity(float m) : Mass(m)	{
		gravitation[0] = 0.0;
		gravitation[1] = -9.8;
		gravitation[2] = 0.0;
		moveType = MOVETYPE_GRAVITY;
	}

	void solve()	{
		vec3_t gravity;
		VectorScale(gravitation, m, gravity);
		applyForce(gravity);
	}

};


class Helicopter : public Mass	{
public:
	double power;
	vec3_t gravitation;
	double area;

	Helicopter(float m) : Mass(m)	{
		moveType = MOVETYPE_GRAVITY;
		area = 1809.5;
		power = 50.0;
		VectorCopy(EARTH_GRAV, gravitation);
	}


	void solve()	{
		// gravity
		vec3_t gravity;
		VectorScale(gravitation, m, gravity);
		applyForce(gravity);

		// a more sophisticated thrust calc
		// power must be in hp

		float pl;
		if( power == 0 )
			pl = 0.0001 / area;
		else
			pl = power / area;

		float tl = 8.6859f * pow(pl, -0.3107f);
		float lift = tl * power;

		// rotor blade thrust
		vec3_t force;
		VectorScale(NORMAL_Y, lift, force);
		applyForce(force);
	}

};


class Baseball : public Mass	{
public:
	vec3_t gravitation;
	float sceneAdvRate;

	Baseball(const vec3_t grav, float advRate, float m) : Mass(m)	{																		//Vector3D gravitation, is the gravitational acceleration
		VectorCopy(grav, gravitation);
		sceneAdvRate = advRate;
	}

	// Returns angular speed in rad/s (?)
	float RPMtoAngularSpeed(float rpm)	{
		return (TWO_PI_DIV_BY_60 * rpm) * sceneAdvRate;
	}

	// this only does drag coefficient for baseballs
	// FIXME make this an index table instead
	float getDragCoeff(int speed)	{
		float drag = 0.0;

		if( speed > 100 )
			drag = 0.22;
		else if( speed > 90 )
			drag = 0.275;
		else if( speed > 80 )
			drag = 0.3;
		else if( speed > 70 )
			drag = 0.4;
		else if( speed > 60 )
			drag = 0.55;
		else if( speed < 60 )
			drag = 0.6;
		else
			drag = 0.6;

		return drag;
	}

	virtual float magnusDecay(float rate, float decayRate)	{
		return rate * decayRate;
	}

	virtual void solve(Mass* mass)	{
		// Do gravity
		vec3_t gravity;
		VectorScale(gravitation, mass->m, gravity);
		mass->applyForce(gravity);
		float dragCoeff = getDragCoeff(mass->instantSpeed);


		// Do Aerodynamic Drag
		vec3_t dragForce;
		VectorScale(mass->vel, dragCoeff, dragForce);
		VectorNegate(dragForce, dragForce);
		mass->applyForce(dragForce);

		// Do Magnus Force
		if( mass->rotationSpeed != 0 )	{
			vec3_t result;
			mass->instantSpeed = VectorLength(mass->vel);

// its a beautiful thing!
//			cout << "Speed: " << mass->instantSpeed << endl;

			CrossProduct(mass->rotationAxis, mass->vel, result);
			VectorScale(result, dragCoeff, result);


			float rotSpeedRad = RPMtoAngularSpeed(mass->rotationSpeed);
			// change for the next go around
			mass->rotationSpeed = magnusDecay(mass->rotationSpeed, 0.99);

			VectorScale(result, rotSpeedRad, result);

			mass->applyForce(result);
		}

	}

};



#endif

