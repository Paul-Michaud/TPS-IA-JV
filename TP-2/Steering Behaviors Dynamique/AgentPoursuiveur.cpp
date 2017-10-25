#include "AgentPoursuiveur.h"
#include "SteeringBehaviors.h"
#include "AgentLeader.h"

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
	m_followedVehicle(NULL),
	m_leaderOfMyQueue(NULL)
{

	this->Steering()->SeparationOn();
	this->Steering()->FlockingOn();
	this->Steering()->WallAvoidanceOn();
	
}


//------------------------------ follow ----------------------------------
//
// Follow a vehicle with an offset
//------------------------------------------------------------------------
void AgentPoursuiveur::follow(AgentLeader* const leaderOfMyQueue, Vehicle* const followedVehicle, Vector2D offset) {
	m_followedVehicle = followedVehicle;
	m_offset = offset;
	m_leaderOfMyQueue = leaderOfMyQueue;
	this->Steering()->OffsetPursuitOn(followedVehicle, offset);
}

//------------------------------ stopFollowing ----------------------------------
//
// Stop following a vehicle
//------------------------------------------------------------------------
void AgentPoursuiveur::stopFollowing() {
	if (m_leaderOfMyQueue) m_leaderOfMyQueue->removeAgentPoursuiveur(this);
	m_followedVehicle = NULL;
	m_leaderOfMyQueue = NULL;
	m_offset = Vector2D(0,0);
	this->Steering()->OffsetPursuitOff();
}

//------------------------------ Destructor ------------------------------
//
//------------------------------------------------------------------------
AgentPoursuiveur::~AgentPoursuiveur() {
	if(m_leaderOfMyQueue) m_leaderOfMyQueue->removeAgentPoursuiveur(this);
}



