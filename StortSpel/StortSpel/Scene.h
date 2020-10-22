#pragma once
#include "Player.h"
#include "Engine.h"
class Scene
{
private:
	//std::unordered_map<std::string, Entity*> m_entities;
	//Entity* m_player;
	//std::vector<
	int nrOfParisWheels = 0;
	void createParisWheel(Vector3 position, float rotation, float rotationSpeed = 20, int nrOfPlatforms = 4);
	int nrOfFlippingPlatforms = 0;
	void createFlippingPlatform(Vector3 position, Vector3 rotation, float upTime = 3, float downTime = 3);
	int m_nrOfStaticPlatforms = 0;
	void createStaticPlatform(Vector3 position, Vector3 rotation, Vector3 scale, std::string meshPath);
	int m_nrOfSweepingPlatforms = 0;
	void createSweepingPlatform(Vector3 startPos, Vector3 endPos);

public:
	Scene();
	~Scene();
	void loadScene(std::string path);
	void updateScene(const float &dt);
	
};