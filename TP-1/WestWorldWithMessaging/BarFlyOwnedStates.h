#ifndef BARFLY_OWNED_STATES_H
#define BARFLY_OWNED_STATES_H
//------------------------------------------------------------------------
//
//  Name:   BarFlyOwnedStates.h
//
//  Desc:   All the states that can be assigned to the BarFly class.
//          Note that a global state has not been implemented.
//
//  Author: Morice Renald and Michaud Paul 2017
//
//------------------------------------------------------------------------
#include "fsm/State.h"


class BarFly;
struct Telegram;




//------------------------------------------------------------------------
//
//  In this state the barfly will peacefuly drink some beers. 
//------------------------------------------------------------------------
class CalmAndDrinking : public State<BarFly>
{
private:

	CalmAndDrinking() {}

	//copy ctor and assignment should be private
	CalmAndDrinking(const CalmAndDrinking&);
	CalmAndDrinking& operator=(const CalmAndDrinking&);

public:

	//this is a singleton
	static CalmAndDrinking* Instance();

	virtual void Enter(BarFly* barfly);

	virtual void Execute(BarFly* barfly);

	virtual void Exit(BarFly* barfly);

	virtual bool OnMessage(BarFly* agent, const Telegram& msg);

};

//------------------------------------------------------------------------
//
//  In this state the barfly will send aggresives messages to Bob, if Bob
//  enters the saloon they will fight, if he does not come he'll continue 
//  drinking.
//------------------------------------------------------------------------
class FightMinerBob : public State<BarFly>
{
private:

	FightMinerBob() {}

	//copy ctor and assignment should be private
	FightMinerBob(const FightMinerBob&);
	FightMinerBob& operator=(const FightMinerBob&);

public:

	//this is a singleton
	static FightMinerBob* Instance();

	virtual void Enter(BarFly* barfly);

	virtual void Execute(BarFly* barfly);

	virtual void Exit(BarFly* barfly);

	virtual bool OnMessage(BarFly* agent, const Telegram& msg);

};

//------------------------------------------------------------------------
//
//  The barfly will sleep in the saloon until his fatigue is decreased
//  sufficiently 
//------------------------------------------------------------------------
class Sleeping : public State<BarFly>
{
private:

	Sleeping() {}

	//copy ctor and assignment should be private
	Sleeping(const Sleeping&);
	Sleeping& operator=(const Sleeping&);

public:

	//this is a singleton
	static Sleeping* Instance();

	virtual void Enter(BarFly* barfly);

	virtual void Execute(BarFly* barfly);

	virtual void Exit(BarFly* barfly);

	virtual bool OnMessage(BarFly* agent, const Telegram& msg);

};

#endif