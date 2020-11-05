#pragma once
#include <vector>
#include "Player.h"
using namespace DirectX;
class Packet
{
private:
	std::vector<char> buffer;
	char readableBuffer[4096];
	int readPos;
	int _ID;

public:
	//enum ServerPackets
	//{
	//	welcome = 1,
	//	playerMovement = 2,
	//	playerRotation = 3

	//};

	//enum ClientPackets
	//{
	//	welcomeReceived = 1,
	//	playerPosition = 2,
	//	playerRotation = 3
	//};


private:


public:
	Packet(int _id);

	//This constructor is bugged, ´parameter only has first value
	Packet(char* data, int length);

	//Writes
	void Write(int _value);
	void Write(float _value);
	void Write(Vector3 position);
	void Write(Quaternion quaternion);

	//Reads
	int ReadInt();
	float ReadFloat();
	XMFLOAT3 ReadVector3();
	XMFLOAT4 ReadVector4();

	//Others
	char* ToArray();
	int Lenght();
	int getID() { return _ID; }

};
#pragma once
