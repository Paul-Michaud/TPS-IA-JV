#include "AgentPoursuiveur.h"
#include "SteeringBehaviors.h"

// A SUPPR// A SUPPR// A SUPPR// A SUPPR
#include <iostream>
using namespace std;
// A SUPPR// A SUPPR// A SUPPR// A SUPPR

//------------------------------ ctor ----------------------------------
//
//------------------------------------------------------------------------

AgentPoursuiveur::AgentPoursuiveur(GameWorld* world,
	Vector2D position,
	double    rotation,
	Vector2D velocity,
	double    mass,
	double    max_force,
	double    max_speed,
	double    max_turn_rate,
	double    scale,
	Vehicle* leaderLocal,
	Vector2D pursuerOffset,
	int vehicule_type) : Vehicle(
		world,
		position,
		rotation,
		velocity,
		mass,
		max_force,
		max_speed,
		max_turn_rate,
		scale,
		vehicule_type)
{

	this->Steering()->OffsetPursuitOn(leaderLocal, pursuerOffset);
	this->Steering()->SeparationOn();
	this->Steering()->FlockingOn();

}



AgentPoursuiveur::~AgentPoursuiveur() {
}
