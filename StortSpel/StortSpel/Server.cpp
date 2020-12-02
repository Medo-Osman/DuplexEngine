#include "3DPCH.h"
#include "Server.h"
#include <windows.h>
#include <wbemidl.h>
#pragma comment(lib,"wbemuuid.lib")
Server::Server()
{

	WORD ver = MAKEWORD(2, 2);

	int wsOk = WSAStartup(ver, &wsData);
	if (wsOk != 0)
	{
		std::cout << "Can't initialize winsock" << std::endl;
		return;
	}

	//Create socket

	listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET)
	{
		std::cout << "Can't create socket" << std::endl;
		return;
	}

	//Bind socket

	
	hint.sin_family = AF_INET;
	hint.sin_port = htons(54000);
	hint.sin_addr.S_un.S_addr = INADDR_ANY;

	bind(listening, (sockaddr*)&hint, sizeof(hint));

	//Make socket listen
	listen(listening, SOMAXCONN);


	FD_ZERO(&master);
	FD_SET(listening, &master);

	std::cout << "Server is up and running" << std::endl;

	for (int i = 0; i < 4; i++)
	{
		playerInfo[i].id = -1;
	}
}

Server::~Server()
{
	FD_CLR(listening, &master);
	closesocket(listening);

	// Message to let users know what's happening.
	std::string msg = "Server is shutting down. Goodbye\r\n";

	while (master.fd_count > 0)
	{
		// Get the socket number
		SOCKET sock = master.fd_array[0];

		// Send the goodbye message
		send(sock, msg.c_str(), msg.size() + 1, 0);

		// Remove it from the master file list and close the socket
		FD_CLR(sock, &master);
		closesocket(sock);
	}

	// Cleanup winsock
	WSACleanup();
}

void Server::sendTo(Packet* _packet)
{
	send(client, _packet->ToArray(), _packet->Lenght() + 1, 0);
}

void Server::sendToAllExcept(Packet* _packet)
{
	for (int i = 0; i < master.fd_count; i++)
	{
		SOCKET outSock = master.fd_array[i];
		if (outSock != listening && outSock != sock)
		{
			send(outSock, _packet->ToArray(), _packet->Lenght() + 1, 0);
		}
	}
}

void Server::sendToAll(Packet* _packet)
{
	for (int i = 0; i < master.fd_count; i++)
	{
		SOCKET outSock = master.fd_array[i];

		send(outSock, _packet->ToArray(), _packet->Lenght() + 1, 0);

	}
}

void Server::sendBossData()
{
	Packet _packet(7);
	_packet.Write(bossData.position);
	_packet.Write(bossData.rotation);
	sendToAll(&_packet);
}

void Server::playerPacket(Packet* _packet)
{
	//loop through players and send data to every other client
	int id = _packet->getID();
	int playerID = _packet->ReadInt();
	float x, y, z, w;
	int state, blend, score;

	Packet _outPacket(id);


	x = _packet->ReadFloat();
	y = _packet->ReadFloat();
	z = _packet->ReadFloat();


	_outPacket.Write(playerID);
	_outPacket.Write(x);
	_outPacket.Write(y);
	_outPacket.Write(z);

	x = _packet->ReadFloat();
	y = _packet->ReadFloat();
	z = _packet->ReadFloat();
	w = _packet->ReadFloat();

	_outPacket.Write(x);
	_outPacket.Write(y);
	_outPacket.Write(z);
	_outPacket.Write(w);

	state = _packet->ReadInt();
	blend = _packet->ReadFloat();
	score = _packet->ReadInt();

	_outPacket.Write(state);
	_outPacket.Write(blend);
	_outPacket.Write(score);

	sendToAllExcept(&_outPacket);
}

void Server::trapPacket(Packet* _packet)
{
	int length = _packet->ReadInt();
	std::string entityID = _packet->ReadString(length);

	Packet _outPacket(5);
	//_outPacket.Write(length);
	_outPacket.Write(entityID);

	sendToAllExcept(&_outPacket);
}

