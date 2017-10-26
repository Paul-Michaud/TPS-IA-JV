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
//  Add a pursuer to this leader, it unfollows his precedent leader
//	then we add it to this leaders's queue and set his offset and 
//	the entity he has to follow with setContextOfPursuer
//------------------------------------------------------------------------
void AgentLeader::addAgentPoursuiveur(AgentPoursuiveur* agentPoursuiveur) {

	agentPoursuiveur->stopFollowing();

	this->m_vAgentsPoursuiveurs.push_back(agentPoursuiveur);

	setContextOfPursuer(agentPoursuiveur);

}

//------------------------------ removeAgentPoursuiveur ---------------------
//
//  remove a pursuer from this leader, we remove it from the queue, and we 
//	update positions from all pursuers that were behind him
//---------------------------------------------------------------------------
void AgentLeader::removeAgentPoursuiveur(AgentPoursuiveur* agentPoursuiveur) {
	unsigned int i;

	//Delete the pursuer from the leader's queue
	for (i = 0; i < m_vAgentsPoursuiveurs.size(); i++) {
		if (m_vAgentsPoursuiveurs[i] == agentPoursuiveur) {
			m_vAgentsPoursuiveurs.erase(m_vAgentsPoursuiveurs.begin() + i);
			break;
		}
	}

	//Update the pursuers that are after this one in the leader's queue
	for (unsigned int j = i; j < m_vAgentsPoursuiveurs.size(); j++) {
		setContextOfPursuer(m_vAgentsPoursuiveurs[j]);
	}
}

//------------------------------ setContextOfPursuer -----------------
//
//  Initialize a pursuer's leader & offset considering his position 
//	in the queue, and the type of queue
//--------------------------------------------------------------------
void AgentLeader::setContextOfPursuer(AgentPoursuiveur* pursuer) {
	Vehicle* leaderOfNewPursuer;
	unsigned int i;
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
