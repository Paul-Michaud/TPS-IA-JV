#include "MinerOwnedStates.h"
#include "fsm/State.h"
#include "Miner.h"
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


//------------------------------------------------------------------------methods for EnterMineAndDigForNugget
EnterMineAndDigForNugget* EnterMineAndDigForNugget::Instance()
{
  static EnterMineAndDigForNugget instance;

  return &instance;
}


void EnterMineAndDigForNugget::Enter(Miner* pMiner)
{
  //if the miner is not already located at the goldmine, he must
  //change location to the gold mine
  if (pMiner->Location() != goldmine){

	pMiner->pushMsg(MSG, "Walkin' to the goldmine");
    pMiner->ChangeLocation(goldmine);
  }
}


void EnterMineAndDigForNugget::Execute(Miner* pMiner)
{  
  //Now the miner is at the goldmine he digs for gold until he
  //is carrying in excess of MaxNuggets. If he gets thirsty during
  //his digging he packs up work for a while and changes state to
  //gp to the saloon for a whiskey.
  pMiner->AddToGoldCarried(1);

  pMiner->IncreaseFatigue();

  pMiner->pushMsg(MSG, "Pickin' up a nugget");

  //if enough gold mined, go and put it in the bank
  if (pMiner->PocketsFull())
  {
    pMiner->GetFSM()->ChangeState(VisitBankAndDepositGold::Instance());
  }

  if (pMiner->Thirsty())
  {
    pMiner->GetFSM()->ChangeState(QuenchThirst::Instance());
  }
}


void EnterMineAndDigForNugget::Exit(Miner* pMiner)
{
	pMiner->pushMsg(MSG, "Ah'm leavin' the goldmine with mah pockets full o' sweet gold");
}


bool EnterMineAndDigForNugget::OnMessage(Miner* pMiner, const Telegram& msg)
{
  //send msg to global message handler
  return false;
}

//------------------------------------------------------------------------methods for VisitBankAndDepositGold

VisitBankAndDepositGold* VisitBankAndDepositGold::Instance()
{
  static VisitBankAndDepositGold instance;

  return &instance;
}

void VisitBankAndDepositGold::Enter(Miner* pMiner)
{  
  //on entry the miner makes sure he is located at the bank
  if (pMiner->Location() != bank)
  {
	pMiner->pushMsg(MSG, "Goin' to the bank. Yes siree");
    pMiner->ChangeLocation(bank);
  }
}


void VisitBankAndDepositGold::Execute(Miner* pMiner)
{
  //deposit the gold
  pMiner->AddToWealth(pMiner->GoldCarried());
    
  pMiner->SetGoldCarried(0);

  pMiner->pushMsg(MSG, "Depositing gold.Total savings now : " + std::to_string(pMiner->Wealth()));

  //wealthy enough to have a well earned rest?
  if (pMiner->Wealth() >= ComfortLevel)
  {
	pMiner->pushMsg(MSG, "WooHoo! Rich enough for now. Back home to mah li'lle lady");
    pMiner->GetFSM()->ChangeState(GoHomeAndSleepTilRested::Instance());      
  }

  //otherwise get more gold
  else 
  {
    pMiner->GetFSM()->ChangeState(EnterMineAndDigForNugget::Instance());
  }
}


void VisitBankAndDepositGold::Exit(Miner* pMiner)
{
	pMiner->pushMsg(MSG, "Leavin' the bank");
}


bool VisitBankAndDepositGold::OnMessage(Miner* pMiner, const Telegram& msg)
{
  //send msg to global message handler
  return false;
}
//------------------------------------------------------------------------methods for GoHomeAndSleepTilRested

GoHomeAndSleepTilRested* GoHomeAndSleepTilRested::Instance()
{
  static GoHomeAndSleepTilRested instance;

  return &instance;
}

void GoHomeAndSleepTilRested::Enter(Miner* pMiner)
{
  if (pMiner->Location() != shack)
  {

	pMiner->pushMsg(MSG, "Walkin' home");

    pMiner->ChangeLocation(shack); 

    //let the wife know I'm home
    Dispatch->DispatchMessage(SEND_MSG_IMMEDIATELY, //time delay
                              pMiner->ID(),        //ID of sender
                              ent_Elsa,            //ID of recipient
                              Msg_HiHoneyImHome,   //the message
                              NO_ADDITIONAL_INFO);    
  }
}

