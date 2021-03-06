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
	this->Steering()->WallAvoidanceOn();
}

//------------------------------ addAgentPoursuiveur ---------------------
//
//  Add a pursuer to this leader
//------------------------------------------------------------------------
void AgentLeader::addAgentPoursuiveur(AgentPoursuiveur* agentPoursuiveur) {

	//Vehicle* leaderOfNewPursuer;
	//Vector2D* offsetOfNewPursuer;
	//getContextOfNewPursuer(&leaderOfNewPursuer, &offsetOfNewPursuer);

	//agentPoursuiveur->follow(leaderOfNewPursuer, *offsetOfNewPursuer);

	//this->m_vAgentsPoursuiveurs.push_back(agentPoursuiveur);

	this->m_vAgentsPoursuiveurs.push_back(agentPoursuiveur);

	setContextOfPursuer(agentPoursuiveur);

}

//------------------------------ removeAgentPoursuiveur ---------------------
//
//  remove a pursuer to this leader
//------------------------------------------------------------------------
void AgentLeader::removeAgentPoursuiveur(AgentPoursuiveur* agentPoursuiveur) {
	size_t i;

	//Delete the pursuer from the leader's queue
	for (i = m_vAgentsPoursuiveurs.size() - 1; i < m_vAgentsPoursuiveurs.size(); i++) {
		if (m_vAgentsPoursuiveurs[i] == agentPoursuiveur) {
			m_vAgentsPoursuiveurs.erase(m_vAgentsPoursuiveurs.begin() + i);
			break;
		}
	}
	//Update the pursuers that are after this one in the leader's queue
	for (size_t j = i; j < m_vAgentsPoursuiveurs.size(); j++) {
		setContextOfPursuer(m_vAgentsPoursuiveurs[j]);
	}
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

//------------------------------ setContextOfPursuer -----------------
//
//  Initialize a pursuer's leader & offset considering his position 
//	in the leader's queue
//-----------------------------------------------------------------------
void AgentLeader::setContextOfPursuer(AgentPoursuiveur* pursuer) {
	Vehicle* leaderOfNewPursuer;
	size_t i;
	Vector2D offsetOfNewPursuer;

	//Get the position of the added pursuer
	for (i = 0; i < m_vAgentsPoursuiveurs.size(); i++) {
		if (m_vAgentsPoursuiveurs[i] == pursuer) {
			break;
		}
	}

	switch (m_following_type) {
		case SIMPLE_QUEUE:

			if (i == 0) leaderOfNewPursuer = (Vehicle*)this;
			else leaderOfNewPursuer = (Vehicle*)this->m_vAgentsPoursuiveurs[i - 1];
			offsetOfNewPursuer = Vector2D(-5.0, 0);
			break;

		case FLOCKING_V_QUEUE:
			if (i == 0 || i == 1) leaderOfNewPursuer = (Vehicle*)this;
			else leaderOfNewPursuer = (Vehicle*)this->m_vAgentsPoursuiveurs.at(i - 2);

			if (i % 2 == 0) offsetOfNewPursuer = Vector2D(0.0, -10.0);
			else offsetOfNewPursuer = Vector2D(0.0, 10.0);
			break;

		default:
			break;
	}
	pursuer->follow(this, leaderOfNewPursuer, offsetOfNewPursuer);
}

//------------------------------ Destructor ------------------------------
//
//------------------------------------------------------------------------
AgentLeader::~AgentLeader() {
	for (size_t i = 0; i < m_vAgentsPoursuiveurs.size(); i++) m_vAgentsPoursuiveurs[i]->stopFollowing();
	
}
