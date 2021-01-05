#include "3DPCH.h"
#include "PacketHandler.h"
#include "Engine.h"
#include "Traps.h"
#include "Server.h"
#include <comutil.h>
#include "ApplicationLayer.h"
#pragma comment(lib,"comsuppw.lib")
PacketHandler::PacketHandler()
{
	isConnected = false;

	
	for (int i = 0; i < 4; i++)
	{
		serverPlayerData[i].ID = -1;
		serverPlayerData[i].rot = XMQuaternionIdentity();
		serverPlayerData[i].state = 0;
		serverPlayerData[i].blend = 0;
		serverPlayerData[i].score = 50;
		serverPlayerData[i].scene = 0;
	}

	//ipAddress = "188.148.250.207";
	//ipAddress = convert(Server::get().getServerIP());
	//std::cout << "this is the IP to connect to " << ipAddress << std::endl;
	ipAddress = "127.0.0.0";
	port = 54000;

	//Connect to server
	//isConnected = tryConnect();
}

bool PacketHandler::tryConnect()
{
	//Initialize winsock
	WORD ver = MAKEWORD(2, 2);
	int wsResult = WSAStartup(ver, &data);
	if (wsResult != 0)
	{
		std::cout << "Cant initialize winsock, Err: " << wsResult << std::endl;
		return false;
	}

	//Create socket
	sock = socket(AF_INET, SOCK_STREAM, 0);

	if (sock == INVALID_SOCKET)
	{
		std::cout << "Can't create socket, Err: " << WSAGetLastError() << std::endl;
		WSACleanup();
		return false;
	}

	//Fill hint structure

	hint.sin_family = AF_INET;
	hint.sin_port = htons(port);
	inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);

	int connResult = connect(sock, (sockaddr*)&hint, sizeof(hint));
	if (connResult == SOCKET_ERROR)
	{
		std::cout << "Can't connect to server, Err: " << WSAGetLastError() << std::endl;
		closesocket(sock);
		WSACleanup();
		return false;
	}
	return true;
}

void PacketHandler::handlePacket(Packet* _packet)
{
	//std::cout << "Entered handlePacket() " << std::endl;
	int ID = _packet->getID();
	switch (ID)
	{
	case 1:
		startGame(_packet);
		break;
	case 2:
		welcomeReceived(_packet);
		break;

	case 3:
		playerData(_packet);
		break;

	case 4:
		newPlayerConnection(_packet);
		break;

	case 5:
		trapActivation(_packet);
		break;

	case 6:
		playerPickUp(_packet);
		break;

	case 10:
		arenaPacket(_packet);
	}
}

void PacketHandler::startGame(Packet* _packet)
{
	std::cout << "reached game started boolean" << std::endl;
	serverReady = true;
}

void PacketHandler::welcomeReceived(Packet* _packet)
{
	//Assigns ID to own client
	int ID = _packet->ReadInt();
	serverPlayerData[0].ID = ID;

	int nrOfPlayers = _packet->ReadInt();
	for (int i = 0; i < nrOfPlayers; i++)
	{
		int networkID = _packet->ReadInt();
		serverPlayerData[i+1].ID = networkID;
	}
	std::cout << "You've connected to the server at ID: " << serverPlayerData[0].ID << std::endl;
}

void PacketHandler::playerData(Packet* _packet)
{
	//Assigns new position to IDd player
	int ID = _packet->ReadInt();
	Vector3 position = _packet->ReadVector3();
	Vector4 rotation = _packet->ReadVector4();
	int state = _packet->ReadInt();
	float blend = _packet->ReadFloat();
	int score = _packet->ReadInt();
	int scene = _packet->ReadInt();
	
	for (int i = 1; i < 4; i++)
	{
		if (serverPlayerData[i].ID == ID)
		{

			serverPlayerData[i].pos = position;
			serverPlayerData[i].rot = rotation;
			serverPlayerData[i].state = state;
			serverPlayerData[i].blend = blend;
			serverPlayerData[i].score = score;
			serverPlayerData[i].scene = scene;
			//std::cout << serverPlayerData[i].scene << std::endl;
			//Add a score component that shows the score of all current players
			
			//std::cout << "Player at ID " + std::to_string(ID) + " has moved" << std::endl;
		}
	}

	//std::cout << "Playermovement reached" << std::endl;
}

void PacketHandler::newPlayerConnection(Packet* _packet)
{
	int ID = _packet->ReadInt();
	if (ID != serverPlayerData[0].ID)
	{
		for (int i = 1; i < 4; i++)
		{
			if (serverPlayerData[i].ID == -1)
			{
				serverPlayerData[i].ID = ID;
				std::cout << "New player connected with ID: " + std::to_string(ID) << std::endl;
				break;
			}
		}
	}

}

