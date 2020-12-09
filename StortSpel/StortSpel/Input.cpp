#include"3DPCH.h"
#include"Input.h"

bool Input::fillInputDataUsingKey(const char& key, const bool& wasPressed, const bool& isMouse)
{
	bool foundContext = false;
	for (size_t i = 0; i < m_contexts.size() && !foundContext; i++) {
		if (wasPressed)
		{
			int action = m_contexts[i]->getAction(key);
			if (action != -1 && !m_contexts[i]->getMute())
			{
				m_currentInputData.actionData.emplace_back(Action(action));
				foundContext = true;
			}
		}

		if (!foundContext && !isMouse)
		{
			int state = m_contexts[i]->getState(key);
			if (state != -1 && !m_contexts[i]->getMute())
			{
				if (wasPressed)
					m_currentInputData.stateData.emplace_back(State(state));
				else
					m_currentInputData.removeStateData(State(state));

				foundContext = true;
			}
		}
	}

	return foundContext;
}

Input::Input()
{
	m_cursorEnabled = false;
	m_contexts.emplace_back(new MovementContext());
	m_contexts.emplace_back(new GUIContext());
	m_currentInputData.mousePtr = &m_Mouse;

	// Gamepad
	m_gamepad = std::make_unique<GamePad>();

}

Input::~Input()
{
	for (size_t i = 0; i < m_contexts.size(); i++)
	{
		delete m_contexts[i];
	}
	m_contexts.clear();
}

LRESULT Input::handleMessages(HWND hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam)
{
	switch (uMsg)
	{
	case WM_KEYDOWN:
	{
		bool wasPressed = lParam & 0x40000000;
		unsigned char key = static_cast<unsigned char>(wParam);
		if (!wasPressed)
		{
			m_Keyboard.onKeyPressed(key);
		}
		/*if (key == VK_ESCAPE)
			uMsg = WM_DESTROY;*/

		if (key == VK_TAB)
		{
			setCursor(!m_cursorEnabled);
		}

		return 0;
	}
	case WM_KEYUP:
	{
		unsigned char key = static_cast<unsigned char>(wParam);
		m_Keyboard.onRelease(key);
		return 0;
	}
	case WM_LBUTTONDOWN:
	{
		if (!m_cursorEnabled)
		{
			while (ShowCursor(FALSE) >= 0);
			RECT windowRect;
			GetClientRect(hwnd, &windowRect);
			MapWindowPoints(hwnd, nullptr, reinterpret_cast<POINT*>(&windowRect), 2);
			ClipCursor(&windowRect);
		}

		MousePos mPos;
		mPos = getMouse()->getPos();
		/*mPos.x = LOWORD(lParam);
		mPos.y = HIWORD(lParam);*/

		m_Mouse.onLeftPress(mPos);
		return 0;
	}
	case WM_LBUTTONUP:
	{
		MousePos mPos;
		mPos.x = LOWORD(lParam);
		mPos.y = HIWORD(lParam);

		m_Mouse.onLeftRelease(mPos);
		return 0;
	}
	case WM_RBUTTONDOWN:
	{
		MousePos mPos;
		mPos.x = LOWORD(lParam);
		mPos.y = HIWORD(lParam);

		m_Mouse.onRightPress(mPos);
		return 0;
	}
	case WM_RBUTTONUP:
	{
		MousePos mPos;
		mPos.x = LOWORD(lParam);
		mPos.y = HIWORD(lParam);

		m_Mouse.onRightRelease(mPos);
		return 0;
	}
	case WM_MBUTTONDOWN:
	{
		MousePos mPos;
		mPos.x = LOWORD(lParam);
		mPos.y = HIWORD(lParam);

		m_Mouse.onMiddlePress(mPos);
		return 0;
	}
	case WM_MBUTTONUP:
	{
		MousePos mPos;
		mPos.x = LOWORD(lParam);
		mPos.y = HIWORD(lParam);

		m_Mouse.onMiddleRelease(mPos);
		return 0;
	}
	case WM_MOUSEMOVE:
	{
		MousePos mPos;
		mPos.x = LOWORD(lParam);
		mPos.y = HIWORD(lParam);

		POINT p = { mPos.x, mPos.y };
		//POINT p;
		//GetCursorPos(&p);
		//ScreenToClient(hwnd, &p);

		mPos.x = p.x;
		mPos.y = p.y;

		m_Mouse.onMove(mPos);
		return 0;
	}
	case WM_MOUSEWHEEL:
	{
		MousePos mPos;
		mPos.x = LOWORD(lParam);
		mPos.y = HIWORD(lParam);

		if (GET_WHEEL_DELTA_WPARAM(wParam) > 0)
		{
			m_Mouse.onWheelUp(mPos);
		}
		else if (GET_WHEEL_DELTA_WPARAM(wParam) < 0)
		{
			m_Mouse.onWheelDown(mPos);
		}
		return 0;
	}
	case WM_INPUT:
	{
		UINT size = 0;
		GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, NULL, &size, sizeof(RAWINPUTHEADER));
		if (size > 0)
		{
			std::unique_ptr<BYTE[]> data = std::make_unique<BYTE[]>(size);
			if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, data.get(), &size, sizeof(RAWINPUTHEADER)) == size)
			{
				RAWINPUT* rInput = reinterpret_cast<RAWINPUT*>(data.get());
				if (rInput->header.dwType == RIM_TYPEMOUSE)
				{
					MousePos mPos;
					mPos.x = (float)rInput->data.mouse.lLastX;
					mPos.y = (float)rInput->data.mouse.lLastY;

					m_Mouse.onRawMove(mPos);
				}
			}
		}

		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	case WM_ACTIVATE:
	{
		if (!m_cursorEnabled)
		{
			if (wParam & WM_ACTIVATE || wParam & WA_CLICKACTIVE)
			{
				while (ShowCursor(FALSE) >= 0);
				RECT windowRect;
				GetClientRect(hwnd, &windowRect);
				MapWindowPoints(hwnd, nullptr, reinterpret_cast<POINT*>(&windowRect), 2);
				ClipCursor(&windowRect);
			}
			else
			{
				while (ShowCursor(TRUE) < 0);
				ClipCursor(nullptr);
			}
		}

	}
	case WA_INACTIVE:
	{
		m_currentInputData.actionData.clear();
		m_currentInputData.rangeData.clear();
		m_currentInputData.stateData.clear();
	}
	case WM_SYSKEYDOWN:
	{
		return (LRESULT)1;
	}
	case WM_SYSKEYUP:
	{
		// NEEDED FOR WINDOW FREEZE BUG WHEN PRESSING ONLY ALT UNTIL THE WINDOW IS CLICKED!!!!
		INPUT    Input = { 0 };
		// left down 
		Input.type = INPUT_MOUSE;
		Input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
		::SendInput(1, &Input, sizeof(INPUT));

		// left up
		::ZeroMemory(&Input, sizeof(INPUT));
		Input.type = INPUT_MOUSE;
		Input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
		::SendInput(1, &Input, sizeof(INPUT));

		return (LRESULT)0;
	}
	case WM_SYSCHAR:
	{
		return (LRESULT)1;
	}
	case WM_SYSCOMMAND:
	{
		if (wParam == SC_KEYMENU && (lParam >> 16) <= 0)
			return 0;
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

	}

	return 0;
}