void GoHomeAndSleepTilRested::Execute(Miner* pMiner)
{ 
  //if miner is not fatigued start to dig for nuggets again.
  if (!pMiner->Fatigued())
  {
	 pMiner->pushMsg(MSG, "All mah fatigue has drained away. Time to find more gold!");
     pMiner->GetFSM()->ChangeState(EnterMineAndDigForNugget::Instance());
  }

  else 
  {
    //sleep
    pMiner->DecreaseFatigue();
	pMiner->pushMsg(MSG, "ZZZZ... ");
  } 
}

void GoHomeAndSleepTilRested::Exit(Miner* pMiner)
{ 
}


bool GoHomeAndSleepTilRested::OnMessage(Miner* pMiner, const Telegram& msg)
{

   switch(msg.Msg)
   {
   case Msg_StewReady:

	 pMiner->pushMsg(HANDLED_MSG);
	 pMiner->pushMsg(MSG, "Okay Hun, ahm a comin'!");
     pMiner->GetFSM()->ChangeState(EatStew::Instance());
      
     return true;

   }//end switch

   return false; //send message to global message handler
}

//------------------------------------------------------------------------QuenchThirst

QuenchThirst* QuenchThirst::Instance()
{
  static QuenchThirst instance;

  return &instance;
}

void QuenchThirst::Enter(Miner* pMiner)
{
  
  if (pMiner->Location() != saloon)
  {    
	pMiner->pushMsg(MSG, "Boy, ah sure is thusty! Walking to the saloon");
	pMiner->ChangeLocation(saloon);
  }
}

void QuenchThirst::Execute(Miner* pMiner)
{
  pMiner->BuyAndDrinkAWhiskey();
  pMiner->pushMsg(MSG, "That's mighty fine sippin' liquer");
  pMiner->GetFSM()->ChangeState(EnterMineAndDigForNugget::Instance());  
}


void QuenchThirst::Exit(Miner* pMiner)
{ 
  //cout << "\n" << GetNameOfEntity(pMiner->ID()) << ": " << "Leaving the saloon, feelin' good";
}


bool QuenchThirst::OnMessage(Miner* pMiner, const Telegram& msg)
{

	switch (msg.Msg){

		case Msg_BarFlyWantsToFight:

			pMiner->pushMsg(HANDLED_MSG);
			pMiner->pushMsg(MSG, "Tannon is still here to lose a fight ? Let's go !");
			pMiner->GetFSM()->ChangeState(FightBarFly::Instance());

			return true;

	}//end switch

	return false; //send message to global message handler

}

//------------------------------------------------------------------------EatStew

EatStew* EatStew::Instance()
{
  static EatStew instance;

  return &instance;
}


void EatStew::Enter(Miner* pMiner)
{
  pMiner->pushMsg(MSG, "Smells Reaaal goood Elsa!");
}

void EatStew::Execute(Miner* pMiner)
{
  pMiner->pushMsg(MSG, "Tastes real good too!");
  pMiner->GetFSM()->RevertToPreviousState();
}

void EatStew::Exit(Miner* pMiner)
{ 
  pMiner->pushMsg(MSG, "Thankya li'lle lady. Ah better get back to whatever ah wuz doin'");
}


bool EatStew::OnMessage(Miner* pMiner, const Telegram& msg)
{
  //send msg to global message handler
  return false;
}


//------------------------------------------------------------------------FightBarFly

FightBarFly* FightBarFly::Instance()
{
	static FightBarFly instance;

	return &instance;
}


void FightBarFly::Enter(Miner* pMiner){
	pMiner->pushMsg(MSG, "Hey ! Tannon, I'm here !");
}

void FightBarFly::Execute(Miner* pMiner)
{
	pMiner->pushMsg(MSG, "It's all you can do Tannon ?");

	//let Bar Fly know that the Miner won the fight
	Dispatch->DispatchMessage(SEND_MSG_IMMEDIATELY, //time delay
		pMiner->ID(),        //ID of sender
		ent_BarFly,            //ID of recipient
		Msg_MinerFightsBarFly,   //the message
		NO_ADDITIONAL_INFO);
}

void FightBarFly::Exit(Miner* pMiner)
{
	pMiner->pushMsg(MSG, "That's was a good fight but you lost !");
}


bool FightBarFly::OnMessage(Miner* pMiner, const Telegram& msg)
{

	switch (msg.Msg) {

	case Msg_BarFlyLostFight:

		pMiner->pushMsg(HANDLED_MSG);
		pMiner->GetFSM()->RevertToPreviousState();

		return true;

	}//end switch

	//send msg to global message handler
	return false;
}
