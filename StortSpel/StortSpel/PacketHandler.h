#pragma once
#include "Packet.h"
struct SimpleData
{
	int ID;
	Vector3 pos;
	Vector4 rot;
	int state;
	float blend;
};
class PacketHandler
{
private:
	PacketHandler();

	bool tryConnect();
	void handlePacket(Packet* _packet);

	void welcomeReceived(Packet* _packet);
	void playerData(Packet* _packet);
	void newPlayerConnection(Packet* _packet);

	void sendPlayerData();
public:
	PacketHandler(const PacketHandler&) = delete;
	void operator=(PacketHandler const&) = delete;
	static PacketHandler& get()
	{
		static PacketHandler instance;
		return instance;
	}


	int getIDAt(int i);
	Vector3 getPosAt(int i);
	Vector4 getRotAt(int i);
	int getStateAt(int i);
	float getBlendAt(int i);

	void setPlayerData(Vector3 pos);
	void setPlayerData(Vector4 rot);
	void setPlayerData(int state);
	void setPlayerData(float blend);

	void update();
	
private:
	WSAData data;
	std::string ipAddress;
	int port;
	SOCKET sock;
	sockaddr_in hint;
	bool isConnected;

	SimpleData serverPlayerData[4];



};