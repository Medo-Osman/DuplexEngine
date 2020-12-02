#pragma once
#include"iContext.h"

class GUIContext : public iContext
{
private:


public:
	GUIContext()
	{
		//States
		m_stateMapping['\x26'] = State::MENU_UP;	// Arrow Up
		m_stateMapping['\x28'] = State::MENU_DOWN;	// Arrow Down

		//Mouse click
		m_actionMapping[(char)'m_rp'] = Action::USE; // Example of using mouse for action, m_rp is right mouse pressed.
		m_actionMapping[(char)' '] = Action::SELECT; // Space
		m_actionMapping['\x1B'] = Action::MENU;

		//Range
		m_rangeMapping[(char)'move'] = Range::REL; //Mouse position, can be used for UI and such. Leave it in here for example.
		
	}
};