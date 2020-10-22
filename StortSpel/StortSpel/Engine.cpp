#include "3DPCH.h"
#include "Engine.h"
#include"ApplicationLayer.h"
#include"CharacterControllerComponent.h"
#include"TriggerComponent.h"
#include"RotateComponent.h"
#include"PickupComponent.h"

Engine::Engine()
{
	m_settings.width = m_startWidth;
	m_settings.height = m_startHeight;
	Physics::get().Attach(this, false, true);
}

void Engine::updateLightData()
{
	lightBufferStruct lightInfo;
	int nrPointLights = 0;
	int nrSpotLights = 0;
	for (auto light : m_lightComponentMap)
	{
		Matrix parentTransform = XMMatrixTranslationFromVector(getEntity(light.second->getParentEntityIdentifier())->getTranslation());
		
		Vector3 offsetFromParent = light.second->getTranslation();

		Matrix parentRotation = getEntity(light.second->getParentEntityIdentifier())->getRotationMatrix();

		Vector3 finalPos = XMVector3TransformCoord(offsetFromParent, parentRotation*parentTransform);

		if (light.second->getLightType() == LightType::Point)
		{
			PointLightRepresentation pointLight;
			pointLight.position = finalPos;
			pointLight.color = light.second->getColor();
			pointLight.intensity = light.second->getIntensity();

			lightInfo.pointLights[nrPointLights++] = pointLight;
			lightInfo.nrOfPointLights = nrPointLights;
		}
		if (light.second->getLightType() == LightType::Spot)
		{
			SpotLightComponent* spotLightComponent = dynamic_cast<SpotLightComponent*>(light.second);

			SpotLightRepresentation spotLight;
			spotLight.position = finalPos;
			spotLight.color = spotLightComponent->getColor();
			spotLight.intensity = spotLightComponent->getIntensity();
			spotLight.coneFactor = spotLightComponent->getConeFactor();
			spotLight.direction = Vector3(XMVector3TransformCoord(XMVectorSet(spotLightComponent->getDirection().x, spotLightComponent->getDirection().y, spotLightComponent->getDirection().z, 0),parentRotation));
			
			lightInfo.spotLights[nrSpotLights++] = spotLight;
			lightInfo.nrOfSpotLights = nrSpotLights;
		}
	}

	lightInfo.skyLight.direction = m_skyLightDir;
	lightInfo.skyLight.color = m_skyLightColor;
	lightInfo.skyLight.brightness = m_skyLightBrightness;
	lightInfo.ambientLightLevel = m_ambientLightLevel;

	Renderer::get().setPointLightRenderStruct(lightInfo);
}

Engine& Engine::get()
{
	static Engine instance;
	return instance;
}

void Engine::sendPhysicsMessage(PhysicsData& physicsData, bool& removed)
{
	std::vector<Component*> vec;
	m_entities[physicsData.entityIdentifier]->getComponentsOfType(vec, ComponentType::MESH);
	for (size_t i = 0; i < vec.size(); i++)
	{
		int id = static_cast<MeshComponent*>(vec[i])->getRenderId();
		m_meshComponentMap.erase(id);
	}
	this->removeEntity(physicsData.entityIdentifier);
	removed = true;
}

Entity* Engine::getEntity(std::string key)
{
	if (m_entities.find(key) != m_entities.end())
		return m_entities[key];
	else
		return nullptr;
}

Entity* Engine::addEntity(std::string identifier)
{
	if (m_entities.find(identifier) != m_entities.end())// If one with that name is already found
	{
		ErrorLogger::get().logError("identical entity");
		return nullptr;
	}

	m_entities[identifier] = new Entity(identifier);

	return m_entities[identifier];
}

void Engine::removeEntity(std::string identifier)
{
	delete m_entities[identifier];
	m_entities.erase(identifier);
}

Engine::~Engine()
{
	if (m_player)
		delete m_player;

	for (std::pair<std::string, Entity*> entity : m_entities)
		delete entity.second;

	m_entities.clear();
}

