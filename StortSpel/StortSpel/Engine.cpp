#include "3DPCH.h"
#include "Engine.h"
#include"ApplicationLayer.h"

Engine::Engine()
{
	m_settings.width = m_startWidth;
	m_settings.height = m_startHeight;
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
		return nullptr;
	}

	m_entities[identifier] = new Entity(identifier);

	return m_entities[identifier];
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
	AudioComponent* ac = dynamic_cast<AudioComponent*>(m_entities["audioTest"]->getComponent("testSound"));
	ac->setVolume(nightVolume);

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

void Engine::createNewPhysicsComponent(Entity* entity, bool dynamic = false, std::string meshName = "", PxGeometryType::Enum geometryType = PxGeometryType::eBOX, std::string materialName = "default", bool isUnique = false)
{
	std::vector<Component*> tempComponentVector;
	PhysicsComponent* physComp = new PhysicsComponent(&ApplicationLayer::getInstance().m_physics);
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
	//if (addEntity("cube-test"))
	//	addComponent(m_entities["cube-test"], "mesh", new MeshComponent("testCube_pCube1.lrm", ShaderProgramsEnum::TEMP_TEST));

	// Tent
	Entity* tent = addEntity("tent");
	if (tent)
	{
		addComponent(tent, "mesh", new MeshComponent("BigTopTent_Cylinder.lrm", Material({ L"T_CircusTent_D.png" })));
		tent->rotate({ XMConvertToRadians(-90.f), 0.f, 0.f });
		tent->move({ -10.f, 0.f, 0.f });

		this->createNewPhysicsComponent(tent, true, "");
	}

	Material gridTest = Material({ L"T_GridTestTex.bmp" });

	// Floor
	Entity* floor = addEntity("floor");
	if (floor)
	{
		addComponent(floor, "mesh", new MeshComponent("testCube_pCube1.lrm", Material({ L"T_CircusTent_D.png" })));
		floor->scale({ 300,0.1,300 });
		floor->move({ 0,-0.6,0 });
		this->createNewPhysicsComponent(floor, false, "", PxGeometryType::eBOX, "earth", false);
	}

	//Cube 2
	Entity* cube = addEntity("cube-test2");
	if (cube)
	{
		addComponent(cube, "mesh", new MeshComponent("testCube_pCube1.lrm", gridTest));
		cube->scaleUniform({ 3.f });
		cube->move({ 0.f, 5.f, 5.f });
		cube->rotate({ 0.f, XMConvertToRadians(-45.f), XMConvertToRadians(-45.f) });
		this->createNewPhysicsComponent(cube, true, "", PxGeometryType::eSPHERE);
	}

	//Cube with sphere shape
	Entity* cubeSphereBB = addEntity("cube-test3");
	if (cubeSphereBB)
	{
		addComponent(cubeSphereBB, "mesh", new MeshComponent("testCube_pCube1.lrm", ShaderProgramsEnum::TEMP_TEST));
		cubeSphereBB->scaleUniform({ 3.f });
		cubeSphereBB->move({ -10.f, 5.f, 5.f });
		cubeSphereBB->rotate({ 0.f, XMConvertToRadians(-45.f), XMConvertToRadians(-45.f) });
		addComponent(cubeSphereBB, "physics", new PhysicsComponent(&ApplicationLayer::getInstance().m_physics));
		PhysicsComponent* physicsComp = static_cast<PhysicsComponent*>(cubeSphereBB->getComponent("physics"));
		physicsComp->initActor(cubeSphereBB, false);
		physicsComp->addSphereShape(2.f);
	}

	Entity* testXwing = addEntity("testXwing");
	if (testXwing)
	{
		addComponent(testXwing, "xwingtestmesh", new MeshComponent("xWingFbx_xwing.lrm", Material({ L"T_tempTestXWing.png" })));
		testXwing->move({ 15.f, 1.5f, -3.f });
	}

	// Platforms
	for (int i = 0; i < 5; i++)
	{
		Entity* cube = addEntity("cube-test" + std::to_string(i));
		if (cube)
		{
			addComponent(cube, "mesh", new MeshComponent("testCube_pCube1.lrm"));
			cube->scale({ 3,0.2,5 });
			cube->move({ 10.f + (float)i * 3.f, .2f + (float)i, 15.f });
			this->createNewPhysicsComponent(cube);
		}
	}

	// Rotating Cube
	Entity* centerCube = addEntity("centerCube");
	if (centerCube)
	{
		addComponent(centerCube, "mesh",
			new MeshComponent("testCube_pCube1.lrm"));
		centerCube->move({ 0.f, 5.f, 0.f });
		centerCube->rotate({ 0.5f, 0, 0 });
	}

	// Rotating Cube
	if (addEntity("RotatingCube"))
	{
		addComponent(m_entities["RotatingCube"], "mesh",
			new MeshComponent("testCube_pCube1.lrm"));
		addComponent(m_entities["RotatingCube"], "rotate",
			new RotateAroundComponent(centerCube->getTranslation(), centerCube->getRotationMatrix(), dynamic_cast<Transform*>(m_entities["RotatingCube"]), 5));
		
	}


	// Skybox
	if (addEntity("Skybox"))
	{
		Material skyboxMat;
		skyboxMat.addTexture(L"Skybox_Texture.dds", true);
		addComponent(m_entities["Skybox"], "cube", new MeshComponent("Skybox_Mesh_pCube1.lrm", ShaderProgramsEnum::SKYBOX, skyboxMat));

	}
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
		m_entities["meshPlayer"]->translation({ 5, 10.f, 0 });

		//Point Light
		addComponent(m_entities["meshPlayer"], "testLight", new LightComponent());
		dynamic_cast<LightComponent*>(m_entities["meshPlayer"]->getComponent("testLight"))->translation({ 0,1.f,-5 });
		dynamic_cast<LightComponent*>(m_entities["meshPlayer"]->getComponent("testLight"))->setColor(XMFLOAT3(1, 1, 1));
		dynamic_cast<LightComponent*>(m_entities["meshPlayer"]->getComponent("testLight"))->setIntensity(1.0f);

		//Spot Light
		addComponent(m_entities["meshPlayer"], "spotlightTest2", new SpotLightComponent());
		dynamic_cast<SpotLightComponent*>(m_entities["meshPlayer"]->getComponent("spotlightTest2"))->translation({ 0,1.f,0 });
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
		createNewPhysicsComponent(m_entities["meshPlayer"], true, "", PxGeometryType::eBOX, "human");
		PhysicsComponent* pc = static_cast<PhysicsComponent*>(m_entities["meshPlayer"]->getComponent("physics"));
		pc->controllRotation(false);
		m_player->setPlayerEntity(m_entities["meshPlayer"]);

		addComponent(m_entities["meshPlayer"], "audio", new AudioComponent(L"Explosion.wav", false, 0.5f));
		//addComponent(m_entities["meshPlayer"], "audioLoop", new AudioComponent(L"PickupTunnels.wav", true, 0.5f));
	}
	else
	{
		ErrorLogger::get().logError("No player model added or already exists when adding");
	}
	m_player->setCameraTranformPtr(m_camera.getTransform());

	Entity* audioTestDelete = addEntity("deleteTestAudio");
	addComponent(audioTestDelete, "deleteSound", new AudioComponent(L"PickupTunnels.wav", true, 0.5f));
	delete m_entities["deleteTestAudio"];
	m_entities.erase("deleteTestAudio");

	// Audio test
	Entity* audioTest = addEntity("audioTest");
	addComponent(audioTest, "testSound", new AudioComponent(L"NightAmbienceSimple_02.wav", true, 0.2f));
	nightSlide = 0.01f;
	nightVolume = 0.2f;

	// Temp entity init
	addEntity("first");
	addComponent(m_entities["first"], "test", new TestComponent());

	//Build the hardcoded test stage
	buildTestStage();

}
