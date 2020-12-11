#pragma once
#include"3DPCH.h"

enum class BoundingVolumeTypes
{
	FRUSTUM,
	BOX,
	ORIENTEDBOX,
	SPHERE,
};

class BoundingVolumeHolder
{
private:
	BoundingVolumeTypes m_type;
	void* m_boundingVolumePtr;

public:
	BoundingVolumeHolder(const BoundingVolumeTypes& type, void* boundingVolumePtr)
	{
		m_type = type;
		m_boundingVolumePtr = boundingVolumePtr;
	}
	void* getBoundingVolumePtr()
	{
		return m_boundingVolumePtr;
	}

	const BoundingVolumeTypes& getBoundingVolumeType() const
	{
		return m_type;
	}
};