void Engine::update(const float& dt)
{
	m_camera.update(dt);
	m_player->updatePlayer(dt);

	for (auto& entity : m_entities)
	{
		entity.second->update(dt);
	}

	updateLightData();

	// AUDIO TEST
	nightVolume += dt * nightSlide;
	if (nightVolume < 0.f)
	{
		nightVolume = 0.f;
		nightSlide = -nightSlide;
	}
	else if (nightVolume > 0.2f)
	{
		nightVolume = 0.2f;
		nightSlide = -nightSlide;
	}
	//AudioComponent* ac = dynamic_cast<AudioComponent*>(m_entities["audioTest"]->getComponent("testSound"));
	//ac->setVolume(nightVolume);

}
Settings Engine::getSettings() const
{
	return m_settings;
}
Camera* Engine::getCameraPtr()
{
	return &m_camera;
}
bool Engine::addComponent(Entity* entity, std::string componentIdentifier, Component* component)
{
	entity->addComponent(componentIdentifier, component);

	if (component->getType() == ComponentType::MESH)
	{
		MeshComponent* meshComponent = dynamic_cast<MeshComponent*>(component);

		addMeshComponent(meshComponent);
	}

	if (component->getType() == ComponentType::LIGHT)
	{
		LightComponent* lightComponent = dynamic_cast<LightComponent*>(component);

		addLightComponent(lightComponent);
	}

	return true;
}



void Engine::addMeshComponent(MeshComponent* component)
{
	component->setRenderId(++m_MeshCount);
	m_meshComponentMap[m_MeshCount] = component;
}

void Engine::createNewPhysicsComponent(Entity* entity, bool dynamic, std::string meshName, PxGeometryType::Enum geometryType, std::string materialName, bool isUnique)
{
	std::vector<Component*> tempComponentVector;
	PhysicsComponent* physComp = new PhysicsComponent();
	MeshComponent* meshComponent = nullptr;
	bool found = false;

	entity->getComponentsOfType(tempComponentVector, ComponentType::MESH);

	if (meshName != "")
	{
		for (std::size_t i = 0; i < tempComponentVector.size() && !found; ++i) {
			MeshComponent* currentMesh = static_cast<MeshComponent*>(tempComponentVector[i]);
			if (currentMesh->getFilePath() == meshName)
			{
				meshComponent = currentMesh;
				found = true;
			}
		}

		if (!found)
		{
			ErrorLogger::get().logError(("When creating new physics component, meshResource " + meshName + " does not exist. ->").c_str());

			if (tempComponentVector.size() > 0)
			{
				ErrorLogger::get().logError("Using first meshComponent.");
				meshComponent = static_cast<MeshComponent*>(tempComponentVector[0]);
				found = true;
			}

		}
	}
	else
	{
		if (tempComponentVector.size() > 0)
		{
			meshComponent = static_cast<MeshComponent*>(tempComponentVector[0]);
			found = true;
		}
	}
	if (!found)
		ErrorLogger::get().logError("Trying to add physic component without any meshcomponent!. Can't use this helper function.");

	
	entity->addComponent("physics", physComp);
	physComp->initActorAndShape(entity, meshComponent, geometryType, dynamic, materialName, isUnique);
}
	
void Engine::addLightComponent(LightComponent* component)
{
	if (m_lightCount < 8)
	{
		component->setLightID(component->getIdentifier());
		m_lightComponentMap[component->getIdentifier()] = component;
	}
	else
		ErrorLogger::get().logError("Maximum lights achieved, failed to add one.");
}

void Engine::removeLightComponent(LightComponent* component)
{
	getEntity(component->getParentEntityIdentifier())->removeComponent(component);

	int nrOfErased = m_lightComponentMap.erase(component->getIdentifier());
	if (nrOfErased > 0) //if it deleted more than 0 elements
	{
		m_lightCount -= nrOfErased;
	}
}

std::map<unsigned int long, MeshComponent*>* Engine::getMeshComponentMap()
{
	return &m_meshComponentMap;
}

void Engine::buildTestStage()
{
	// Cube 1
	
}

void Engine::setDeviceAndContextPtrs(ID3D11Device* devicePtr, ID3D11DeviceContext* dContextPtr)
{
	m_devicePtr = devicePtr;
	m_dContextPtr = dContextPtr;

	DeviceAndContextPtrsAreSet = true;
}

