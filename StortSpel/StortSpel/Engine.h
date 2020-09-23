#pragma once
#include <unordered_map>
#include "Entity.h"
#include "Player.h"
class Engine
{

private:

	// Entities
	std::unordered_map<std::string, Entity*> m_entities;
	Engine() {};
	Player* m_player = nullptr;
public:
	static Engine& get();
	Entity* getEntity(std::string key);
	bool addEntity(std::string identifier);
	~Engine();
	
	void update(Mouse* mousePtr, Keyboard* keyboardPtr, const float &dt);
	void Init();
};

