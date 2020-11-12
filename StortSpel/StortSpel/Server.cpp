#include "3DPCH.h"
#include "Server.h"

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

void Server::playerPacket(Packet* _packet)
{
	//loop through players and send data to every other client
	int id = _packet->getID();
	int playerID = _packet->ReadInt();
	float x, y, z, w;
	int state, blend;

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

	_outPacket.Write(state);
	_outPacket.Write(blend);


	sendToAllExcept(&_outPacket);
}

void Server::addTrap()
{
	Trap temp;
	//Is this line correct?
	temp.id = traps.size();

	temp.active = 0;
	traps.push_back(temp);
}

void Server::setTrapActive(int id, bool tf)
{
	traps[id].active = (int)tf;

	//Then send trap packet to all clients
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



				//Send trash message to keep client running and not getting stuck in a loop
				//Packet trash(0);
				//SendToAll(&trash);

			}
		}
	}
	
}
