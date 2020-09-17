#include "3DPCH.h"
#ifndef ENITY_H
#define ENITY_H

class Enity
{
private:
	XMFLOAT3 m_scale;
	XMFLOAT3 m_rotation;
	XMFLOAT3 m_rotation;



public:
	Enity()
	{
		m_scale = { 1.f, 1.f, 1.f };
		m_rotation = { 0.f, 0.f, 0.f };
		m_rotation = { 0.f, 0.f, 0.f };
	}
	~Enity()
	{

	}
};

#endif // !ENITY_H