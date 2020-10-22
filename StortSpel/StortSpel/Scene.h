#pragma once
#include "Player.h"
#include "Engine.h"

class Scene
{
private:
	Player* m_player;
	
	float m_nightVolume;
	float m_nightSlide;

	unsigned int long m_meshCount = 0;
	unsigned int long m_lightCount = 0;

	std::unordered_map<std::string, Entity*> m_entities;
	std::unordered_map<unsigned int long, MeshComponent*> m_meshComponentMap;
	std::unordered_map<std::string, LightComponent*> m_lightComponentMap;
	

public:
	Scene();
	~Scene();
	void loadScene(std::string path);
	void loadLobby();
	void updateScene(const float &dt);
	

	Entity* getEntity(std::string key);
	Entity* addEntity(std::string identifier);

	bool addComponent(Entity* entity, std::string componentIdentifier, Component* component);
	void addMeshComponent(MeshComponent* component);
	void addLightComponent(LightComponent* component);
	void removeLightComponent(LightComponent* component);
	void removeLightComponentFromMap(LightComponent* component);
	void createNewPhysicsComponent(Entity* entity, bool dynamic = false, std::string meshName = "", PxGeometryType::Enum geometryType = PxGeometryType::eBOX, std::string materialName = "default", bool isUnique = false);

	std::unordered_map<std::string, Entity*>* getEntityMap();
	std::unordered_map<std::string, LightComponent*>* getLightMap();
	std::unordered_map<unsigned int long, MeshComponent*>* getMeshComponentMap();

};