void PacketHandler::trapActivation(Packet* _packet)
{
	//std::cout << "trap package received" << std::endl;
	int length = _packet->ReadInt();
	std::string entityID = _packet->ReadString(length);

	std::vector<Component*> pushTraps;
	Engine::get().getEntityMap()->at(entityID)->getComponentsOfType(pushTraps, ComponentType::TRIGGER);
	if (pushTraps.size() > 0)
	{
		if ((TrapType)(static_cast<TriggerComponent*>(pushTraps[0])->getAssociatedTriggerEnum()) == TrapType::PUSH)
		{
			static_cast<PushTrapComponent*>(pushTraps[0])->push();
		}
		else
		{
			ApplicationLayer::getInstance().m_scenemanager.m_currentScene->addBarrelDrop(Vector3(-30, 50, 130));
			static_cast<BarrelTriggerComponent*>(pushTraps[0])->m_triggerTimer.restart();
			ApplicationLayer::getInstance().m_scenemanager.m_currentScene->addedBarrel = true;
		}
		
	}
	//else
	//{
	//	Entity* temp = Engine::get().getEntityMap()->at(entityID);
	//	std::vector<Component*> tempVec;
	//	temp->getComponentsOfType(tempVec, ComponentType::TRIGGER);

	//	for (int i = 0; i < tempVec.size(); i++)
	//	{
	//		BarrelTriggerComponent* test = dynamic_cast<BarrelTriggerComponent*>(tempVec[i]);
	//		if (test != nullptr)
	//		{

	//			
	//			ApplicationLayer::getInstance().m_scenemanager.m_currentScene->addBarrelDrop(Vector3(-30, 50, 130));
	//			test->m_triggerTimer.restart();
	//			ApplicationLayer::getInstance().m_scenemanager.m_currentScene->addedBarrel = true;
	//		}
	//	}
	//}

}

void PacketHandler::playerPickUp(Packet* _packet)
{
	int length = _packet->ReadInt();
	std::string entityID = _packet->ReadString(length);

	//Find entity and remove it
	entitiesToBeRemoved.push_back(entityID);
	
}

void PacketHandler::arenaPacket(Packet* _packet)
{
	reachedArena = true;
}

void PacketHandler::sendPlayerData()
{
	//Write Packet ID
	Packet _packet(3);

	//Write Player ID
	_packet.Write(serverPlayerData[0].ID);

	//Write Player Position
	_packet.Write(serverPlayerData[0].pos);

	//Write Player Rotation
	_packet.Write(serverPlayerData[0].rot);

	//Write Player State
	_packet.Write(serverPlayerData[0].state);

	//Write Player Animation Blend
	_packet.Write(serverPlayerData[0].blend);

	//Write Player Score
	_packet.Write(serverPlayerData[0].score);

	//Write Player Scene
	_packet.Write(serverPlayerData[0].scene);

	//Send Packet
	int sendResult = send(sock, _packet.ToArray(), _packet.Lenght() + 1, 0);
}

void PacketHandler::sendTrapData(std::string entityID)
{
	
	Packet _packet(5);
	//add int for trap enum
	_packet.Write(entityID);

	int sendResult = send(sock, _packet.ToArray(), _packet.Lenght() + 1, 0);
}

void PacketHandler::sendScorePickup(std::string entityID)
{
	Packet _packet(6);
	_packet.Write(entityID);

	int sendResult = send(sock, _packet.ToArray(), _packet.Lenght() + 1, 0);

}

void PacketHandler::sendReady()
{
	clientReady = true;
	Packet _packet(1);
	int sendResult = send(sock, _packet.ToArray(), _packet.Lenght() + 1, 0);
}

void PacketHandler::sendArenaPacket()
{
	Packet _packet(10);
	int sendResult = send(sock, _packet.ToArray(), _packet.Lenght() + 1, 0);
}

int PacketHandler::getIDAt(int i)
{
	return serverPlayerData[i].ID;
}

Vector3 PacketHandler::getPosAt(int i)
{
	return serverPlayerData[i].pos;
}

Vector4 PacketHandler::getRotAt(int i)
{
	return serverPlayerData[i].rot;
}

int PacketHandler::getStateAt(int i)
{
	return this->serverPlayerData[i].state;
}

float PacketHandler::getBlendAt(int i)
{
	return this->serverPlayerData[i].blend;
}

int PacketHandler::getScoreAt(int i)
{
	return this->serverPlayerData[i].score;
}

int PacketHandler::getSceneAt(int i)
{
	return this->serverPlayerData[i].scene;
}

void PacketHandler::setPlayerData(Vector3 pos)
{
	this->serverPlayerData[0].pos = pos;
}

void PacketHandler::setPlayerData(Vector4 rot)
{
	this->serverPlayerData[0].rot = rot;
}

void PacketHandler::setPlayerState(int state)
{
	this->serverPlayerData[0].state = state;
}

void PacketHandler::setPlayerData(float blend)
{
	this->serverPlayerData[0].blend = blend;
}

void PacketHandler::setPlayerScore(int score)
{
	this->serverPlayerData[0].score = score;
}

void PacketHandler::setPlayerScene(int scene)
{
	this->serverPlayerData[0].scene = scene;
}


bool PacketHandler::getStarted()
{
	//std::cout << "getting boolean" << std::endl;
	return serverReady;
}

bool PacketHandler::getArena()
{
	bool returnvalue = reachedArena;
	reachedArena = false;
	return returnvalue;
}

std::vector<TrapData>& PacketHandler::getTrapData()
{
	return this->trapData;
}

std::vector<std::string>& PacketHandler::getEntitiesToBeRemoved()
{
	return this->entitiesToBeRemoved;
}

void PacketHandler::update()
{

	//std::cout << "you are updating client, good job" << std::endl;
	if (!isConnected)
	{
		isConnected = tryConnect();
	}
	char buf[4096];

	ZeroMemory(buf, 4096);
	int bytesReceived = recv(sock, buf, 4096, 0);
	if (bytesReceived > 0)
	{
		Packet _packet(buf, bytesReceived);
		handlePacket(&_packet);

	}

	//Send our packet to server
	sendPlayerData();
}

std::string PacketHandler::convert(BSTR source)
{
		//source = L"lol2inside";
		_bstr_t wrapped_bstr = _bstr_t(source);
		int length = wrapped_bstr.length();
		char* char_array = new char[length];
		strcpy_s(char_array, length + 1, wrapped_bstr);
		return char_array;
	
}