void Server::pickUpPacket(Packet* _packet)
{
	int length = _packet->ReadInt();
	std::string entityID = _packet->ReadString(length);

	Packet _outPacket(6);
	_outPacket.Write(entityID);

	sendToAllExcept(&_outPacket);
}
void Server::startGame()
{
	Packet _packet(1);
	sendToAll(&_packet);
}
using namespace std;
BSTR Server::getServerIP()
{
	CoInitializeEx(0, 0);
	CoInitializeSecurity(0, -1, 0, 0, 0, 3, 0, 0, 0);

	IWbemLocator* locator = 0;

	CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (void**)&locator);

	IWbemServices* services = 0;
	const wchar_t* name = L"root\\cimv2";

	if (SUCCEEDED(locator->ConnectServer((BSTR)name, 0, 0, 0, 0, 0, 0, &services)))
	{
		CoSetProxyBlanket(services, 10, 0, 0, 3, 3, 0, 0);

		const wchar_t* language = L"WQL";
		const wchar_t* query = L"SELECT * from Win32_NetworkAdapterConfiguration where IPEnabled ='TRUE'";

		IEnumWbemClassObject* e = 0;

		if (SUCCEEDED(services->ExecQuery(
			(BSTR)language,
			(BSTR)query,
			WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY
			,
			0,
			&e)))
		{
			printf("Query OK!\n");

			IWbemClassObject* object = 0;
			ULONG u = 0;

			//lets enumerate all data from this table
			while (e)
			{
				e->Next(WBEM_INFINITE, 1, &object, &u);
				if (!u) break;//no more data,end enumeration
				VARIANT data;

				//Default gateway (Router)
				if (SUCCEEDED(object->Get(L" ", 0, &data, 0, 0)))
				{
					printf("%d\n", data.vt);
					SAFEARRAY* safe = NULL;
					safe = V_ARRAY(&data);
					BSTR element;
					long i = 0;
					SafeArrayGetElement(safe, &i, (void*)&element);
					wcout << L"Gateway: " << element << endl;

				}

				//Ip address
				if (SUCCEEDED(object->Get(L"IpAddress", 0, &data, 0, 0)))
				{
					return ((BSTR*)(data.parray->pvData))[0];
				}


				VariantClear(&data);
			}
		}
		else
		{
			printf("Error executing query!\n");
		}
	}
	else
	{
		printf("Connection error!\n");
	}

	//close all used data
	services->Release();
	locator->Release();
	CoUninitialize();
}

void Server::setNrOfPlayers(int x)
{
	this->nrOfPlayers = x;
}

void Server::setBossData(Vector3 pos, Vector4 rot)
{
	bossData.position = pos;
	bossData.rotation = rot;
}

void Server::sendBossActionData(int bossEnum, BossStructures::BossActionData actionData)
{
	Packet _packet(8);
	_packet.Write(bossEnum);
	_packet.Write(actionData.direction);
	_packet.Write(actionData.origin);
	_packet.Write(actionData.rotation);
	_packet.Write(actionData.speed);
	_packet.Write(actionData.maxStarCount);
	_packet.Write(actionData.currentStarCount);
	_packet.Write(actionData.entityID);
}

void Server::update()
{

	fd_set copy = master;
	int socketCount = select(0, &copy, nullptr, nullptr, nullptr);

	for (int i = 0; i < socketCount; i++)
	{
		sock = copy.fd_array[i];
		if (sock == listening)
		{
			//Accept connection
			client = accept(listening, nullptr, nullptr);

			FD_SET(client, &master);

			Packet _packet(2);

			int newPlayerID = -1;
			int currentPlayers = 0;
			for (int j = 0; j < 4; j++)
			{
				if (playerInfo[j].id == -1)
				{
					newPlayerID = j;
					playerInfo[j].id = j;
					_packet.Write(newPlayerID);
					break;
				}

			}
			for (int j = 0; j < 4; j++)
			{
				if (playerInfo[j].id != -1 && j != newPlayerID)
				{
					currentPlayers++;

				}
			}

			_packet.Write(currentPlayers);

			for (int j = 0; j < 4; j++)
			{
				if (playerInfo[j].id != -1 && j != newPlayerID)
				{
					_packet.Write(j);

				}
			}

			sendTo(&_packet);


			Packet _newPlayerPacket(4);
			_newPlayerPacket.Write(newPlayerID);
			sendToAllExcept(&_newPlayerPacket);
			std::cout << "Client Connected with ID: " + std::to_string(newPlayerID) << std::endl;
		}
		else
		{
			//Accept message
			char buf[4096];
			ZeroMemory(buf, 4096);
			int bytesReceived = recv(sock, buf, 4096, 0);

			if (bytesReceived <= 0)
			{
				//Drop Client, ADD DISCONNECT HERE LATER
				closesocket(sock);
				FD_CLR(sock, &master);
			}
			else
			{
				//Handle package
				Packet _packet(buf, bytesReceived);
				if (_packet.getID() == 3)
				{
					playerPacket(&_packet);
				}
				else if (_packet.getID() == 5)
				{
					
					trapPacket(&_packet);
				}
				else if (_packet.getID() == 6)
				{
					pickUpPacket(&_packet);
				}
				else if (_packet.getID() == 1)
				{
					if (playersReady == -1)
					{
						playersReady++;
					}
					playersReady++;
				}



				//Send trash message to keep client running and not getting stuck in a loop
				//Packet trash(0);
				//SendToAll(&trash);

			}
			if (nrOfPlayers == playersReady && !gameRunning)
			{
				Packet _packet(1);
				sendToAll(&_packet);
				gameRunning = true;
			}

			sendBossData();
			
		}
	}
	
}
