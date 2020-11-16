#pragma once
#include "Player.h"
#include "LightComponent.h"
#include "SpotLightComponent.h"
#include "MeshComponent.h"
#include "AnimatedMeshComponent.h"
#include "TestComponent.h"
#include "RotateAroundComponent.h"
#include "SweepingComponent.h"
#include "FlippingComponent.h"
#include "PhysicsComponent.h"
#include "AudioHandler.h"
#include "AudioComponent.h"
#include "CheckpointComponent.h"
#include "Camera.h"
#include "PacketHandler.h"
#include "ProjectileComponent.h"
#include "SlowTrapComponent.h"
#include "PushTrapComponent.h"
#include "BarrelComponent.h"
#include "BarrelTriggerComponent.h"



struct Settings
{
	int width = 1920;
	int height = 1080;
};

const std::string PLAYER_ENTITY_NAME = "playerEntity";

class Engine
{
private:
	Engine();
	
	static const int m_startWidth = 1920;
	static const int m_startHeight = 1080;

	
	ID3D11Device* m_devicePtr = NULL;
	ID3D11DeviceContext* m_dContextPtr = NULL;

	Vector4 m_skyLightDir = Vector4(0.25, -0.5, 0.25, 0);//Vector4(-0.8f, 1.0f, -0.7f, 0);//Vector4(-0.5, 0, -0.5, 0);
	Vector4 m_skyLightColor = Vector4(1, 1, 1, 1);
	FLOAT m_skyLightBrightness = 4.f;
	FLOAT m_ambientLightLevel = 0.3f;

	// Entities
	std::unordered_map<std::string, Entity*>* m_entities;
	std::unordered_map<unsigned int long, MeshComponent*>* m_meshComponentMap;
	std::unordered_map<std::string, LightComponent*>* m_lightComponentMap;

	Player* m_player = nullptr;
	std::vector<Player*>* serverPlayers;

	

	Camera m_camera; 
	Settings m_settings;

	Input* m_input = nullptr;
	
	bool DeviceAndContextPtrsAreSet; //This bool just ensures that no one calls Engine::initialize before Renderer::initialize has been called
	void updateLightData();

	
public:
	static Engine& get();

	void initialize(Input* input);
	void setDeviceAndContextPtrs(ID3D11Device* devicePtr, ID3D11DeviceContext* dContextPtr);

	~Engine();

	void update(const float &dt);

	void setEntitiesMapPtr(std::unordered_map<std::string, Entity*>* entities);
	void setMeshComponentMapPtr(std::unordered_map<unsigned int long, MeshComponent*>* meshComponents);
	void setLightComponentMapPtr(std::unordered_map<std::string, LightComponent*>* lightComponents);

	bool addComponentToPlayer(std::string componentIdentifier, Component* component);
	void removeLightComponentFromPlayer(LightComponent* component);

	void setBarrelVector(std::vector<Entity*>* entitiesVector);
	std::vector<Entity*>* getBarrelVector();

	std::unordered_map<unsigned int long, MeshComponent*>* getMeshComponentMap();
	std::unordered_map<std::string, LightComponent*>* getLightComponentMap();
	std::unordered_map<std::string, Entity*>* getEntityMap();
	Vector4& getSkyLightDir();

	Input* getInput();

	Settings getSettings() const;
	Camera* getCameraPtr();
	float getGameTime();
	Player* getPlayerPtr();
	std::vector<Player*>* getServerPlayers();

	ID3D11DeviceContext* getDeviceContextPtr() { return m_dContextPtr; }
	ID3D11Device* getDevicePtr() { return m_devicePtr; }

};
