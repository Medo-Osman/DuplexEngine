#pragma once
#include "Player.h"
#include "PointLightComponent.h"
#include "MeshComponent.h"
#include "TestComponent.h"
#include "Camera.h"

struct Settings
{
	int width;
	int height;
};

class Engine
{

private:
	Engine();
	Settings m_settings;
	static const int m_startHeight = 600;
	static const int m_startWidth = 600;
	
	ID3D11Device* m_devicePtr = NULL;
	ID3D11DeviceContext* m_dContextPtr = NULL;

	// Entities
	std::unordered_map<std::string, Entity*> m_entities;
	//{};
	Player* m_player = nullptr;
	Camera m_camera;
	std::map<unsigned int long, MeshComponent*> m_meshComponentMap;
	std::map<unsigned int long, PointLightComponent*> m_lightComponentMap;

	unsigned int long m_MeshCount = 0;
	unsigned int long m_lightCount = 0;

	bool DeviceAndContextPtrsAreSet; //This bool just insures that no one calls Engine::initialize before Renderer::initialize has been called

	void updateRenderPointLights();
public:
	static Engine& get();

	void initialize();
	void setDeviceAndContextPtrs(ID3D11Device* devicePtr, ID3D11DeviceContext* dContextPtr);

	Entity* getEntity(std::string key);
	Entity* addEntity(std::string identifier);
	~Engine();

	void update(const float &dt);

	Settings getSettings() const;
	Camera* getCameraPtr();

	bool addComponent(Entity* entity, std::string componentIdentifier, Component* component);

	void addMeshComponent(MeshComponent* component);
	void addPointLightComponent(PointLightComponent* component);
	void removePointLightComponent(UINT32 id);

	std::map<unsigned int long, MeshComponent*>* getMeshComponentMap();

	void buildTestStage();
};
