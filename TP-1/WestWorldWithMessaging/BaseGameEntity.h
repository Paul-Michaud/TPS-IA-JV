#ifndef ENTITY_H
#define ENTITY_H
//------------------------------------------------------------------------
//
//  Name:   BaseGameEntity.h
//
//  Desc:   Base class for a game object
//
//  Author: Mat Buckland 2002 (fup@ai-junkie.com)
//
//------------------------------------------------------------------------
#include <string>
#include <mutex>
#include <queue>
#include <vector>

#include "EntityNames.h"
#include "messaging/Telegram.h"
#include "misc/consoleutils.h"
#include "Time/CrudeTimer.h"

#define MSG 0
#define HANDLED_MSG 1


class BaseGameEntity
{

private:

  //every entity must have a unique identifying number
  int          m_ID;

  //this is the next valid ID. Each time a BaseGameEntity is instantiated
  //this value is updated
  static int  m_iNextValidID;

  //Every entity has a color to print output
  int colorText = FOREGROUND_RED;

  //Queue to store messages types (Msg or HandledMsg)
  std::queue<int> m_msgTypeQueue;
  //Queue to store messages
  std::queue<std::string> m_msgQueue;

  //this must be called within the constructor to make sure the ID is set
  //correctly. It verifies that the value passed to the method is greater
  //or equal to the next valid ID, before setting the ID and incrementing
  //the next valid ID
  void SetID(int val);

public:

  //mutex for thread safe for print on console
  std::mutex* m_mutex;

  BaseGameEntity(int id, std::mutex* mutex){
    SetID(id);
	m_mutex = mutex;
  }

  virtual ~BaseGameEntity(){}

  //all entities must implement an update function
  virtual void  Update()=0;

  //all entities can communicate using messages. They are sent
  //using the MessageDispatcher singleton class
  virtual bool  HandleMessage(const Telegram& msg)=0;

  int           ID()const{return m_ID;}

  //Print message
  /*void printMsg(std::string msg);
  void printHandledMsg();*/

  //Push a message
  void pushMsg(int msg_type, std::string msg = "");
  //Print queued message(s)
  void printQueuedMsg();

  //text color
  void setColorText(int colorText) { this->colorText = colorText; }

};

#endif


