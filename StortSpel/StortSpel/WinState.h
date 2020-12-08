#pragma once
//#include "GUIContext.h"
#include "GUIHandler.h"
#include "Scene.h"
#include "Player.h"
class WinState: public GUIObserver
{
private:
	Timer m_time;
	
public:
	WinState();
	bool firstWinState;
	bool secondWinState;
	
	void timeInMinutes();

	virtual void update(const float& dt);
};

