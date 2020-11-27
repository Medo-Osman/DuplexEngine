#pragma once
#include "3DPCH.h"
#include <mutex>

class ReferenceCounted
{
protected:

	//std::mutex referenceLock;
	UINT refCount = 0;
public:
	std::string debugName = "";
	void addRef()
	{
		//std::lock_guard<std::mutex> guard(referenceLock);
		refCount++;
	}

	void deleteRef()
	{
		//std::lock_guard<std::mutex> guard(referenceLock);
		if (this)
		{
			refCount--;
		}

	}

	int getRefCount()
	{
		return refCount;
	}
};