#include"3DPCH.h"
#include"ApplicationLayer.h"
#include <thread>
#include "PacketHandler.h"
#include "Server.h"

void runClient();
void runServer();
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	
	HRESULT hr = CoInitialize(NULL);

	bool initOK = false;
	ApplicationLayer* g_Application = &ApplicationLayer::getInstance();
	initOK = g_Application->initializeApplication(hInstance, lpCmdLine, g_Application->getWindow(), nShowCmd);

	if (initOK)
	{

		std::thread serverThread(runServer);
		std::thread clientThread(runClient);

		OutputDebugStringA("Window Created!\n");
		g_Application->applicationLoop();
	}

	return 0;
};

using namespace std::literals::chrono_literals;
void runClient()
{
	PacketHandler::get();
	while (true)
	{

		PacketHandler::get().update();
		std::this_thread::sleep_for(0.3ms);

	}
}

void runServer()
{
	Server::get();
	while (true)
	{

		Server::get().update();
		std::this_thread::sleep_for(0.3ms);

	}
}
