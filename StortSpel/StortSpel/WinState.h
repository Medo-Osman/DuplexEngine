#pragma once
//#include "GUIContext.h"
#include "GUIHandler.h"
//#include "Scene.h"
//#include "Player.h"
class WinState
{
private:
	int m_timeLimit = 10;
	
	
public:
	WinState();
	Timer m_time;
	int	m_timerIndex;
	int m_totalSeconds = 0;
	int m_seconds = 0;
	int m_minute = 0;
	void loadTimerText();
	bool firstWinState = false;
	bool secondWinState = false;
	
	

	virtual void update(const float& dt);
};

