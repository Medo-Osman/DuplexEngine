#pragma once
#include "3DPCH.h"
#include <mutex>

class ReferenceCounted
{
protected:
	UINT refCount = 0;
public:
	std::string debugName = "";
	void addRef()
	{
		refCount++;
	}

	void deleteRef()
	{

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