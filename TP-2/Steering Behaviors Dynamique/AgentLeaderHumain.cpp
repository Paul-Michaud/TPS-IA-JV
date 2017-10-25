#include "AgentLeaderHumain.h"
#include "SteeringBehaviors.h"
#include "misc/cgdi.h"

AgentLeaderHumain::AgentLeaderHumain(GameWorld* world,
		Vector2D position,
		double    rotation,
		Vector2D velocity,
		double    mass,
		double    max_force,
		double    max_speed,
		double    max_turn_rate,
		double    scale,
		int		  vehicule_type,
	    int       following_type) : AgentLeader(
			world,
			position,
			rotation,
			velocity,
			mass,
			max_force,
			max_speed,
			max_turn_rate,
			scale,
			vehicule_type,
			following_type)
{
	this->Steering()->WanderOff();
}

void AgentLeaderHumain::turnLeft() {
	if (this->m_vVelocity != Vector2D(0,0)) // Turn only if we are in movement
		this->SetVelocity(this->m_vVelocity - this->Side() * this->MaxTurnRate());

}

void AgentLeaderHumain::turnRight() {
	if (this->m_vVelocity != Vector2D(0, 0)) // Turn only if we are in movement
		this->SetVelocity(this->m_vVelocity + this->Side() * this->MaxTurnRate());
}

void AgentLeaderHumain::accelerate() {
	if (this->m_vVelocity.y < this->MaxSpeed()) //Accelerate to maximum speed
		this->SetVelocity(this->m_vVelocity + this->Heading() * this->MaxSpeed()/15.0);
}

void AgentLeaderHumain::decelerate() {
		if(this->m_vVelocity.Length() > this->Heading().Length() * this->MaxSpeed() / 15.0)
			this->SetVelocity(this->m_vVelocity - this->Heading() * this->MaxSpeed() / 15.0);
		else this->SetVelocity(Vector2D(0, 0));
}

AgentLeaderHumain::~AgentLeaderHumain(){
}