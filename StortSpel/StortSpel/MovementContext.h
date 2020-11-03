#pragma once
#include"iContext.h"

class MovementContext : public iContext
{
private:


public:
	MovementContext()
	{
		//Actions
		m_actionMapping[' '] = Action::JUMP;
		m_actionMapping['O'] = Action::PLAYSOUND;
		m_actionMapping['P'] = Action::SWAP_SCENES;
		m_actionMapping['\x10'] = Action::DASH; // Shift: '\x10'
		m_actionMapping[(char)'m_rp'] = Action::USE; // Example of using mouse for action, m_rp is right mouse pressed.
		m_actionMapping[(char)'\x0D'] = Action::CLOSEINTROGUI; //Enter 

		//States
		m_stateMapping['W'] = State::WALK_FORWARD;
		m_stateMapping['S'] = State::WALK_BACKWARD;
		m_stateMapping['D'] = State::WALK_RIGHT;
		m_stateMapping['A'] = State::WALK_LEFT;

		//Range
		//m_rangeMapping[(char)'move'] = Range::REL; //Mouse position, can be used for UI and such. Leave it in here for example.
		m_rangeMapping[(char)'raw'] = Range::RAW; //How much has the mouse moved.
		
	}
};