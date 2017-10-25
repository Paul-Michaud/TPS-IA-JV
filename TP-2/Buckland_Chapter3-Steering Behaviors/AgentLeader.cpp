#include "AgentLeader.h"
#include "SteeringBehaviors.h"

//------------------------------ ctor ----------------------------------
//
//------------------------------------------------------------------------

AgentLeader::AgentLeader(GameWorld* world,
	Vector2D  position,
	double    rotation,
	Vector2D  velocity,
	double    mass,
	double    max_force,
	double    max_speed,
	double    max_turn_rate,
	double    scale,
	int		  vehicule_type,
	int       following_type) : Vehicle (
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
	m_following_type(following_type)
{
	this->Steering()->WanderOn();
	this->SetScale(Vector2D(10,10));
	this->SetMaxSpeed(70);
}

//------------------------------ addAgentPoursuiveur ---------------------
//
//  Add a pursuer to this leader
//------------------------------------------------------------------------
void AgentLeader::addAgentPoursuiveur(AgentPoursuiveur* agentPoursuiveur) {

	Vehicle* leaderOfNewPursuer;
	Vector2D* offsetOfNewPursuer;
	getContextOfNewPursuer(&leaderOfNewPursuer, &offsetOfNewPursuer);

	agentPoursuiveur->follow(leaderOfNewPursuer, *offsetOfNewPursuer);

	this->m_vAgentsPoursuiveurs.push_back(agentPoursuiveur);

}

//------------------------------ getContextOfNewPursuer -----------------
//
//  Initialize funtion's parameters with the context
//  (vehicle to follow and offset) for the new pursuer
//-----------------------------------------------------------------------
void AgentLeader::getContextOfNewPursuer(Vehicle** leaderOfNewPursuer, Vector2D** offsetOfNewPursuer) {

	switch (m_following_type) {
		case SIMPLE_QUEUE: //-------------------
			if (this->m_vAgentsPoursuiveurs.size() == 0) *leaderOfNewPursuer = (Vehicle*)this;
			else *leaderOfNewPursuer = (Vehicle*)this->m_vAgentsPoursuiveurs.back();

			(*offsetOfNewPursuer) = new Vector2D(-5.0, 0);

			break;

		case FLOCKING_V_QUEUE:
			if (this->m_vAgentsPoursuiveurs.size() == 0 || this->m_vAgentsPoursuiveurs.size() == 1) *leaderOfNewPursuer = (Vehicle*)this;
			else *leaderOfNewPursuer = (Vehicle*) this->m_vAgentsPoursuiveurs.at(this->m_vAgentsPoursuiveurs.size() - 2);

			if (this->m_vAgentsPoursuiveurs.size() % 2 == 0) (*offsetOfNewPursuer) = new Vector2D(0.0, -10.0);
			else (*offsetOfNewPursuer) = new Vector2D(0.0, 10.0);
			break;

		default:
			break;
	}
	
}


//------------------------------ Destructor ------------------------------
//
//------------------------------------------------------------------------
AgentLeader::~AgentLeader() {
	for (size_t i = 0; i < m_vAgentsPoursuiveurs.size(); i++) m_vAgentsPoursuiveurs[i]->stopFollowing();
	
}
