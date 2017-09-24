#include "BarFlyOwnedStates.h"
#include "fsm/State.h"
#include "BarFly.h"
#include "Locations.h"
#include "messaging/Telegram.h"
#include "MessageDispatcher.h"
#include "MessageTypes.h"
#include "Time/CrudeTimer.h"
#include "EntityNames.h"

#include <iostream>
using std::cout;


#ifdef TEXTOUTPUT
#include <fstream>
extern std::ofstream os;
#define cout os
#endif


//------------------------------------------------------------------------methods for CalmAndDrinking
CalmAndDrinking* CalmAndDrinking::Instance()
{
	static CalmAndDrinking instance;

	return &instance;
}


void CalmAndDrinking::Enter(BarFly* pBarFly)
{
	pBarFly->SetDrinks(0);

	//the barfly is always in the saloon
	cout << "\n" << GetNameOfEntity(pBarFly->ID()) << ": " << "Oh what a good day to drink some beers !";
}


void CalmAndDrinking::Execute(BarFly* pBarFly)
{
	//The barfly will drink some beers, if he drinks too much he will
	//change state to AggressiveAndDrinking


	pBarFly->BuyAndDrinkABeer();

	cout << "\n" << GetNameOfEntity(pBarFly->ID()) << ": " << "Drinkin' a delicious Beer";

	//if he drinks more than 5 beers
	if (pBarFly->isAggressive())
	{
		pBarFly->GetFSM()->ChangeState(FightMinerBob::Instance());
	}

}


void CalmAndDrinking::Exit(BarFly* pBarFly)
{
	cout << "\n" << GetNameOfEntity(pBarFly->ID()) << ": "
		<< "Oh this beer was a lil' too strong !";
}


bool CalmAndDrinking::OnMessage(BarFly* pBarFly, const Telegram& msg)
{
	//send msg to global message handler
	return false;
}

//------------------------------------------------------------------------methods for FightMinerBob

FightMinerBob* FightMinerBob::Instance()
{
	static FightMinerBob instance;

	return &instance;
}

void FightMinerBob::Enter(BarFly* pBarFly)
{
	cout << "\n" << GetNameOfEntity(pBarFly->ID()) << ": " << "Grumph where is the fukin' miner ?!";

}


void FightMinerBob::Execute(BarFly* pBarFly)
{
	pBarFly->BuyAndDrinkABeer();
	cout << "\n" << GetNameOfEntity(pBarFly->ID()) << ": " << "Gimme one more beer now ! I'm angry !";
	if (pBarFly->isTired()) {
		pBarFly->GetFSM()->ChangeState(Sleeping::Instance());
	}
}


void FightMinerBob::Exit(BarFly* pBarFly)
{
	cout << "\n" << GetNameOfEntity(pBarFly->ID()) << ": " << "Aww so tired now ...";
}


bool FightMinerBob::OnMessage(BarFly* pBarFly, const Telegram& msg)
{
	//send msg to global message handler
	return false;
}
//------------------------------------------------------------------------methods for Sleeping

Sleeping* Sleeping::Instance()
{
	static Sleeping instance;

	return &instance;
}

void Sleeping::Enter(BarFly* pBarFly)
{
	pBarFly->SetFatigue();
	cout << "\n" << GetNameOfEntity(pBarFly->ID()) << ": " << "Goin' to sleep now ...";

}

void Sleeping::Execute(BarFly* pBarFly)
{
	//if the barfly is not tired he can start to drink again
	if (pBarFly->GetFatigue() == 0)
	{
		cout << "\n" << GetNameOfEntity(pBarFly->ID()) << ": "
			<< "Ahh feelin' so well now";

		pBarFly->GetFSM()->ChangeState(CalmAndDrinking::Instance());
	}
	else {
		pBarFly->DecreaseFatigue();

		cout << "\n" << GetNameOfEntity(pBarFly->ID()) << ": " << "ZZZZ...  (Snoring loudly)";
	}
	
}

void Sleeping::Exit(BarFly* pBarFly)
{
}


bool Sleeping::OnMessage(BarFly* pBarFly, const Telegram& msg)
{
	return false; //send message to global message handler
}