void Input::readBuffers(const float& dt)
{
	m_currentInputData.actionData.clear();
	m_currentInputData.rangeData.clear();

	while(!m_Keyboard.empty())
	{
		KeyboardEvent readEvent = m_Keyboard.readKey();
		if (readEvent.isPressed())
		{
			inputDataChanged = this->fillInputDataUsingKey(readEvent.getKey(), true) || inputDataChanged;
		}
		else //Is realeased
		{
			inputDataChanged = this->fillInputDataUsingKey(readEvent.getKey(), false) || inputDataChanged;
		}
	}
	
	while(!m_Mouse.empty())
	{
		MouseEvent mouseEvnt = m_Mouse.readEvent();
		char charConverted = iContext::convertEventToMouseChar(mouseEvnt.getEvent());

		if (mouseEvnt.getEvent() == Event::MouseMove || mouseEvnt.getEvent() == Event::MouseRAW_MOVE) //Range
		{
			bool foundContext = false;
			for (size_t i = 0; i < m_contexts.size() && !foundContext; i++) 
			{
				Range rangeState = Range(m_contexts[i]->getRange(charConverted));
				if (rangeState != -1 && !m_contexts[i]->getMute())
				{
					MousePos pos = mouseEvnt.getPos();
					this->m_currentInputData.rangeData.emplace_back(rangeState, pos);
					inputDataChanged = true;
				}
			}
		}
		else //Button
		{
			inputDataChanged = this->fillInputDataUsingKey(charConverted, true, true) || inputDataChanged;
		}
	}
	// Gamepad
	auto gamepadState = m_gamepad->GetState(0, GamePad::DEAD_ZONE_CIRCULAR);

	if (gamepadState.IsConnected())
	{
		// Actions
		m_tracker.Update(gamepadState);
		using BState = GamePad::ButtonStateTracker;

		// A
		if (m_tracker.a == BState::PRESSED)
			inputDataChanged = this->fillInputDataUsingKey(' ', true) || inputDataChanged; // Space
		else if (m_tracker.a == BState::RELEASED)
			inputDataChanged = this->fillInputDataUsingKey(' ', false) || inputDataChanged; // Space

		// X
		if (m_tracker.x == BState::PRESSED)
		{
			inputDataChanged = this->fillInputDataUsingKey('\x10', true) || inputDataChanged; // Shift
			//if (m_gamepad->SetVibration(0, 0.1f, 0.1f)) {} // player 0, leftMotor, rightMotor
		}
		else if (m_tracker.x == BState::RELEASED)
		{
			inputDataChanged = this->fillInputDataUsingKey('\x10', false) || inputDataChanged; // Shift
			//if (m_gamepad->SetVibration(0, 0.f, 0.f)) {}
		}

		// B
		if (m_tracker.b == BState::PRESSED)
		{
			inputDataChanged = this->fillInputDataUsingKey('\x10', true) || inputDataChanged;
			//if (m_gamepad->SetVibration(0, 0.1f, 0.1f)) {}
		}
		else if (m_tracker.b == BState::RELEASED)
		{
			inputDataChanged = this->fillInputDataUsingKey('\x10', false) || inputDataChanged;
			//if (m_gamepad->SetVibration(0, 0.f, 0.f)) {}
		}

		// Y
		if (m_tracker.y == BState::PRESSED)
		{
			inputDataChanged = this->fillInputDataUsingKey('F', true) || inputDataChanged;
			if (m_gamepad->SetVibration(0, 0.5f, 0.5f)) {}
		}
		else if (m_tracker.y == BState::RELEASED)
		{
			inputDataChanged = this->fillInputDataUsingKey('F', false) || inputDataChanged;
			if (m_gamepad->SetVibration(0, 0.f, 0.f)) {}
		}

		// Menu / Start
		if (m_tracker.menu == GamePad::ButtonStateTracker::PRESSED)
			inputDataChanged = this->fillInputDataUsingKey('\x1B', true) || inputDataChanged;
		else if (m_tracker.menu == GamePad::ButtonStateTracker::RELEASED)
			inputDataChanged = this->fillInputDataUsingKey('\x1B', false) || inputDataChanged;

		// View / Back
		if (m_tracker.view == BState::PRESSED)
			inputDataChanged = this->fillInputDataUsingKey('\x0D', true) || inputDataChanged; //Enter 
		else if (m_tracker.view == BState::RELEASED)
			inputDataChanged = this->fillInputDataUsingKey('\x0D', false) || inputDataChanged; //Enter 

		// Walk / Menu Navigation
		// Up
		if (m_tracker.dpadUp == BState::PRESSED)
		{
			inputDataChanged = this->fillInputDataUsingKey('W', true) || inputDataChanged;
			inputDataChanged = this->fillInputDataUsingKey('\x26', true) || inputDataChanged; // Arrow Up
		}
		else if (m_tracker.dpadUp == BState::RELEASED)
		{
			inputDataChanged = this->fillInputDataUsingKey('W', false) || inputDataChanged;
			inputDataChanged = this->fillInputDataUsingKey('\x26', false) || inputDataChanged; // Arrow Up
		}

		// Left
		if (m_tracker.dpadLeft == BState::PRESSED)
		{
			inputDataChanged = this->fillInputDataUsingKey('A', true) || inputDataChanged;
		}
		else if (m_tracker.dpadLeft == BState::RELEASED)
		{
			inputDataChanged = this->fillInputDataUsingKey('A', false) || inputDataChanged;
		}

		// Down
		if (m_tracker.dpadDown == BState::PRESSED)
		{
			inputDataChanged = this->fillInputDataUsingKey('S', true) || inputDataChanged;
			inputDataChanged = this->fillInputDataUsingKey('\x28', true) || inputDataChanged; // Arrow Down
		}
		else if (m_tracker.dpadDown	== BState::RELEASED)
		{
			inputDataChanged = this->fillInputDataUsingKey('S', false) || inputDataChanged;
			inputDataChanged = this->fillInputDataUsingKey('\x28', false) || inputDataChanged; // Arrow Down
		}

		// Right
		if (m_tracker.dpadRight == BState::PRESSED)
		{
			inputDataChanged = this->fillInputDataUsingKey('D', true) || inputDataChanged;
		}
		else if (m_tracker.dpadRight == BState::RELEASED)
		{
			inputDataChanged = this->fillInputDataUsingKey('D', false) || inputDataChanged;
		}

		// Right Trigger
		if (m_tracker.dpadRight == BState::PRESSED)
		{
			inputDataChanged = this->fillInputDataUsingKey('D', true) || inputDataChanged;
		}
		else if (m_tracker.dpadRight == BState::RELEASED)
		{
			inputDataChanged = this->fillInputDataUsingKey('D', false) || inputDataChanged;
		}

		// Analog Left Stick input
		if (!m_contexts[0]->getMute()) // only for Movement Context for now
		{
			MousePos pos({ gamepadState.thumbSticks.leftX, gamepadState.thumbSticks.leftY });
			if (pos.x > -m_centerDeadzoneLeftLimit && pos.x < m_centerDeadzoneLeftLimit) // Check if pos X is within center Deadzone
				pos.x = 0;
				
			if (pos.y > -m_centerDeadzoneLeftLimit && pos.y < m_centerDeadzoneLeftLimit) // Check if pos X is within center Deadzone
				pos.y = 0;
				
			if (pos.x != 0.f || pos.y != 0.f) // only send input if it has changed
			{
				m_currentInputData.rangeData.emplace_back(WALK, pos);
				inputDataChanged = true;
			}
		}

		// Camera
		float posX = gamepadState.thumbSticks.rightX * m_rightStickSensetivity * dt;
		float posY = gamepadState.thumbSticks.rightY * m_rightStickSensetivity * dt;

		if (posX != 0.f || posY != 0.f)
		{
			if (!m_contexts[0]->getMute()) // only for Movement Context for now
			{
				if (!m_invertedRightStickY)
					posY *= -1.f;
				MousePos pos({ posX, posY });
				this->m_currentInputData.rangeData.emplace_back(RAW, pos);
				inputDataChanged = true;
			}
		}

		// Example code
		//if (gamepadState.buttons.y)
			// Do action for button Y being down

		//if (gamepadState.IsDPadLeftPressed())
			// Do action for DPAD Left being down

		//if (gamepadState.dpad.up || gamepadState.dpad.down || gamepadState.dpad.left || gamepadState.dpad.right)
			// Do action based on any DPAD change

		//float posx = gamepadState.thumbSticks.leftX;
		//float posy = gamepadState.thumbSticks.leftY;
			// These values are normalized to -1 to 1

		//float throttle = gamepadState.triggers.right;
			// This value is normalized 0 -> 1

		//if (gamepadState.IsLeftTriggerPressed())
			// Do action based on a left trigger pressed more than halfway
	}
	else
	{
		m_tracker.Reset();
	}

	//Notify
	if (inputDataChanged)
	{
		for (size_t i = 0; i < m_inputObservers.size(); i++)
		{
			m_inputObservers[i]->inputUpdate(m_currentInputData);
		}
	}

	// if Movement Context is muted
	if (m_contexts[0]->getMute())
		m_currentInputData.clearStateData();

}

