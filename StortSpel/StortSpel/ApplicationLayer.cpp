#include"3DPCH.h"
#include"ApplicationLayer.h"


LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

ApplicationLayer::ApplicationLayer()
{
	m_graphicEnginePtr = nullptr;
	m_window = 0;
	this->width = 800;
	this->height = 800;
	m_time = 0.f;

}

ApplicationLayer::~ApplicationLayer()
{

}

bool ApplicationLayer::initializeApplication(const HINSTANCE& hInstance, const LPWSTR& lpCmdLine, HWND hWnd, const int& showCmd)
{
	if (hWnd != NULL) return true;
	const wchar_t WINDOWTILE[] = L"3DProject";
	HRESULT hr = 0;
	bool initOK = false;

	SetCursor(NULL);
	this->createWin32Window(hInstance, WINDOWTILE, hWnd);// hwnd is refference, is set to created window.
	m_window = hWnd;

	AudioHandler::get().initialize(m_window);

	RAWINPUTDEVICE rawIDevice;
	rawIDevice.usUsagePage = 0x01;
	rawIDevice.usUsage = 0x02;
	rawIDevice.dwFlags = 0;
	rawIDevice.hwndTarget = NULL;
	if (RegisterRawInputDevices(&rawIDevice, 1, sizeof(rawIDevice)) == FALSE)
		return false;
	m_graphicEnginePtr = &Renderer::get();//new Renderer();
	hr = m_graphicEnginePtr->initialize(m_window);
	if (SUCCEEDED(hr))
	{
		initOK = true;
		ShowWindow(m_window, showCmd);
	}
	Engine::get().initialize();
	m_enginePtr = &Engine::get();

	srand(static_cast <unsigned> (time(0)));

	//PhysX
	m_physics.init(XMFLOAT3(0.0f, -9.81f, 0.0f), 1);

	return initOK;
}

void ApplicationLayer::createWin32Window(const HINSTANCE hInstance, const wchar_t* windowTitle, HWND& _d3d11Window)
{
	WNDCLASS wc = { };

	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = windowTitle;

	RegisterClass(&wc);

	RECT windowRect;
	windowRect.left = 20;
	windowRect.right = windowRect.left + this->width;
	windowRect.top = 20;
	windowRect.bottom = windowRect.top + this->height;
	AdjustWindowRect(&windowRect, NULL, false);

	// Create the window.
	_d3d11Window = CreateWindowEx(
		0,                          // Optional window styles.
		windowTitle,                 // Window class
		windowTitle,				// Window text
		WS_OVERLAPPEDWINDOW,        // Window style
		windowRect.left,				// Position, X
		windowRect.top,				// Position, Y
		this->width,	// Width
		this->height,	// Height
		NULL,						// Parent window
		NULL,						// Menu
		hInstance,					// Instance handle
		NULL						// Additional application data
	);
	assert(_d3d11Window);
}

void ApplicationLayer::applicationLoop()
{
	float dt = 0.001f; //DT
	MSG msg = { };
	while (WM_QUIT != msg.message)
	{

		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) // Message loop
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else // Render/Logic Loop
		{
			m_input.readBuffers();
			m_enginePtr->update(dt);
			m_physics.update(dt);
			AudioHandler::get().update();
			m_graphicEnginePtr->update(dt);
			m_graphicEnginePtr->render();

		}
	}
	m_physics.release();
}



LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	ApplicationLayer* g_ApplicationLayer = &ApplicationLayer::getInstance();
	g_ApplicationLayer->m_input.handleMessages(hwnd, uMsg, wParam, lParam);
	switch (uMsg)
	{
	case WM_DEVICECHANGE:
		if (wParam == DBT_DEVICEARRIVAL)
		{
			auto pDev = reinterpret_cast<PDEV_BROADCAST_HDR>(lParam);
			if (pDev)
			{
				if (pDev->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE)
				{
					auto pInter = reinterpret_cast<
						const PDEV_BROADCAST_DEVICEINTERFACE>(pDev);
					if (pInter->dbcc_classguid == KSCATEGORY_AUDIO)
					{
						AudioHandler::get().onNewAudioDevice();
					}
				}
			}
		}
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_PAINT:
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);

		FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

		EndPaint(hwnd, &ps);
		break;

	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
