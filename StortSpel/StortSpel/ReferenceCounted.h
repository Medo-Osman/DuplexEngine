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
		//std::cout << "Addedref to: " << debugName << ", " << refCount <<  std::endl;
		refCount++;
	}

	void deleteRef()
	{

		if (this)
		{
			//std::cout << "Removed ref to: " << debugName << ", " << refCount << std::endl;
			refCount--;
		}

	}

	int getRefCount()
	{
		return refCount;
	}
};