#pragma once
#include "3DPCH.h"

class Engine
{

private:

	// Entities
	std::unordered_map<std::string, Entity*> m_entities;

public:

	void Init();

};

