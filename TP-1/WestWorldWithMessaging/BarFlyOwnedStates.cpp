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

	pBarFly->pushMsg(MSG, "Oh what a good day to drink some beers !");
}


void CalmAndDrinking::Execute(BarFly* pBarFly)
{
	//The barfly will drink some beers, if he drinks too much he will
	//change state to AggressiveAndDrinking


	pBarFly->BuyAndDrinkABeer();

	pBarFly->pushMsg(MSG, "Drinkin' a delicious Beer");

	//if he drinks more than 5 beers
	if (pBarFly->isAggressive()) pBarFly->GetFSM()->ChangeState(FightMinerBob::Instance());
}


void CalmAndDrinking::Exit(BarFly* pBarFly)
{
	pBarFly->pushMsg(MSG, "Oh this beer was a lil' too strong !");
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
	pBarFly->pushMsg(MSG, "Grumph where is the fukin' miner ?!");
}


void FightMinerBob::Execute(BarFly* pBarFly)
{
	pBarFly->BuyAndDrinkABeer();
	
	if (pBarFly->isTired()) pBarFly->GetFSM()->ChangeState(Sleeping::Instance());
	else {
	  pBarFly->pushMsg(MSG, "Grumph where is the fukin' miner ?! Gimme one more beer now ! I'm angry !");

	  //let Miner Bob know that the Bar Fly wants to fight
	  Dispatch->DispatchMessage(SEND_MSG_IMMEDIATELY, //time delay
		pBarFly->ID(),        //ID of sender
		ent_Miner_Bob,            //ID of recipient
		Msg_BarFlyWantsToFight,   //the message
		NO_ADDITIONAL_INFO);
	}

}


void FightMinerBob::Exit(BarFly* pBarFly)
{
	pBarFly->pushMsg(MSG, "Aw so tired now ...");
}


bool FightMinerBob::OnMessage(BarFly* pBarFly, const Telegram& msg)
{

	switch (msg.Msg) {

	case Msg_MinerFightsBarFly:

		pBarFly->pushMsg(HANDLED_MSG);
		pBarFly->pushMsg(MSG, "I Lost the fight ...");

		//let Miner  know that the Bar Fly lost to fight
		Dispatch->DispatchMessage(SEND_MSG_IMMEDIATELY, //time delay
			pBarFly->ID(),        //ID of sender
			ent_Miner_Bob,            //ID of recipient
			Msg_BarFlyLostFight,   //the message
			NO_ADDITIONAL_INFO);

		pBarFly->GetFSM()->ChangeState(Sleeping::Instance());



		return true;

	}//end switch

	return false; //send message to global message handler


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
	pBarFly->pushMsg(MSG, "Goin' to sleep now ...");
}

void Sleeping::Execute(BarFly* pBarFly)
{
	//if the barfly is not tired he can start to drink again
	if (pBarFly->GetFatigue() == 0){
		pBarFly->pushMsg(MSG, "Ahh feelin' so well now");
		pBarFly->GetFSM()->ChangeState(CalmAndDrinking::Instance());
	}
	else {
		pBarFly->DecreaseFatigue();
		pBarFly->pushMsg(MSG, "ZZZZ...  (Snoring loudly)");
	}
	
}

void Sleeping::Exit(BarFly* pBarFly)
{
}


bool Sleeping::OnMessage(BarFly* pBarFly, const Telegram& msg)
{
	return false; //send message to global message handler
}