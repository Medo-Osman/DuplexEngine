#pragma once
#include"Input.h"
#include"GraphicEngine.h"
#include"Physics.h"

class ApplicationLayer
{
private:
	ApplicationLayer();
	GraphicEngine* m_graphicEnginePtr;

	float m_time;

	int width, height;
public:
	static ApplicationLayer& getInstance()
	{
		static ApplicationLayer instance;
		return instance;
	}
	ApplicationLayer(ApplicationLayer const&) = delete;
	void operator=(ApplicationLayer const&) = delete;
	~ApplicationLayer();

	bool initializeApplication(const HINSTANCE& hInstance, const LPWSTR& lpCmdLine, HWND hWnd, const int& showCmd);
	void createWin32Window(const HINSTANCE hInstance, const wchar_t* windowTitle, HWND& _d3d11Window);
	void applicationLoop();

	Physics m_physics;

	Input m_input;
	HWND m_window;


};