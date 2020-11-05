#include "3DPCH.h"
#include "Packet.h"
#include <iostream>

Packet::Packet(int _id)
{
	readPos = 0;
	Write(_id);
}


Packet::Packet(char* data, int length)
{

	readPos = 0;
	for (int i = 0; i < length; i++)
	{
		readableBuffer[i] = data[i];
	}

	this->_ID = ReadInt();
}



void Packet::Write(int _value)
{
	char vOut[4];
	_itoa_s(_value, vOut, sizeof(vOut), 10);
	for (int i = 0; i < 4; i++)
	{
		buffer.push_back(vOut[i]);
	}

}

void Packet::Write(float _value)
{
	char vOut[17];
	_gcvt_s(vOut, sizeof(vOut), _value, 8);
	for (int i = 0; i < 17; i++)
	{
		buffer.push_back(vOut[i]);
	}
}

void Packet::Write(Vector3 position)
{
	Write(position.x);
	Write(position.y);
	Write(position.z);
}

void Packet::Write(Quaternion quaternion)
{
	Write(quaternion.x);
	Write(quaternion.y);
	Write(quaternion.z);
	Write(quaternion.w);
}



int Packet::ReadInt()
{
	char* vIn = new char[4];
	for (int i = 0; i < 4; i++)
	{
		vIn[i] = readableBuffer[readPos++];
	}
	int _value = atoi(vIn);

	return _value;
}

float Packet::ReadFloat()
{
	char* vIn = new char[17];
	for (int i = 0; i < 17; i++)
	{
		vIn[i] = readableBuffer[readPos++];
	}
	float vOut = (float)strtod(vIn, NULL);
	return vOut;
}

XMFLOAT3 Packet::ReadVector3()
{
	XMFLOAT3 returnValue;
	returnValue.x = ReadFloat();
	returnValue.y = ReadFloat();
	returnValue.z = ReadFloat();

	return returnValue;
}

XMFLOAT4 Packet::ReadVector4()
{
	XMFLOAT4 returnValue;
	returnValue.x = ReadFloat();
	returnValue.y = ReadFloat();
	returnValue.z = ReadFloat();
	returnValue.w = ReadFloat();

	return returnValue;
}

char* Packet::ToArray()
{
	for (int i = 0; i < buffer.size(); i++)
	{
		readableBuffer[i] = buffer[i];
	}

	return readableBuffer;
}

int Packet::Lenght()
{
	return buffer.size();
}
