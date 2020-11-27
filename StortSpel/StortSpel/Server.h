#pragma once
#include "Packet.h"
#pragma comment (lib, "ws2_32.lib")
struct PlayerInfo
{
	int id;
	int ready; // 0 is unready, 1 is ready
};
class Server
{
private:
	Server();
	~Server();
	void sendTo(Packet* _packet);
	void sendToAllExcept(Packet* _packet);
	void sendToAll(Packet* _packet);

	void playerPacket(Packet* _packet);
	void trapPacket(Packet* _packet);
	void pickUpPacket(Packet* _packet);
	void startGame();
public:
	Server(const Server&) = delete;
	void operator=(Server const&) = delete;
	static Server& get()
	{
		static Server instance;
		return instance;
	}

	BSTR getServerIP();
	void setNrOfPlayers(int x);
	void update();
private:
	SOCKET client;
	fd_set master;
	SOCKET listening;
	SOCKET sock;
	sockaddr_in hint;
	WSADATA wsData;

	int nrOfPlayers = 0;
	int playersReady = -1;
	PlayerInfo playerInfo[4];

};