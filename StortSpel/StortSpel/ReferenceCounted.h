#pragma once
#include "3DPCH.h"

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
		if (this && refCount > 0)
			refCount--;

	}

	int getRefCount()
	{
		return refCount;
	}
};