void Engine::initialize()
{
	if (!DeviceAndContextPtrsAreSet)
	{
		// Renderer::initialize needs to be called and it needs to call setDeviceAndContextPtrs()
		// before this function call be called.
		assert(false);
	}

	m_player = new Player();
	m_camera.setProjectionMatrix(80.f,  (float)m_settings.width/(float)m_settings.height, 0.01f, 1000.0f);
	ApplicationLayer::getInstance().m_input.Attach(m_player);
	if (addEntity("meshPlayer"))
	{
		addComponent(m_entities["meshPlayer"], "mesh", new MeshComponent("testTania_tania_geo.lrm", ShaderProgramsEnum::TEMP_TEST));
		m_entities["meshPlayer"]->setPosition({ 5, 10.f, 0 });

		//Point Light
		addComponent(m_entities["meshPlayer"], "testLight", new LightComponent());
		dynamic_cast<LightComponent*>(m_entities["meshPlayer"]->getComponent("testLight"))->setPosition({ 0,1.f,-5 });
		dynamic_cast<LightComponent*>(m_entities["meshPlayer"]->getComponent("testLight"))->setColor(XMFLOAT3(1, 1, 1));
		dynamic_cast<LightComponent*>(m_entities["meshPlayer"]->getComponent("testLight"))->setIntensity(1.0f);

		//Spot Light
		addComponent(m_entities["meshPlayer"], "spotlightTest2", new SpotLightComponent());
		dynamic_cast<SpotLightComponent*>(m_entities["meshPlayer"]->getComponent("spotlightTest2"))->setPosition({ 0,1.f,0 });
		dynamic_cast<SpotLightComponent*>(m_entities["meshPlayer"]->getComponent("spotlightTest2"))->setColor(XMFLOAT3(1, 1, 1));
		dynamic_cast<SpotLightComponent*>(m_entities["meshPlayer"]->getComponent("spotlightTest2"))->setIntensity(3.f);

		//Tests and demonstration how to add and remove lights
		for (int i = 0; i < 8; i++)
		{
			addComponent(m_entities["meshPlayer"], std::string("lightTest")+std::to_string(i), new LightComponent());
		}

		for (int i = 0; i < 8; i++)
		{
			removeLightComponent(static_cast<LightComponent*>(m_entities["meshPlayer"]->getComponent(std::string("lightTest") + std::to_string(i))));
		}

		m_entities["meshPlayer"]->scaleUniform(0.02f);
		m_entities["meshPlayer"]->addComponent("CCC", new CharacterControllerComponent());
		CharacterControllerComponent* pc = static_cast<CharacterControllerComponent*>(m_entities["meshPlayer"]->getComponent("CCC"));
		pc->initController(m_entities["meshPlayer"], 1.75f, 0.5, {0.f, -1.45f, 0.f}, "human");


		m_player->setPlayerEntity(m_entities["meshPlayer"]);

		addComponent(m_entities["meshPlayer"], "audio", new AudioComponent(L"Explosion.wav", false, 0.5f));
		//addComponent(m_entities["meshPlayer"], "audioLoop", new AudioComponent(L"PickupTunnels.wav", true, 0.5f));
	}
	else
	{
		ErrorLogger::get().logError("No player model added or already exists when adding");
	}
	m_player->setCameraTranformPtr(m_camera.getTransform());

	//TriggerEntity
	Entity* triggerEntity = addEntity("TriggerEntity");
	triggerEntity->setPosition(0, 10.f, 10);
	addComponent(triggerEntity, "mesh", new MeshComponent("testCube_pCube1.lrm", ShaderProgramsEnum::TEMP_TEST));
	addComponent(triggerEntity, "trigger", new PickupComponent(PickupType::SPEED, 8.f, 10));
	static_cast<TriggerComponent*>(triggerEntity->getComponent("trigger"))->initTrigger(triggerEntity, { 1, 1, 1 });
	addComponent(triggerEntity, "rotate", new RotateComponent(triggerEntity, { 0.f, 1.f, 0.f }));

	//ScoreEntity
	Entity* scoreEntity = addEntity("ScoreEntity");
	scoreEntity->setPosition(10, 10.f, 10);
	addComponent(scoreEntity, "mesh", new MeshComponent("star.lrm", ShaderProgramsEnum::TEMP_TEST));
	addComponent(scoreEntity, "trigger", new PickupComponent(PickupType::SCORE, 8.f, 10));
	static_cast<TriggerComponent*>(scoreEntity->getComponent("trigger"))->initTrigger(scoreEntity, { 1, 1, 1 });
	addComponent(scoreEntity, "rotate", new RotateComponent(scoreEntity, { 0.f, 1.f, 0.f }));
	
	//Entity* audioTestDelete = addEntity("deleteTestAudio");
	//addComponent(audioTestDelete, "deleteSound", new AudioComponent(L"PickupTunnels.wav", true, 0.5f));
	//delete m_entities["deleteTestAudio"];
	//m_entities.erase("deleteTestAudio");

	// Audio test
	//Entity* audioTest = addEntity("audioTest");
	//addComponent(audioTest, "testSound", new AudioComponent(L"GoodSongYes.wav", true, 0.2f));
	//nightSlide = 0.01f;
	//nightVolume = 0.2f;

	// Temp entity init
	addEntity("first");
	addComponent(m_entities["first"], "test", new TestComponent());

	//Build the hardcoded test stage
	buildTestStage();

}
