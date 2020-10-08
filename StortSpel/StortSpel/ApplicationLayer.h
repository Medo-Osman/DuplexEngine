#pragma once
#include"Physics.h"
#include"Input.h"
#include"Renderer.h"
#include "Engine.h"

class ApplicationLayer
{
private:
	ApplicationLayer();
	Renderer* m_graphicEnginePtr;
	HWND m_window;
	Engine* m_enginePtr;

	float m_time;

	int width, height;

	void createWin32Window(const HINSTANCE hInstance, const wchar_t* windowTitle, HWND& _d3d11Window);
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
	void applicationLoop();

	const HWND& getWindow() { return m_window; }
	Input m_input;
	Physics m_physics;

};
