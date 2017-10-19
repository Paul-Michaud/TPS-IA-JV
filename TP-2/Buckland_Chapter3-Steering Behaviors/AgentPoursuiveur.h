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
#include "vehicle.h"


class AgentPoursuiveur : public Vehicle
{
public :
	AgentPoursuiveur(GameWorld* world,
		Vector2D position,
		double    rotation,
		Vector2D velocity,
		double    mass,
		double    max_force,
		double    max_speed,
		double    max_turn_rate,
		double    scale,
		Vehicle*	  leaderLocal);

	~AgentPoursuiveur();

};



#endif