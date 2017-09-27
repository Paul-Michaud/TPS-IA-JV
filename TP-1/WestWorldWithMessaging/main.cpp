#include <fstream>
#include <time.h>
#include <thread>
#include <mutex>
#include <iostream>

#include "Locations.h"
#include "Miner.h"
#include "MinersWife.h"
#include "BarFly.h"
#include "EntityManager.h"
#include "MessageDispatcher.h"
#include "misc/ConsoleUtils.h"
#include "EntityNames.h"


using namespace std;

std::ofstream os;

void LaunchBob(Miner* bob) {
	bob->Update();
}

void LaunchElsa(MinersWife* elsa) {

	//seed random number generator
	srand(clock() % 100 * 1000);

	elsa->Update();
}

void LaunchTannon(BarFly* tannnon) {
	tannnon->Update();
}


int main(){

//define this to send output to a text file (see locations.h)
#ifdef TEXTOUTPUT
  os.open("output.txt");
#endif

  //seed random number generator
  srand((unsigned) time(NULL));

  //Create a mutex
  std::mutex* mutex = new std::mutex();

  //give mutex for cout
  Dispatch->setMutex(mutex);

  //create a miner (give mutex for cout)
  Miner* bob = new Miner(ent_Miner_Bob, mutex);

  //create his wife
  MinersWife* elsa = new MinersWife(ent_Elsa, mutex);

  //create a barfly
  BarFly* tannon = new BarFly(ent_BarFly, mutex);

  //register them with the entity manager
  EntityMgr->RegisterEntity(bob);
  EntityMgr->RegisterEntity(elsa);
  EntityMgr->RegisterEntity(tannon);

  /*----------------*/
  /*BEGINNING STATES*/
  /*----------------*/
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

  bob->GetFSM()->SetPreviousState(EnterMineAndDigForNugget::Instance());
  switch (bobStateNumber) {
  case 1:

	  bob->GetFSM()->SetCurrentState(EnterMineAndDigForNugget::Instance());
	  break;
  case 2:
	  bob->GetFSM()->SetCurrentState(VisitBankAndDepositGold::Instance());
	  break;
  case 3:
	  bob->GetFSM()->SetCurrentState(GoHomeAndSleepTilRested::Instance());
	  break;
  case 4:
	  bob->GetFSM()->SetCurrentState(QuenchThirst::Instance());
	  break;
  case 5:
	  bob->GetFSM()->SetCurrentState(EatStew::Instance());
	  break;
  case 6:
	  bob->GetFSM()->SetCurrentState(FightBarFly::Instance());
	  break;
  default:
	  bob->GetFSM()->SetCurrentState(EnterMineAndDigForNugget::Instance());
  }

  elsa->GetFSM()->SetPreviousState(DoHouseWork::Instance());
  switch (elsaStateNumber) {
  case 1:
	  elsa->GetFSM()->SetCurrentState(DoHouseWork::Instance());
	  break;
  case 2:
	  elsa->GetFSM()->SetCurrentState(VisitBathroom::Instance());
	  break;
  case 3:
	  elsa->GetFSM()->SetCurrentState(CookStew::Instance());
	  break;
  default:
	  elsa->GetFSM()->SetCurrentState(DoHouseWork::Instance());
  }

  tannon->GetFSM()->SetPreviousState(CalmAndDrinking::Instance());
  switch (tannonStateNumber) {
  case 1:
	  tannon->GetFSM()->SetCurrentState(CalmAndDrinking::Instance());
	  break;
  case 2:
	  tannon->GetFSM()->SetCurrentState(FightMinerBob::Instance());
	  break;
  case 3:
	  tannon->GetFSM()->SetCurrentState(Sleeping::Instance());
	  break;
  default:
	  tannon->GetFSM()->SetCurrentState(CalmAndDrinking::Instance());
  }
  /*--------------------------*/

  //run Bob and Elsa through a few Update calls
  for (int i = 0; i<30; ++i)
  {

	  //Display loop number
	  SetTextColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	  std::cout << "\n------------------------------------";
	  std::cout << "\n    Loop " << i+1;
	  std::cout << "\n------------------------------------";

	  std::thread bobThread(LaunchBob, bob);
	  std::thread elsaThread(LaunchElsa, elsa);
	  std::thread tannonThread(LaunchTannon, tannon);

	  //Waiting for threads to finish
	  bobThread.join();
	  elsaThread.join();
	  tannonThread.join();

	  //dispatch any delayed messages
	  Dispatch->DispatchDelayedMessages();
	  Sleep(800);
  }

  //tidy up
  delete bob;
  delete elsa;
  delete tannon;

  //wait for a keypress before exiting
  PressAnyKeyToContinue();


  return 0;
}






