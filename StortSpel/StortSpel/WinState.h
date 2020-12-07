#pragma once
//#include "GUIContext.h"
#include "GUIHandler.h"
#include "Scene.h"
#include "Player.h"
class WinState: public GUIObserver
{
private:
	
public:
	bool firstWinState;
	bool secondWinState;


	virtual void update(const float& dt);
};

