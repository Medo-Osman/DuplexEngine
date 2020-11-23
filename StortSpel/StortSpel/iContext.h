#pragma once
#include"3DPCH.h"
#include"HighLevelInput.h"


class iContext
{
protected:
	std::map<char, Action> m_actionMapping;
	std::map<char, State> m_stateMapping;
	std::map<char, Range> m_rangeMapping;

	bool m_mute = false;
public:
	
	int getAction(char character)
	{
		int action = -1;
		if (m_actionMapping.find(character) != m_actionMapping.end()) //CharacterFound
		{
			action = (int)m_actionMapping[character];
		}
		return action;
	}

	void setMute(bool eatAll)
	{
		m_mute = eatAll;
	}

	bool getMute()
	{
		return m_mute;
	}

	int getState(char character)
	{
		int state = -1;
		if (m_stateMapping.find(character) != m_stateMapping.end()) //CharacterFound
		{
			state = (int)m_stateMapping[character];
		}
		return state;
	}

	int getRange(char character)
	{
		int range = -1;
		if (m_rangeMapping.find(character) != m_rangeMapping.end()) //CharacterFound
		{
			range = (int)m_rangeMapping[character];
		}
		return range;
	}

	static int convertEventToMouseChar(Event mouseEvent)
	{
		int convertedChar = 0;
		switch (mouseEvent)
		{
		case Event::MouseLPressed:
			convertedChar = 'm_lp';
			break;
		case Event::MouseLReleased:
			convertedChar = 'm_lr';
			break;
		case Event::MouseRPressed:
			convertedChar = 'm_rp';
			break;
		case Event::MouseRReleased:
			convertedChar = 'm_rr';
			break;
		case Event::MouseMPressed:
			convertedChar = 'm_mp';
			break;
		case Event::MouseMReleased:
			convertedChar = 'm_mr';
			break;
		case Event::MouseWheelUp:
			convertedChar = 'm_wp';
			break;
		case Event::MouseWheelDown:
			convertedChar = 'm_wr';
			break;
		case Event::MouseMove:
			convertedChar = 'move';
			break;
		case Event::MouseRAW_MOVE:
			convertedChar = 'raw';
			break;
		default:
			break;
		}

		return convertedChar;
	}

};