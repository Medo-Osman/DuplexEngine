#include"3DPCH.h"
#include"ApplicationLayer.h"

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

ApplicationLayer::ApplicationLayer()
{
	m_rendererPtr = nullptr;
	m_window = 0;
	this->width = 1920;
	this->height = 1080;
	m_dt = 0.f;

}

ApplicationLayer::~ApplicationLayer()
{
	std::cout << "Memory upon shutdown: " << std::endl;
}

bool ApplicationLayer::initializeApplication(const HINSTANCE& hInstance, const LPWSTR& lpCmdLine, HWND hWnd, const int& showCmd)
{
	if (hWnd != NULL) return true;
	const wchar_t WINDOWTILE[] = L"3DProject";
	HRESULT hr = 0;
	bool initOK = false;
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);
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
	m_rendererPtr = &Renderer::get();//new Renderer();
	hr = m_rendererPtr->initialize(m_window);
	if (SUCCEEDED(hr))
	{
		initOK = true;
		ShowWindow(m_window, showCmd);
	}
	//PhysX
	m_physics = &Physics::get();
	m_physics->init(XMFLOAT3(0.0f, -9.81f, 0.0f), 1);

	Engine::get().initialize();
	m_enginePtr = &Engine::get();

	m_scenemanager.initalize();
	ApplicationLayer::getInstance().m_input.Attach(&m_scenemanager);

	srand(static_cast <unsigned> (time(0)));

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
		(float)this->width,	// Width
		(float)this->height,	// Height
		NULL,						// Parent window
		NULL,						// Menu
		hInstance,					// Instance handle
		NULL						// Additional application data
	);
	assert(_d3d11Window);

	RedirectIOToConsole(); // Disabled For PlayTest
}

void ApplicationLayer::RedirectIOToConsole()
{
	AllocConsole();
	HANDLE stdHandle;
	int hConsole;
	FILE* fp;
	stdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	hConsole = _open_osfhandle((long)stdHandle, _O_TEXT);
	fp = _fdopen(hConsole, "w");

	freopen_s(&fp, "CONOUT$", "w", stdout);
}

void ApplicationLayer::applicationLoop()
{
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

			this->m_dt = (float)m_timer.timeElapsed();
			m_gameTime += m_dt;
			m_timer.restart();

			ImGui_ImplDX11_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			m_input.readBuffers();
			m_physics->update(m_dt);
			m_enginePtr->update(m_dt);
			PerformanceTester::get().runPerformanceTestsGui(m_dt);
			m_scenemanager.updateScene(m_dt);
			AudioHandler::get().update(m_dt);
			m_rendererPtr->update(m_dt);
			m_rendererPtr->render();
		}
	}
	m_physics->release();
}


extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam))
		return true;

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
