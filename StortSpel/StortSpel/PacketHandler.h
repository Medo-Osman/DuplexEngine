#pragma once
#include "Packet.h"
#include "BaseAction.h"

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
struct BossData
{
	Vector3 position;
	int x, y;
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
	void setBossData(Packet* _packet);
	void setBossActionData(Packet* _packet);
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
	int getScoreAt(int i);

	void setPlayerData(Vector3 pos);
	void setPlayerData(Vector4 rot);
	void setPlayerState(int state);
	void setPlayerData(float blend);
	void setPlayerScore(int score);

	void sendTrapData(std::string entityID);
	void sendScorePickup(std::string entityID);
	void sendReady();

	Vector3 getBossPosition() { return this->bossData.position; }
	int getBossPIX() { return this->bossData.x; }
	int getBossPIY() { return this->bossData.y; }

	bool getServerReady() { return serverReady; }
	bool getClientReady() { return clientReady; }
	bool getStarted();
	std::vector<TrapData>& getTrapData();
	std::vector<std::string>& getEntitiesToBeRemoved();
	void update();
	
	std::string convert(BSTR source);

	std::vector<BossStructures::BossActionData>& getBossActionData();
	bool gameStarted = false;
	std::vector<int> bossEnum;
	bool hasDoneAction = true;
private:
	WSAData data;
	std::string ipAddress;
	int port;
	SOCKET sock;
	sockaddr_in hint;
	bool isConnected;
	bool serverReady = false;
	bool clientReady = false;
	SimpleData serverPlayerData[4];
	std::vector<TrapData> trapData;
	std::vector<std::string> entitiesToBeRemoved;
	BossData bossData;
	std::vector<BossStructures::BossActionData> bossActionData;

	

};