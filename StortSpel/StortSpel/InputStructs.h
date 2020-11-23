#pragma once

enum class Event
{
	Invalid,
	Pressed,
	Released,
	MouseLPressed,
	MouseLReleased,
	MouseRPressed,
	MouseRReleased,
	MouseMPressed,
	MouseMReleased,
	MouseWheelUp,
	MouseWheelDown,
	MouseMove,
	MouseRAW_MOVE
};

struct MousePos
{
	float x;
	float y;
};

