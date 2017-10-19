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


class AgentLeader : public Vehicle
{
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
		double scale);

	~AgentLeader();
};



#endif
