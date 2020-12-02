#pragma once
#include "Input_Mouse.h"
#include "Input_Keyboard.h"
#include"iContext.h"
#include"MovementContext.h"
#include "GUIContext.h"

struct RangeData
{
	Range rangeFlag;
	MousePos pos;
	RangeData(Range range, MousePos mousePosition)
	{
		rangeFlag = range;
		pos = mousePosition;
	}
};

struct InputData
{
	std::vector<Action> actionData;
	std::vector<State> stateData;
	std::vector<RangeData> rangeData;
	Mouse* mousePtr = nullptr;

	void removeStateData(State stateID)
	{
		bool removed = false;
		for (std::vector<int>::size_type i = 0; i < stateData.size() && !removed; i++) {
			if (stateData[i] == stateID)
			{
				stateData.erase(stateData.begin() + i);
				removed = true;
			}
		}
	}
	void clearStateData()
	{
		stateData.clear();
	}
	void removeActionData(const int& pos)
	{
		actionData.erase(actionData.begin() + pos);
	}

	void removeRangeData(const int& pos)
	{
		rangeData.erase(rangeData.begin() + pos);
	}

};


class InputObserver {
public:
	InputObserver() {};
	virtual ~InputObserver() {};
	virtual void inputUpdate(InputData& inputData) = 0;
};

class InputSubject {
public:
	virtual ~InputSubject() {};
	virtual void Attach(InputObserver* observer) = 0;
	virtual void Detach(InputObserver* observer) = 0;
};

class Input : InputSubject
{
private:
	Keyboard m_Keyboard;
	Mouse m_Mouse;
	bool m_cursorEnabled;

	// Gamepad
	std::unique_ptr<GamePad> m_gamepad;
	GamePad::ButtonStateTracker m_tracker;
	float m_rightStickSensetivity	= 1000.f;
	float m_centerDeadzoneLeftLimit	= 0.3f;
	bool m_invertedRightStickY		= false;

	std::vector<InputObserver*> m_inputObservers;
	std::vector<iContext*> m_contexts;


	InputData m_currentInputData;

	bool fillInputDataUsingKey(const char& key, const bool& wasPressed, const bool& isMouse = false);
	bool inputDataChanged = false;
public:
	Input();
	~Input();
	LRESULT handleMessages(HWND hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam);
	void readBuffers(const float& dt);
	void addContext(iContext* context);
	void removeContext(iContext* context);

	MouseEvent getMouseEvent();
	Keyboard* getKeyboard();
	Mouse* getMouse();
	std::vector<iContext*>* getIContextPtr();

	void Attach(InputObserver* observer);
	void Detach(InputObserver* observer);
};
