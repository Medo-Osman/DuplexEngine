#pragma once
#include <unordered_map>
#include "Entity.h"

class Engine
{

private:

	// Entities
	std::unordered_map<std::string, Entity*> m_entities;
	Engine() {};	
public:
	static Engine& get();
	Entity* getEntity(std::string key);
	bool addEntity(std::string identifier);

	void Init();

};

