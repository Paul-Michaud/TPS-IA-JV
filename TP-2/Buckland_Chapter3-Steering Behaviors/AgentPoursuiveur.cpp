#include "AgentPoursuiveur.h"
#include "SteeringBehaviors.h"


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
	Vehicle* leaderLocal) : Vehicle(
		world,
		position,
		rotation,
		velocity,
		mass,
		max_force,
		max_speed,
		max_turn_rate,
		scale)
	
{
	Vector2D PoursuiveurOffset((double)-5, (double)0);
	this->Steering()->OffsetPursuitOn(leaderLocal, PoursuiveurOffset);
	this->Steering()->SeparationOn();
}


AgentPoursuiveur::~AgentPoursuiveur() {
}