void Input::addContext(iContext* context)
{
	m_contexts.emplace_back(context);
}

void Input::removeContext(iContext* context)
{
	bool removed = false;
	for (std::vector<int>::size_type i = 0; i < m_contexts.size() && !removed; i++)
	{
		if (m_contexts[i] = context)
		{
			m_contexts.erase(m_contexts.begin() + i);
			removed = true;
		}
	}
}

MouseEvent Input::getMouseEvent()
{
	return m_Mouse.readEvent();
}

Keyboard* Input::getKeyboard()
{
	return &m_Keyboard;
}

Mouse* Input::getMouse()
{
	return &m_Mouse;
}

std::vector<iContext*>* Input::getIContextPtr()
{
	return &m_contexts;
}

void Input::setCursor(bool ifCursorEnabled)
{
	if (ifCursorEnabled != m_cursorEnabled)
	{
		m_cursorEnabled = !m_cursorEnabled;
		if (m_cursorEnabled)
		{
			while (::ShowCursor(TRUE) < 0);
			m_contexts[0]->setMute(true);
		}
		else
		{
			while (::ShowCursor(FALSE) >= 0);
			m_contexts[0]->setMute(false);
		}
	}
}

void Input::Attach(InputObserver* observer)
{

	m_inputObservers.emplace_back(observer);

}

void Input::Detach(InputObserver* observer)
{
	bool detatched = false;
	for (size_t i = 0; i < m_inputObservers.size() && !detatched; i++) {
		if (m_inputObservers[i] == observer)
		{
			m_inputObservers.erase(m_inputObservers.begin() + i);
			detatched = true;
		}
	}
}

