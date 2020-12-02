#pragma once
#include "3DPCH.h"
#include "ReferenceCounted.h"

class AudioResource : public ReferenceCounted
{
public:
	SoundEffect* audio = nullptr;
	bool m_doReferenceCount = false;

	~AudioResource()
	{
		delete audio;
	}
};