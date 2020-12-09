#include "3DPCH.h"
#include "Input_Mouse.h"

MouseEvent::MouseEvent()
{
	this->eventType = Event::Invalid;
	mousePos.x = 0;
	mousePos.y = 0;
}
MouseEvent::MouseEvent(Event evnt, float x, float y)
{
	this->eventType = evnt;
	mousePos.x = x;
	mousePos.y = y;
}
float MouseEvent::getPosX() const
{
	return mousePos.x;
}
float MouseEvent::getPosY()const
{
	return mousePos.y;
}
Event MouseEvent::getEvent() const
{
	return this->eventType;
}
MousePos MouseEvent::getPos() const
{
	return mousePos;
}
bool MouseEvent::isValid() const
{
	return this->eventType != Event::Invalid;
}
Mouse::Mouse()
{
	m_LDown = false;
	m_MDown = false;
	m_RDown = false;
	m_mousePos.x = 0;
	m_mousePos.y = 0;
}

MouseEvent Mouse::readEvent()
{
	MouseEvent evnt;
	if (m_events.empty())
	{

	}
	else
	{
		evnt = m_events.front();
		m_events.pop();
	}

	return evnt;
}

bool Mouse::empty() const
{
	return m_events.empty();
}

void Mouse::onMove(MousePos pos)
{
	std::cout << pos.x << ", " << pos.y << std::endl;

	m_mousePos = pos;
	m_events.push(MouseEvent(Event::MouseMove, (pos.x), (pos.y)));
}
void Mouse::onRawMove(MousePos pos)
{
	m_events.push(MouseEvent(Event::MouseRAW_MOVE, pos.x, pos.y));
}
void Mouse::onLeftPress(MousePos pos)
{
	m_LDown = true;
	m_events.push(MouseEvent(Event::MouseLPressed, pos.x, pos.y));
}
void Mouse::onRightPress(MousePos pos)
{
	m_RDown = true;
	m_events.push(MouseEvent(Event::MouseRPressed, pos.x, pos.y));
}
void Mouse::onMiddlePress(MousePos pos)
{
	m_MDown = true;
	m_events.push(MouseEvent(Event::MouseMPressed, pos.x, pos.y));
}
void Mouse::onLeftRelease(MousePos pos)
{
	m_LDown = false;
	m_events.push(MouseEvent(Event::MouseLReleased, pos.x, pos.y));
}
void Mouse::onRightRelease(MousePos pos)
{
	m_RDown = false;
	m_events.push(MouseEvent(Event::MouseRReleased, pos.x, pos.y));
}
void Mouse::onMiddleRelease(MousePos pos)
{
	m_MDown = false;
	m_events.push(MouseEvent(Event::MouseMReleased, pos.x, pos.y));
}
void Mouse::onWheelUp(MousePos pos)
{
	m_events.push(MouseEvent(Event::MouseWheelUp, pos.x, pos.y));
}
void Mouse::onWheelDown(MousePos pos)
{
	m_events.push(MouseEvent(Event::MouseWheelDown, pos.x, pos.y));
}

bool Mouse::isLDown() const
{
	return m_LDown;
}
bool Mouse::isRDown() const
{
	return m_RDown;
}
bool Mouse::isMDown() const
{
	return m_MDown;
}
float Mouse::getPosX() const
{
	return m_mousePos.x;
}
float Mouse::getPosY() const
{
	return m_mousePos.y;
}
MousePos Mouse::getPos() const
{
	return m_mousePos;
}