#ifndef AGENTLEADERHUMAIN_H
#define AGENTLEADERHUMAIN_H
#pragma warning (disable:4786)
#include "AgentLeader.h"

class AgentLeaderHumain : public AgentLeader
{
public:
	AgentLeaderHumain(GameWorld * world,
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

	//Moving functions
	void turnRight();
	void turnLeft();
	void accelerate();
	void decelerate();

	~AgentLeaderHumain();

};

#endif
