#pragma once
#include "Player.h"
#include "Engine.h"
class Scene
{
private:
	std::unordered_map<std::string, Entity*> m_entities;
	//Entity* m_player;
	//std::vector<

public:
	Scene();
	~Scene();
	void loadScene(std::string path);
	void updateScene(const float &dt);

};