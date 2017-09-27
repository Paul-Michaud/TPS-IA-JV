#include "BaseGameEntity.h"
#include <cassert>



int BaseGameEntity::m_iNextValidID = 0;



//----------------------------- SetID -----------------------------------------
//
//  this must be called within each constructor to make sure the ID is set
//  correctly. It verifies that the value passed to the method is greater
//  or equal to the next valid ID, before setting the ID and incrementing
//  the next valid ID
//-----------------------------------------------------------------------------
void BaseGameEntity::SetID(int val)
{
  //make sure the val is equal to or greater than the next available ID
  assert ( (val >= m_iNextValidID) && "<BaseGameEntity::SetID>: invalid ID");

  m_ID = val;
    
  m_iNextValidID = m_ID + 1;
}

//----------------------------- printMsg --------------------------------------
//
//  Allow an entity to print a message with his color
//-----------------------------------------------------------------------------
/*void BaseGameEntity::printMsg(std::string msg) {
	(*m_mutex).lock();
	SetTextColor(this->colorText | FOREGROUND_INTENSITY);
	std::cout << "\n" << GetNameOfEntity(ID()) << " : " << msg << ".";
	(*m_mutex).unlock();
}

//----------------------------- printHandleMsg --------------------------------
//
//  Allow an entity to print a handled message with his color
//-----------------------------------------------------------------------------
void BaseGameEntity::printHandledMsg() {
	(*m_mutex).lock();
	SetTextColor(BACKGROUND_RED | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	std::cout << "\nMessage handled by " << GetNameOfEntity(ID()) << " at time: " << Clock->GetCurrentTime();
	(*m_mutex).unlock();
}*/


//----------------------------- pushMsg --------------------------------
//
//  Push message in a dedicated queue
//----------------------------------------------------------------------
void BaseGameEntity::pushMsg(int msg_type, std::string msg){
	m_msgTypeQueue.push(msg_type);
	m_msgQueue.push(msg);
}

//----------------------------- printQueuedMsg --------------------------------
//
//  Print all queued messages in console
//-----------------------------------------------------------------------------
void BaseGameEntity::printQueuedMsg() {

	while (!m_msgQueue.empty()) {

		(*m_mutex).lock();

		if (m_msgTypeQueue.front() == MSG) {
			SetTextColor(this->colorText | FOREGROUND_INTENSITY);
			std::cout << "\n" << GetNameOfEntity(ID()) << " : " << m_msgQueue.front() << ".";
		}
		else if (m_msgTypeQueue.front() == HANDLED_MSG) {
			SetTextColor(BACKGROUND_RED | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
			std::cout << "\nMessage handled by " << GetNameOfEntity(ID()) << " at time: " << Clock->GetCurrentTime();
		}
		m_msgTypeQueue.pop();
		m_msgQueue.pop();

		(*m_mutex).unlock();
	}

}