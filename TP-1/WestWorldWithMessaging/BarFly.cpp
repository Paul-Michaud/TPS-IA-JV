#include "BarFly.h"

bool BarFly::HandleMessage(const Telegram& msg)
{
	return m_pStateMachine->HandleMessage(msg);
}


void BarFly::Update()
{
	m_pStateMachine->Update();
	printQueuedMsg();
}
