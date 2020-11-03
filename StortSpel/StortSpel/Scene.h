#pragma once
#include "Player.h"
#include "Engine.h"

enum class ScenesEnum
{
	LOBBY,
	START,
	ARENA,
};

class Scene : public PhysicsObserver
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
	void createStaticPlatform(Vector3 position, Vector3 rotation, Vector3 scale, std::string meshPath, std::wstring texPath = L"GrayTexture.png");
	int m_nrOfSweepingPlatforms = 0;
	void createSweepingPlatform(Vector3 startPos, Vector3 endPos);
	int m_nrOfSpotLight = 0;
	void createSpotLight(Vector3 position, Vector3 rotation, Vector3 color, float intensity);
	int m_nrOfPointLight = 0;
	void createPointLight(Vector3 position, Vector3 color, float intensity);

	Player* m_player;

	Material ObjectSpaceGrid;	// Temp global grid material
	
	Vector3 m_sceneEntryPosition;

	float m_nightVolume;
	float m_nightSlide;

	unsigned int long m_meshCount = 0;
	unsigned int long m_lightCount = 0;

	std::unordered_map<std::string, Entity*> m_entities;
	std::unordered_map<unsigned int long, MeshComponent*> m_meshComponentMap;
	std::unordered_map<std::string, LightComponent*> m_lightComponentMap;


	void sendPhysicsMessage(PhysicsData& physicsData, bool& removed);

	int m_nrOfPickups = 0;
	void addPickup(const Vector3& position, const int tier = 1, std::string name = "");
	void loadPickups();
	void loadScore();
	int m_nrOfScore = 0;
	void addScore(const Vector3& position, const int tier = 1, std::string name = "");
	void addCheckpoint(const Vector3& position);
	void addTrap(const Vector3& position);
	int m_nrOfCheckpoints = 0;
	int m_nrOftraps = 0;
public:
	Scene();
	~Scene();
	void loadScene(std::string path);
	void loadLobby();
	void loadArena();
	void loadMaterialTest();
	void updateScene(const float &dt);
	Vector3 getEntryPosition();

	Entity* getEntity(std::string key);
	Entity* addEntity(std::string identifier);
	void removeEntity(std::string identifier);

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
