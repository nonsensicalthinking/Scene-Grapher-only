/**************************************************************************

  File: physics.h
	Portions of the code in this file were derived from the works of:
	Erkin Tunca for http://nehe.gamedev.net/
 	More specifically the introduction to physical simulations lesson.
	Thanks for the lesson.


	// FIXME: This is probably one of the single most costly areas of the
	// engine.  Try to keep physics calculations to a minimum while still
	// obtaining "accurate enough" results.

**************************************************************************/


#ifndef PHYSICS_H_
#define PHYSICS_H_

#include <math.h>
#include "shared.h"
#include "Mass.h"


#define STATIONARY				0
#define MOBILE					1





// Abstract class Simulation
class Simulation	{
public:
	
	Simulation()	{
	}

	virtual ~Simulation()	{

	}

	virtual void release(Mass* mass)	{
		delete(mass);
		mass = NULL;
	}

	virtual void init(Mass* mass)	{
		mass->init();
	}

	virtual void solve(Mass* mass)	{
	}

	virtual void simulate(float dt, Mass* mass)	{
		mass->simulate(dt);
	}

	virtual void operate(float dt, Mass* mass)	{
		init(mass);
		solve(mass);
		simulate(dt, mass);
	}

};


// runge-kutta 4 **needs to be implemented**
class RK4Phys : public Simulation	{

public:
	RK4Phys()	{

	}

	virtual void solve(Mass* mass)	{


	    /*def rk4(x, v, a, dt):
	        """Returns final (position, velocity) tuple after
	        time dt has passed.

	        x: initial position (number-like object)
	        v: initial velocity (number-like object)
	        a: acceleration function a(x,v,dt) (must be callable)
	        dt: timestep (number)"""
	        x1 = x
	        v1 = v
	        a1 = a(x1, v1, 0)

	        x2 = x + 0.5*v1*dt
	        v2 = v + 0.5*a1*dt
	        a2 = a(x2, v2, dt/2.0)

	        x3 = x + 0.5*v2*dt
	        v3 = v + 0.5*a2*dt
	        a3 = a(x3, v3, dt/2.0)

	        x4 = x + v3*dt
	        v4 = v + a3*dt
	        a4 = a(x4, v4, dt)

	        xf = x + (dt/6.0)*(v1 + 2*v2 + 2*v3 + v4)
	        vf = v + (dt/6.0)*(a1 + 2*a2 + 2*a3 + a4)

	        return xf, vf*/
	}


};

// typical gravity simulation
class Gravity : public Simulation	{
public:
	vec3_t gravitation;

	Gravity(const vec3_t gravitation) : Simulation()	{
		VectorCopy(gravitation, this->gravitation);
	}

	virtual void solve(Mass* mass)	{
		vec3_t gravity;
		VectorScale(gravitation, mass->m, gravity);
		mass->applyForce(gravity);
	}

};


// Physics of a curve ball in flight
class BaseballPhysics : public Simulation	{
public:
	vec3_t gravitation;													//the gravitational acceleration
	float sceneAdvRate;

	BaseballPhysics(const vec3_t gravitation, float advRate) : Simulation()	{																		//Vector3D gravitation, is the gravitational acceleration
		VectorCopy(gravitation, this->gravitation);
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

class Stationary : public Simulation	{
	Stationary()	{

	}

	virtual void solve(Mass* mass)	{

	}
};

//  class ConstantVelocity is derived from class Simulation
//  It creates 1 mass with mass value 1 kg and sets its velocity to (1.0f, 0.0f, 0.0f)
//  so that the mass moves in the x direction with 1 m/s velocity.
/*
class ConstantVelocity : public Simulation	{
public:
	ConstantVelocity() : Simulation(1, 1.0f)	{
		vec3_t startPosition = {0.0f, 0.0f, 0.0f};
		vec3_t startVelocity = {1.0f, 0.0f, 0.0f};

		VectorCopy(startPosition, masses[0]->pos);
		VectorCopy(startVelocity, masses[0]->vel);
	}

};
*/



//	class MassConnectedWithSpring is derived from class Simulation
//	It creates 1 mass with mass value 1 kg and binds the mass to an arbitrary constant point with a spring.
//	This point is referred as the connectionPos and the spring has a springConstant value to represent its
//	stiffness.
/*
class MassConnectedWithSpring : public Simulation	{
public:
	float springConstant;
	vec3_t connectionPos;

	MassConnectedWithSpring(const float springConstant) : Simulation(1, 1.0f)	{
		this->springConstant = springConstant;

		vec3_t attachmentPosition = {0.0f, -5.0f, 0.0f};

		VectorCopy(attachmentPosition, connectionPos);

		vec3_t pullBackDistance = {10.0f, 0.0f, 0.0f};
		vec3_t startVelocity = {0.0, 0.0, 0.0};

		VectorAdd(connectionPos, pullBackDistance, masses[0]->pos);
		VectorCopy(startVelocity, masses[0]->vel);

	}

	virtual void solve()	{
		vec3_t springVector;
		vec3_t springForce;

		for (int a = 0; a < numOfMasses; ++a)	{
			VectorSubtract(masses[a]->pos, connectionPos, springVector);
			VectorNegate(springVector, springVector);
			VectorScale(springVector, springConstant, springForce);
			masses[a]->applyForce(springForce);
		}
	}
};
*/

#endif	// ENDIF PHYSICS_H_


