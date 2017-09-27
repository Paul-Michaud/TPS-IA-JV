#include <fstream>
#include <time.h>
#include <iostream>
using namespace std;

#include "Locations.h"
#include "Miner.h"
#include "MinersWife.h"
#include "BarFly.h"
#include "EntityManager.h"
#include "MessageDispatcher.h"
#include "misc/ConsoleUtils.h"
#include "EntityNames.h"


std::ofstream os;

int main()
{
//define this to send output to a text file (see locations.h)
#ifdef TEXTOUTPUT
  os.open("output.txt");
#endif

  //seed random number generator
  srand((unsigned) time(NULL));


  //create a miner
  Miner* Bob = new Miner(ent_Miner_Bob);

  //create his wife
  MinersWife* Elsa = new MinersWife(ent_Elsa);

  //create a barfly
  BarFly* Tannon = new BarFly(ent_BarFly);

  //register them with the entity manager
  EntityMgr->RegisterEntity(Bob);
  EntityMgr->RegisterEntity(Elsa);
  EntityMgr->RegisterEntity(Tannon);

  int bobStateNumber, elsaStateNumber, tannonStateNumber;
  cout << "Starting state for miner, Bob :" << endl << endl;
  cout << "1 (default) : EnterMineAndDigForNugget" << endl;
  cout << "2 : VisitBankAndDepositGold" << endl;
  cout << "3 : GoHomeAndSleepTilRested" << endl;
  cout << "4 : QuenchThirst" << endl;
  cout << "5 : EatStew" << endl;
  cout << "6 : FightBarFly" << endl;
  cout << "Choice : ";
  cin >> bobStateNumber;

  cout << "Starting state for miner wife, Elsa :" << endl << endl;
  cout << "1 (default) : DoHouseWork" << endl;
  cout << "2 : VisitBathroom" << endl;
  cout << "3 : CookStew" << endl;
  cout << "Choice : ";
  cin >> elsaStateNumber;

  cout << "Starting state for barfly, Tannon :" << endl << endl;
  cout << "1 (default) : CalmAndDrinking" << endl;
  cout << "2 : FightMinerBob" << endl;
  cout << "3 : Sleeping" << endl;
  cout << "Choice : ";
  cin >> tannonStateNumber;

  switch (bobStateNumber) {
  case 1:
	  Bob->GetFSM()->ChangeState(EnterMineAndDigForNugget::Instance());
	  break;
  case 2:
	  Bob->GetFSM()->ChangeState(VisitBankAndDepositGold::Instance());
	  break;
  case 3:
	  Bob->GetFSM()->ChangeState(GoHomeAndSleepTilRested::Instance());
	  break;
  case 4:
	  Bob->GetFSM()->ChangeState(QuenchThirst::Instance());
	  break;
  case 5:
	  Bob->GetFSM()->ChangeState(EatStew::Instance());
	  break;
  case 6:
	  Bob->GetFSM()->ChangeState(FightBarFly::Instance());
	  break;
  default:
	  Bob->GetFSM()->ChangeState(EnterMineAndDigForNugget::Instance());  
  }

  switch (elsaStateNumber) {
  case 1:
	  Elsa->GetFSM()->ChangeState(DoHouseWork::Instance());
	  break;
  case 2:
	  Elsa->GetFSM()->ChangeState(VisitBathroom::Instance());
	  break;
  case 3:
	  Elsa->GetFSM()->ChangeState(CookStew::Instance());
	  break;
  default:
	  Elsa->GetFSM()->ChangeState(DoHouseWork::Instance());
  }

  switch (tannonStateNumber) {
  case 1:
	  Tannon->GetFSM()->ChangeState(CalmAndDrinking::Instance());
	  break;
  case 2:
	  Tannon->GetFSM()->ChangeState(FightMinerBob::Instance());
	  break;
  case 3:
	  Tannon->GetFSM()->ChangeState(Sleeping::Instance());
	  break;
  default:
	  Tannon->GetFSM()->ChangeState(CalmAndDrinking::Instance());
  }

  //run Bob and Elsa through a few Update calls
  for (int i=0; i<30; ++i)
  { 
    Bob->Update();
    Elsa->Update();
	Tannon->Update();

    //dispatch any delayed messages
    Dispatch->DispatchDelayedMessages();

    Sleep(800);
  }

  //tidy up
  delete Bob;
  delete Elsa;
  delete Tannon;

  //wait for a keypress before exiting
  PressAnyKeyToContinue();

  return 0;
}






