#include"3DPCH.h"
#include"ApplicationLayer.h"

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd)
{
	HRESULT hr = CoInitialize(NULL);

	bool initOK = false;
	ApplicationLayer* g_Application = &ApplicationLayer::getInstance();
	initOK = g_Application->initializeApplication(hInstance, lpCmdLine, g_Application->m_window, nShowCmd);

	if (initOK)
	{
		OutputDebugStringA("Window Created!\n");
		g_Application->applicationLoop();
	}

	return 0;
};
