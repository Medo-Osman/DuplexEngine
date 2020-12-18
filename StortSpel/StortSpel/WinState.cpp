#include "3DPCH.h"
#include "WinState.h"

WinState::WinState()
{
}

void WinState::loadTimerText()
{
	GUITextStyle style;
	style.position = Vector2(900, 62);
	style.color = Colors::White;
	m_timerIndex = GUIHandler::get().addGUIText(std::to_string(m_minute) + ":" + std::to_string(m_seconds), L"squirk.spritefont", style);
	//m_timerIndex = GUIHandler::get().addGUIText(std::to_string(m_totalSeconds), L"squirk.spritefont", style);
	GUIHandler::get().setVisible(m_timerIndex, false);
}

void WinState::update(const float& dt)
{
	m_totalSeconds = m_time.timeElapsed();
	m_seconds = ((m_timeLimit - m_totalSeconds) % 60);
	m_minute = ((m_timeLimit - m_totalSeconds) % 3600 / 60);
}