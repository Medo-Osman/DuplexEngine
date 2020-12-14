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

	static char convertEventToMouseChar(Event mouseEvent)
	{
		char convertedChar;
		switch (mouseEvent)
		{
		case Event::MouseLPressed:
			convertedChar = '\x0D';
			break;
		case Event::MouseLReleased:
			convertedChar = 'mlr';
			break;
		case Event::MouseRPressed:
			convertedChar = 'mrp';
			break;
		case Event::MouseRReleased:
			convertedChar = 'mrr';
			break;
		case Event::MouseMPressed:
			convertedChar = 'mmp';
			break;
		case Event::MouseMReleased:
			convertedChar = 'mmr';
			break;
		case Event::MouseWheelUp:
			convertedChar = 'mwp';
			break;
		case Event::MouseWheelDown:
			convertedChar = 'mwr';
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