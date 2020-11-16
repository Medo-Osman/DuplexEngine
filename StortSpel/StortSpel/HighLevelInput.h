#pragma once



enum Action
{
	JUMP,
	DASH,
	ROLL,
	USEPICKUP,
	PLAYSOUND,
	TEST_SCENE,
	LOAD_SCENE,
	CLOSEINTROGUI,
	USE,
};

enum State
{
	WALK_LEFT,
	WALK_RIGHT,
	WALK_FORWARD,
	WALK_BACKWARD,
};

enum Range
{
	RAW,
	REL,
};

