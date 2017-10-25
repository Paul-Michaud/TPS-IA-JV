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
		vehicule_type),
	m_followedVehicle(NULL)
{

	this->Steering()->SeparationOn();
	this->Steering()->FlockingOn();

}


//------------------------------ follow ----------------------------------
//
// Follow a vehicle with an offset
//------------------------------------------------------------------------
void AgentPoursuiveur::follow(Vehicle* const leader, Vector2D offset) {
	m_followedVehicle = leader;
	m_offset = offset;
	this->Steering()->OffsetPursuitOn(leader, offset);
}

//------------------------------ follow ----------------------------------
//
// Stop following a vehicle
//------------------------------------------------------------------------
void AgentPoursuiveur::stopFollowing() {
	m_followedVehicle = NULL;
	m_offset = Vector2D(0,0);
	this->Steering()->OffsetPursuitOff();
}


//------------------------------ Destructor ------------------------------
//
//------------------------------------------------------------------------
AgentPoursuiveur::~AgentPoursuiveur() {

}