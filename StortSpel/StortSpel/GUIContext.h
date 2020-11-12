#pragma once
#include"iContext.h"

class GUIContext : public iContext
{
private:


public:
	GUIContext()
	{
		//Mouse click
		m_actionMapping[(char)'m_rp'] = Action::USE; // Example of using mouse for action, m_rp is right mouse pressed.

		//Range
		m_rangeMapping[(char)'move'] = Range::REL; //Mouse position, can be used for UI and such. Leave it in here for example.
		
	}
};