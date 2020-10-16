#pragma once
#include "Player.h"
#include "Engine.h"
class Scene
{
private:
	std::unordered_map<std::string, Entity*> m_entities;
	//Entity* m_player;
	//std::vector<
	int nrOfParisWheels = 0;
	void createParisWheel(Engine*& engine, Vector3 position, float rotation, float rotationSpeed = 20, int nrOfPlatforms = 4);
	int nrOfFlippingPlatforms = 0;
	void createFlippingPlatform(Engine*& engine, Vector3 position, float upTime = 3, float downTime = 3);
	int nrOfStaticPlatforms = 0;
	void createStaticPlatform(Engine*& engine, Vector3 position, float rotation, std::string meshName);

public:
	Scene();
	~Scene();
	void loadScene(std::string path);
	void updateScene(const float &dt);
	
};