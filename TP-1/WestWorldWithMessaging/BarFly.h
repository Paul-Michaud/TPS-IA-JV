#ifndef BARFLY_H
#define BARFLY_H
//------------------------------------------------------------------------
//
//  Name:   BarFly.h
//
//  Desc:   A class defining a bar fly.
//
//  Author: Morice Renald and Michaud Paul 2017
//
//------------------------------------------------------------------------
#include <string>
#include <cassert>
#include <iostream>

#include "BaseGameEntity.h"
#include "Locations.h"
#include "misc/ConsoleUtils.h"
#include "BarFLyOwnedStates.h"
#include "fsm/StateMachine.h"

template <class entity_type> class State; //pre-fixed with "template <class entity_type> " for vs8 compatibility

struct Telegram;

//the amount of drinks the barfly must have before he gets aggressive
const int DrinksAggressive = 3;

//the amount of drinks the barfly must have before he sleeps (If he didn't fight)
const int DrinksSleep = 9;

//The amount of time the barfly has to sleep
const int maxFatigue = 5;

class BarFly : public BaseGameEntity
{
private:

	//an instance of the state machine class
	StateMachine<BarFly>*  m_pStateMachine;

	location_type         m_Location;

	//how many drinks the barfly has drunk
	int                   m_iDrinks;

	//the variable to know when the barfly can wake up
	int                   m_iFatigue;

public:

	BarFly(int id, std::mutex* mtx_locker):
		m_Location(saloon),
		m_iDrinks(0),
		m_iFatigue(0),
		BaseGameEntity(id, mtx_locker){

		//set up state machine
		m_pStateMachine = new StateMachine<BarFly>(this);
		m_pStateMachine->SetCurrentState(CalmAndDrinking::Instance());

		this->setColorText(FOREGROUND_GREEN | FOREGROUND_BLUE);
	}

	~BarFly() { delete m_pStateMachine; }

	//this must be implemented
	void Update();

	//so must this
	virtual bool  HandleMessage(const Telegram& msg);


	StateMachine<BarFly>* GetFSM()const { return m_pStateMachine; }



	//-------------------------------------------------------------accessors
	location_type Location()const { return m_Location; }
	void          ChangeLocation(location_type loc) { m_Location = loc; }

	int           GetDrinks()const { return m_iDrinks; }
	void          SetDrinks(int val) { m_iDrinks = val; }
	void          BuyAndDrinkABeer() { m_iDrinks += 1; }

	void          SetFatigue() { m_iFatigue = maxFatigue; }
	int           GetFatigue()const { return m_iFatigue; }
	void          DecreaseFatigue() { m_iFatigue -= 1; }

	bool          isAggressive()const { return m_iDrinks > DrinksAggressive;  }
	bool          isTired()const { return m_iDrinks > DrinksSleep;  }
};


#endif
