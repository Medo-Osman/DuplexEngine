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
//#include <algorithm>
#include "SwingComponent.h"
#include <filesystem>
#include <algorithm>
#include"QuadTree.h"
#include "DrawCallStructFile.h"


struct Settings
{
	int width = 1920;
	int height = 1080;
};

const std::string PLAYER_ENTITY_NAME = "playerEntity";
const float PLAYER_CAPSULE_HEIGHT = .6f;
const float PLAYER_CAPSULE_RADIUS = .2f;

class Engine
{
private:
	Engine();

	//static const int m_startWidth = ApplicationLayer::getInstance().m_width;
	//static const int m_startHeight = ApplicationLayer::m_height;


	ID3D11Device* m_devicePtr = NULL;
	ID3D11DeviceContext* m_dContextPtr = NULL;

	Vector4 m_skyLightDir = Vector4(0.25, -0.5, 0.25, 0);//Vector4(-0.8f, 1.0f, -0.7f, 0);//Vector4(-0.5, 0, -0.5, 0);
	Vector4 m_skyLightColor = Vector4(1, 1, 1, 1);
	FLOAT m_skyLightBrightness = 4.f;
	FLOAT m_ambientLightLevel = 0.5f;

	// Entities
	std::unordered_map<std::string, Entity*>* m_entities;

	std::vector<std::vector<DrawCallStruct>>* m_drawCallsPtr;
	std::vector<MeshComponent*>* m_shadowPassDrawCallsPtr;
	std::vector<MeshComponent*>* m_meshComponents;

	std::unordered_map<std::string, LightComponent*>* m_lightComponentMap;

	// Player
	Player* m_player = nullptr;
	std::vector<Player*>* serverPlayers;

	bool isHost = false;
	bool serverRunning = false;
	bool isClient = false;
	bool isConnected = false; 

	// Camera
	Camera m_camera;

	Settings m_settings;

	Input* m_input = nullptr;



	QuadTree* m_quadTreePtr;
	
	//NETWORK
	static void runClient();
	static void runServer();

	// Materials


	bool DeviceAndContextPtrsAreSet; //This bool just ensures that no one calls Engine::initialize before Renderer::initialize has been called
	void updateLightData();

public:
	static Engine& get();

	void initialize(Input* input);
	void setDeviceAndContextPtrs(ID3D11Device* devicePtr, ID3D11DeviceContext* dContextPtr);

	~Engine();

	void release();
	void update(const float &dt);

	void setEntitiesMapPtr(std::unordered_map<std::string, Entity*>* entities);

	void setDrawCallsPtr(std::vector<std::vector<DrawCallStruct>>* drawCallsPtr);
	void setShadowPassDrawCallsPtr(std::vector<MeshComponent*>* shadowPassDrawCallsPtr);

	void setMeshComponentVector(std::vector<MeshComponent*>* meshComponentVector);

	void setLightComponentMapPtr(std::unordered_map<std::string, LightComponent*>* lightComponents);
	void setQuadTreePtr(QuadTree* quadTree) { m_quadTreePtr = quadTree; }
	QuadTree* getQuadTreePtr() { return m_quadTreePtr; }

	bool addComponentToPlayer(std::string componentIdentifier, Component* component);

	std::vector<std::vector<DrawCallStruct>>* getDrawCallsPtr();
	std::vector<MeshComponent*>* getShadowPassDrawCallsPtr();

	std::vector<MeshComponent*>* getMeshComponents();

	std::unordered_map<std::string, LightComponent*>* getLightComponentMap();
	std::unordered_map<std::string, Entity*>* getEntityMap();
	Vector4& getSkyLightDir();
	void setSkyLightDir(Vector4 dir);
	void setSkyLightColor(Vector4 color);
	void setSkyLightIntensity(float intensity);

	Input* getInput();

	Settings getSettings() const;
	Camera* getCameraPtr();
	float getGameTime() const;
	Player* getPlayerPtr();
	std::vector<Player*>* getServerPlayers();

	bool getHost() { return this->isHost; }
	void setHost(bool tf);
	void setClient(bool tf);
	ID3D11DeviceContext* getDeviceContextPtr() { return m_dContextPtr; }
	ID3D11Device* getDevicePtr() { return m_devicePtr; }

};

