#pragma once
#include <unordered_map>
#include "Entity.h"
#include "Player.h"
#include "MeshComponent.h"
#include "TestComponent.h"

class Engine
{

private:

	ID3D11Device* m_devicePtr = NULL;
	ID3D11DeviceContext* m_dContextPtr = NULL;

	// Entities
	std::unordered_map<std::string, Entity*> m_entities;
	Engine() {};
	Player* m_player = nullptr;
	std::map<unsigned int long, MeshComponent*> m_meshComponentMap;

	unsigned int long m_MeshCount = 0;

	bool DeviceAndContextPtrsAreSet; //This bool just insures that no one calls Engine::initialize before Renderer::initialize has been called

	Engine() {};

public:
	static Engine& get();

	void initialize();
	void setDeviceAndContextPtrs(ID3D11Device* devicePtr, ID3D11DeviceContext* dContextPtr);

	Entity* getEntity(std::string key);
	bool addEntity(std::string identifier);
	~Engine();

	void update(Mouse* mousePtr, Keyboard* keyboardPtr, const float &dt);

	bool addComponent(Entity* entity, std::string componentIdentifier, Component* component);

	void addMeshComponent(MeshComponent* component);
	std::map<unsigned int long, MeshComponent*>* getMeshComponentMap();


};
