#pragma once
#include <string>

template <typename T>
inline T readDataFromChar(char* data, int& offset)
{
	T readData; // ask if it is bad to init a var and return it everytime like this, maybe returning a refrence is better.
	memcpy(&readData, data + offset, sizeof(T));
	offset += sizeof(T);

	return readData;
}

inline std::string readStringFromChar(char* data, int& offset)
{
	std::string strData;

	int strSize;
	memcpy(&strSize, data + offset, sizeof(int));
	offset += sizeof(int);

	if (strSize == 2)
		return "";

	char* rawData = new char[strSize];
	memcpy(rawData, data + offset, strSize);
	offset += strSize;

	strData = std::string(rawData);

	delete[] rawData;

	return strData;
}

#define readStringFromChar1(data, offset)  std::string(); int strSize; memcpy(&strSize, data + offset, sizeof(int)); offset += sizeof(int); char* rawData = new char[strSize]; memcpy(rawData, data + offset, strSize); offset += strSize; strData = std::string(rawData); delete[] rawData;  