#include "3DPCH.h"
#include "Client.h"

void Client::sendPlayerPosition()
{
	//Write Packet ID
	Packet _packet(3);

	//Write Player ID
	_packet.Write(m_LocalPlayer->getNetworkID());

	//Write Player Position
	_packet.Write(m_LocalPlayer->getPlayerEntity()->getTranslation());
	
	//Send Packet
	int sendResult = send(sock, _packet.ToArray(), _packet.Lenght() + 1, 0);
}

void Client::sendPlayerRotation()
{
	//Write Packet ID
	Packet _packet(4);

	//Write Player ID
	_packet.Write(m_LocalPlayer->getNetworkID());

	//Write Player Rotation
	_packet.Write(m_LocalPlayer->getPlayerEntity()->getRotation());

	//Send Packet
	int sendResult = send(sock, _packet.ToArray(), _packet.Lenght() + 1, 0);
}

void Client::packetHandler(Packet* _packet)
{
	int ID = _packet->getID();
	switch (ID)
	{
	case 0:
		//std::cout << "Trash msg received, continuing loop" << std::endl;
		break;
	case 2:
		welcomeReceived(_packet);
		break;

	case 3:
		playerMovement(_packet);
		break;

	case 4:
		playerRotation(_packet);
		break;

	case 5:
		newPlayerConnection(_packet);
		break;
	}
}

void Client::welcomeReceived(Packet* _packet)
{
	//Assigns ID to own client
	std::cout << "You've connected to the server!" << std::endl;
	int ID = _packet->ReadInt();
	m_LocalPlayer->setNetworkID(ID);

	int nrOfPlayers = _packet->ReadInt();
	for (int i = 0; i < nrOfPlayers; i++)
	{
		int networkID = _packet->ReadInt();
		m_serverPlayers->at(i)->setNetworkID(networkID);
	}

}

void Client::playerMovement(Packet* _packet)
{
	//Assigns new position to IDd player
	int ID = _packet->ReadInt();
	Vector3 position = _packet->ReadVector3();
	for (int i = 0; i < 3; i++)
	{
		if (m_serverPlayers->at(i)->getNetworkID() == ID)
		{
			m_serverPlayers->at(i)->getPlayerEntity()->setPosition(position);
			std::cout << "Player at ID " + std::to_string(ID) + " has moved" << std::endl;
		}
	}
	
	std::cout << "Playermovement reached" << std::endl;
	
}

void Client::playerRotation(Packet* _packet)
{
	//Assigns new rotation to IDd player
	int ID = _packet->ReadInt();
	Vector4 rotation = _packet->ReadVector4();
	for (int i = 0; i < 3; i++)
	{
		if (m_serverPlayers->at(i)->getNetworkID() == ID)
		{
			m_serverPlayers->at(i)->getPlayerEntity()->setRotationQuat(rotation);
			std::cout << "Player at ID " + std::to_string(ID) + " has rotated" << std::endl;
		}
	}

	std::cout << "Playerrotation reached" << std::endl;
}

void Client::newPlayerConnection(Packet* _packet)
{
	
	int ID = _packet->ReadInt();
	for (int i = 0; i < 3; i++)
	{
		if (m_serverPlayers->at(i)->getNetworkID() == -1)
		{
			m_serverPlayers->at(i)->setNetworkID(ID);
			std::cout << "New player connected with ID: " + std::to_string(ID) << std::endl;
			break;
		}
	}

}

bool Client::tryConnect()
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

Client::Client(Player* player, std::vector<ServerPlayer*> *serverPlayers)
{
	isConnected = false;

	m_LocalPlayer = player;

	m_serverPlayers = serverPlayers;
	for (int i = 0; i < 3; i++)
	{
		m_serverPlayers->at(i) = serverPlayers->at(i);
	}
	m_serverPlayers->at(0)->getNetworkID();
	ipAddress = "127.0.0.1";
	port = 54000;

	//Connect to server
	isConnected = tryConnect();

	//thread = new std::thread(update);
	
}

void Client::update()
{
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
		packetHandler(&_packet);

	}


	sendPlayerPosition();
	//sendPlayerRotation();


}
