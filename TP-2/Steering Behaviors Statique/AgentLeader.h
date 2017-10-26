#ifndef AGENTLEADER_H
#define AGENTLEADER_H
#pragma warning (disable:4786)
//------------------------------------------------------------------------
//
//  Name:   AgentLeader.h
//
//  Desc:   Definition of a leader vehicle
//
//  Author: Renald Morice & Paul Michaud 
//
//------------------------------------------------------------------------
#include "vehicle.h"
#include "AgentPoursuiveur.h"
#include "2d/Vector2D.h"

#define SIMPLE_QUEUE 1
#define FLOCKING_V_QUEUE 2

class AgentLeader : public Vehicle{
private:
	int m_following_type;

	//All pursuers which are following this leader
	std::vector<AgentPoursuiveur*> m_vAgentsPoursuiveurs;

	//Initialize funtion's parameters with the context
	//(vehicle to follow and offset) for the new pursuer
	void getContextOfNewPursuer(Vehicle**, Vector2D**);

	//Set the context of a pursuer add the the queue of the leader
	void setContextOfPursuer(AgentPoursuiveur*);

public:
	AgentLeader(
		GameWorld * world, 
		Vector2D position, 
		double rotation, 
		Vector2D velocity, 
		double mass, 
		double max_force, 
		double max_speed, 
		double max_turn_rate, 
		double scale,
		int vehicule_type,
		int following_type);

	//Add a pursuer to this leader
	void addAgentPoursuiveur(AgentPoursuiveur*);

	//Remove a pursuer to this leader
	void removeAgentPoursuiveur(AgentPoursuiveur*);

	int getNumberOfPursuer() { return m_vAgentsPoursuiveurs.size(); }

	~AgentLeader();
};


#endif