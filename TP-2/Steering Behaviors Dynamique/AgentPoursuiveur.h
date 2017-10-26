#ifndef AGENTPOURSUIVEUR_H
#define AGENTPOURSUIVEUR_H
#pragma warning (disable:4786)
//------------------------------------------------------------------------
//
//  Name:   AgentPoursuiveur.h
//
//  Desc:   Definition of a pursuer vehicle that follow an other vehicle using steering behaviors
//
//  Author: Renald Morice & Paul Michaud 
//
//------------------------------------------------------------------------
#include "Vehicle.h"

class AgentLeader;

class AgentPoursuiveur : public Vehicle{
private:
	//Which vehicle this object has to follow (his queue's predecessor)
	Vehicle* m_followedVehicle;

	//Following offset
	Vector2D m_offset;

	//Agent leading the queue
	AgentLeader* m_leaderOfMyQueue;


public:
	AgentPoursuiveur(GameWorld* world,
		Vector2D position,
		double    rotation,
		Vector2D velocity,
		double    mass,
		double    max_force,
		double    max_speed,
		double    max_turn_rate,
		double    scale,
		int vehicule_type);

	//destructor
	~AgentPoursuiveur();

	// Follow a leader with an offset
	void follow(AgentLeader* const, Vehicle* const, Vector2D);

	// Stop following a vehicle
	void stopFollowing();

	//accessors
	Vehicle* getFollowedVehicle()const { return m_followedVehicle; }
	Vector2D  getOffset()const { return m_offset; }
	AgentLeader* getLeaderOfMyQueue()const { return m_leaderOfMyQueue; }

};



#endif