#include "AgentLeader.h"
#include "SteeringBehaviors.h"


//------------------------------ ctor ----------------------------------
//
//------------------------------------------------------------------------

AgentLeader::AgentLeader(GameWorld* world,
	Vector2D position,
	double    rotation,
	Vector2D velocity,
	double    mass,
	double    max_force,
	double    max_speed,
	double    max_turn_rate,
	double    scale) : Vehicle (
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
	this->Steering()->WanderOn();
	this->SetScale(Vector2D(10,10));
	this->SetMaxSpeed(70);

}



AgentLeader::~AgentLeader() {
}
