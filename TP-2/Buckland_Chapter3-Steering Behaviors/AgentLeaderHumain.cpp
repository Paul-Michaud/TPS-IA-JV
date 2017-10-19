#include "AgentLeaderHumain.h"
#include "SteeringBehaviors.h"

AgentLeaderHumain::AgentLeaderHumain(GameWorld* world,
		Vector2D position,
		double    rotation,
		Vector2D velocity,
		double    mass,
		double    max_force,
		double    max_speed,
		double    max_turn_rate,
		double    scale) : AgentLeader(
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
	this->Steering()->WanderOff();
	//this->SetVelocity(Vector2D(0, max_speed));
	
	
}

void AgentLeaderHumain::turnLeft() {

}

void AgentLeaderHumain::turnRight() {

}

void AgentLeaderHumain::accelerate() {

}

void AgentLeaderHumain::decelerate() {

}
AgentLeaderHumain::~AgentLeaderHumain(){
}
