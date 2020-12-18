#include"3DPCH.h"
#include"ApplicationLayer.h"
#include <thread>



int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	
	HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	bool initOK = false;
	ApplicationLayer* g_Application = &ApplicationLayer::getInstance();
	initOK = g_Application->initializeApplication(hInstance, lpCmdLine, g_Application->getWindow(), nShowCmd);

	if (initOK)
	{

		OutputDebugStringA("Window Created!\n");
		g_Application->applicationLoop();
	}

	//Renderer::get().printLiveObject();

	return 0;
};


