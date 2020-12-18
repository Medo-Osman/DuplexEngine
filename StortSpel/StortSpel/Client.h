#include <iostream>
#include <string>
#include <WS2tcpip.h>
#include <vector>
#include "Packet.h"
#include "Player.h"
#include <cstring>
#include "ServerPlayer.h"
#include <thread>

#pragma comment(lib, "ws2_32.lib")

class Client
{
private:
	WSADATA data;
	std::string ipAddress;
	int port;
	SOCKET sock;
	sockaddr_in hint;
	bool isConnected;

	Player* m_LocalPlayer;
	std::vector<ServerPlayer*> *m_serverPlayers;
	void sendPlayerPosition();
	void sendPlayerRotation();

	void packetHandler(Packet* _packet);

	void welcomeReceived(Packet* _packet);
	void playerMovement(Packet* _packet);
	void playerRotation(Packet* _packet);
	void newPlayerConnection(Packet* _packet);

	bool tryConnect();
public:
	Client(Player* player, std::vector<ServerPlayer*> *serverPlayers);

	void update();
};
