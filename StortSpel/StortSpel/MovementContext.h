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
		m_actionMapping[(char)'m_r'] = Action::USE;

		//States
		m_stateMapping['W'] = State::WALK_FORWARD;
		m_stateMapping['s'] = State::WALK_BACKWARD;
		m_stateMapping['D'] = State::WALK_LEFT;
		m_stateMapping['A'] = State::WALK_RIGHT;

		//Range
		m_rangeMapping[(char)'move'] = Range::REL;
		m_rangeMapping[(char)'raw'] = Range::RAW;
		
	}
};