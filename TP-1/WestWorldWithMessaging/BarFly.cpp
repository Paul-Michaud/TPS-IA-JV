#include "BarFly.h"

bool BarFly::HandleMessage(const Telegram& msg)
{
	return m_pStateMachine->HandleMessage(msg);
}


void BarFly::Update()
{
	SetTextColor(FOREGROUND_GREEN | FOREGROUND_BLUE);

	m_pStateMachine->Update();
}
