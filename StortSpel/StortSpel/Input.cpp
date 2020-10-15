#include"3DPCH.h"
#include"Input.h"

bool Input::fillInputDataUsingKey(const char& key, const bool& wasPressed, const bool& isMouse)
{
	bool foundContext = false;
	for (std::vector<int>::size_type i = 0; i < m_contexts.size() && !foundContext; i++) {
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
		if (key == VK_ESCAPE)
			uMsg = WM_DESTROY;

		if (key == VK_TAB)
		{
			m_contexts[0]->setMute(!m_contexts[0]->getMute());
			m_cursorEnabled = !m_cursorEnabled;
			ShowCursor(CURSOR_SHOWING);
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
		mPos.x = LOWORD(lParam);
		mPos.y = HIWORD(lParam);

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
		UINT size;
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
					mPos.x = rInput->data.mouse.lLastX;
					mPos.y = rInput->data.mouse.lLastY;

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

	}

	return 0;
}

void Input::readBuffers()
{
	bool inputDataChanged = false;
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
			for (std::vector<int>::size_type i = 0; i < m_contexts.size() && !foundContext; i++) 
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

	//Notify
	if (inputDataChanged)
	{
		for (std::vector<int>::size_type i = 0; i < m_inputObservers.size(); i++) 
		{
			m_inputObservers[i]->inputUpdate(m_currentInputData);
		}
	}

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



void Input::Attach(InputObserver* observer)
{

	m_inputObservers.emplace_back(observer);

}

void Input::Detach(InputObserver* observer)
{
	bool detatched = false;
	for (std::vector<int>::size_type i = 0; i < m_inputObservers.size() || !detatched; i++) {
		if (m_inputObservers[i] == observer)
			m_inputObservers.erase(m_inputObservers.begin() + i);
	}
}

