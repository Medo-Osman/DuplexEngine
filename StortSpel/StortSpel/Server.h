#pragma once
#include "Packet.h"
#pragma comment (lib, "ws2_32.lib")
struct Trap
{
	int id;
	int active; //0 is inactive, 1 is active;
};
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
public:
	Server(const Server&) = delete;
	void operator=(Server const&) = delete;
	static Server& get()
	{
		static Server instance;
		return instance;
	}

	void addTrap();
	void setTrapActive(int id, bool tf);
	void update();
private:
	SOCKET client;
	fd_set master;
	SOCKET listening;
	SOCKET sock;
	sockaddr_in hint;
	WSADATA wsData;

	PlayerInfo playerInfo[4];

	std::vector<Trap> traps;
};