#pragma once
#include"Physics.h"
#include"Input.h"
#include"Renderer.h"
#include "Engine.h"
#include"AudioHandler.h"
#include"Timer.h"
#include "SceneManager.h"

#include <windows.h>
#include <fcntl.h>
#include <io.h>
#include <iostream>


class ApplicationLayer
{
private:
	ApplicationLayer();

	Renderer* m_rendererPtr;
	HWND m_window;
	Engine* m_enginePtr;
	
	Timer m_timer;
	float m_dt;
	float m_gameTime = 0;
	FILE* m_consoleFile;

	int m_width, m_height;

	void createWin32Window(const HINSTANCE hInstance, const wchar_t* windowTitle, HWND& _d3d11Window);

	void RedirectIOToConsole();

	
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
	Physics* m_physics;
	const float& getGameTime()
	{
		return m_gameTime;
	}

	SceneManager m_scenemanager;
};
