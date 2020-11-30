#pragma once
#include "Packet.h"
struct SimpleData
{
	int ID;
	Vector3 pos;
	Vector4 rot;
	int state;
	float blend;
	int score;
};
struct TrapData
{
	std::string entityID;
	bool isActive;
};
class PacketHandler
{
private:
	PacketHandler();

	bool tryConnect();
	void handlePacket(Packet* _packet);

	void startGame(Packet* _packet);
	void welcomeReceived(Packet* _packet);
	void playerData(Packet* _packet);
	void newPlayerConnection(Packet* _packet);
	void trapActivation(Packet* _packet);
	void playerPickUp(Packet* _packet);

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
	void setPlayerState(int state);
	void setPlayerData(float blend);
	void setPlayerScore(int score);

	void sendTrapData(std::string entityID);
	void sendScorePickup(std::string entityID);
	void sendReady();


	bool getServerReady() { return serverReady; }
	bool getClientReady() { return clientReady; }
	bool getStarted();
	std::vector<TrapData>& getTrapData();
	std::vector<std::string>& getEntitiesToBeRemoved();
	void update();
	
	std::string convert(BSTR source);
private:
	WSAData data;
	std::string ipAddress;
	int port;
	SOCKET sock;
	sockaddr_in hint;
	bool isConnected;
	//bool gameStarted = false;
	bool serverReady = false;
	bool clientReady = false;
	SimpleData serverPlayerData[4];
	std::vector<TrapData> trapData;
	std::vector<std::string> entitiesToBeRemoved;


};