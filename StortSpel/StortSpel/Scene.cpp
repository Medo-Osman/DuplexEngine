#include "3DPCH.h"
#include "Scene.h"



Scene::Scene()
{
	m_player = Engine::get().getPlayerPtr();

	MeshComponent* meshComponent = dynamic_cast<MeshComponent*>(m_player->getPlayerEntity()->getComponent("mesh"));
	addMeshComponent(meshComponent);
}

Scene::~Scene()
{
}

void Scene::loadScene(std::string path)
{
	Engine* engine = &Engine::get();
	Entity* entity;

	entity = addEntity("cube1");
	if (entity)
	{
		addComponent(entity, "mesh", new MeshComponent("testCube_pCube1.lrm", Material({ L"DevTexture1m.png" })));
		entity->scaleUniform({ -1.f });
	}

	// Cube 2
	entity = addEntity("cube2");
	if (entity)
	{
		addComponent(entity, "mesh", new MeshComponent("testCube_pCube1.lrm", Material({ L"DevTexture2m.png" })));
		entity->move({ 10.f, 0.5f, 0.f });
		entity->scaleUniform({ -2.f });
	}

	// Cube 3
	entity = addEntity("cube3");
	if (entity)
	{
		addComponent(entity, "mesh", new MeshComponent("testCube_pCube1.lrm", Material({ L"DevTexture3m.png" })));
		entity->move({ 20.f, 1.f, 0.f });
		entity->scaleUniform({ -3.f });
	}

	// Cube 4
	entity = addEntity("cube4");
	if (entity)
	{
		addComponent(entity, "mesh", new MeshComponent("testCube_pCube1.lrm", Material({ L"DevTexture4m.png" })));
		entity->move({ 30.f, 1.5f, 0.f });
		entity->scaleUniform({ -4.f });
	}

	// Tent
	/*Entity* tent = addEntity("tent");
	if (tent)
	{
		addComponent(tent, "mesh", new MeshComponent("BigTopTent_Cylinder.lrm", Material({ L"T_CircusTent_D.png" })));
		tent->rotate({ XMConvertToRadians(-90.f), 0.f, 0.f });
		tent->move({ -10.f, 0.f, 0.f });

		this->createNewPhysicsComponent(tent, true, "");
	}*/

	// Floor
	Material gridTest = Material({ L"T_GridTestTex.bmp" });
	entity = addEntity("floor");
	if (entity)
	{
		addComponent(entity, "mesh", new MeshComponent("testCube_pCube1.lrm", gridTest));
		entity->scale({ 300, 2,300 });
		entity->move({ 0,-2,0 });
		createNewPhysicsComponent(entity, false, "", PxGeometryType::eBOX, "earth", false);
	}

	// Flying Cube
	entity = addEntity("cube-test2");
	if (entity)
	{
		addComponent(entity, "mesh", new MeshComponent("testCube_pCube1.lrm", gridTest));
		entity->scaleUniform({ 3.f });
		entity->move({ 0.f, 5.f, 5.f });
		entity->rotate({ 0.f, XMConvertToRadians(-45.f), XMConvertToRadians(-45.f) });
		createNewPhysicsComponent(entity, true, "", PxGeometryType::eSPHERE);
	}

	// Cube with sphere shape
	entity = addEntity("cube-test3");
	if (entity)
	{
		addComponent(entity, "mesh", new MeshComponent("testCube_pCube1.lrm", ShaderProgramsEnum::TEMP_TEST));
		entity->scaleUniform({ 3.f });
		entity->move({ -10.f, 5.f, 5.f });
		entity->rotate({ 0.f, XMConvertToRadians(-45.f), XMConvertToRadians(-45.f) });
		addComponent(entity, "physics", new PhysicsComponent(&Physics::get()));
		PhysicsComponent* physicsComp = static_cast<PhysicsComponent*>(entity->getComponent("physics"));
		physicsComp->initActor(entity, false);
		physicsComp->addSphereShape(2.f);
	}

	// XWing
	Entity* testXwing = addEntity("testXwing");
	if (testXwing)
	{
		addComponent(testXwing, "xwingtestmesh",
			new MeshComponent("xWingFbx_xwing.lrm", Material({ L"T_tempTestXWing.png" })));
		addComponent(testXwing, "xwingtestmove",
			new SweepingComponent(dynamic_cast<Transform*>(testXwing), Vector3(0, 10, -5), Vector3(0, 10, 100), 20));
	}
	// Rotating Cube
	Entity* rotatingCube = addEntity("RotatingCube");
	if (rotatingCube)
	{
		addComponent(rotatingCube, "mesh",
			new MeshComponent("testCube_pCube1.lrm"));
		addComponent(rotatingCube, "rotate",
			new RotateAroundComponent(dynamic_cast<Transform*>(testXwing), testXwing->getRotationMatrix(), dynamic_cast<Transform*>(rotatingCube), 5));
	}
	// Rotating Cube 2
	Entity* rotatingCube2 = addEntity("RotatingCube2");
	if (rotatingCube2)
	{
		addComponent(rotatingCube2, "mesh",
			new MeshComponent("testCube_pCube1.lrm"));
		addComponent(rotatingCube2, "rotate",
			new RotateAroundComponent(dynamic_cast<Transform*>(rotatingCube), rotatingCube->getRotationMatrix(), dynamic_cast<Transform*>(rotatingCube2), 2, 40.f));
		rotatingCube2->scale({ 0.5f, 0.5f, 0.5f });
	}
	// Rotating Cube 3
	Entity* rotatingCube3 = addEntity("RotatingCube3");
	if (rotatingCube3)
	{
		addComponent(rotatingCube3, "mesh",
			new MeshComponent("testCube_pCube1.lrm"));
		addComponent(rotatingCube3, "rotate",
			new RotateAroundComponent(dynamic_cast<Transform*>(rotatingCube), rotatingCube->getRotationMatrix(), dynamic_cast<Transform*>(rotatingCube3), 2, 40.f, 180.f));
		rotatingCube3->scale({ 0.5f, 0.5f, 0.5f });
	}

	// Flipping Cube 
	entity = addEntity("FlippingCube");
	if (entity)
	{
		addComponent(entity, "mesh",
			new MeshComponent("testCube_pCube1.lrm", Material({ L"DevTexture2m.png" })));
		entity->move({ 5.f, 0.f, 15.f });
		entity->scale({ 4, 1, 4 });
		addComponent(entity, "flipp",
			new FlippingComponent(dynamic_cast<Transform*>(entity), 3, 3));
	}

	// Platforms
	for (int i = 0; i < 5; i++)
	{
		entity = addEntity("cube-test" + std::to_string(i));
		if (entity)
		{
			addComponent(entity, "mesh", new MeshComponent("testCube_pCube1.lrm"));
			entity->scale({ 3,0.2,5 });
			entity->move({ 10.f + (float)i * 3.f, .2f + (float)i, 15.f });
			createNewPhysicsComponent(entity);
		}
	}

	m_entities["SkyBox"] = addEntity("SkyBox");
	if (m_entities["SkyBox"])
	{
		Material skyboxMat;
		skyboxMat.addTexture(L"Skybox_Texture.dds", true);
		addComponent(m_entities["SkyBox"], "cube", new MeshComponent("Skybox_Mesh_pCube1.lrm", ShaderProgramsEnum::SKYBOX, skyboxMat));

	}

	Entity* audioTestDelete = addEntity("deleteTestAudio");
	addComponent(audioTestDelete, "deleteSound", new AudioComponent(L"PickupTunnels.wav", true, 0.5f));
	delete m_entities["deleteTestAudio"];
	m_entities.erase("deleteTestAudio");

	// Audio test
	Entity* audioTest = addEntity("audioTest");
	addComponent(audioTest,"testSound", new AudioComponent(L"NightAmbienceSimple_02.wav", true, 0.2f));
	m_nightSlide = 0.01f;
	m_nightVolume = 0.2f;



}

void Scene::loadLobby()
{
	Engine* engine = &Engine::get();
	Entity* entity;

	Material gridTest = Material({ L"T_GridTestTex.bmp" });
	entity = addEntity("floor");
	if (entity)
	{
		addComponent(entity, "mesh", new MeshComponent("testCube_pCube1.lrm", gridTest));
		entity->scale({ 300, 2,300 });
		entity->move({ 0,-2,0 });
		createNewPhysicsComponent(entity, false, "", PxGeometryType::eBOX, "earth", false);
	}

	entity = addEntity("walls");
	if (entity)
	{
		addComponent(entity, "mesh", new MeshComponent("testCube_pCube1.lrm", Material({ L"DevTexture1m.png" })));
		entity->scale({ 30, -30, 30});
		entity->move({ 0,8,0 });
	}

	//Point Light
	addComponent(m_player->getPlayerEntity(),"testLight", new LightComponent());
	dynamic_cast<LightComponent*>(m_player->getPlayerEntity()->getComponent("testLight"))->translation({ 0,1.f,-5 });
	dynamic_cast<LightComponent*>(m_player->getPlayerEntity()->getComponent("testLight"))->setColor(XMFLOAT3(1, 1, 1));
	dynamic_cast<LightComponent*>(m_player->getPlayerEntity()->getComponent("testLight"))->setIntensity(1.0f);

	//Spot Light
	addComponent(m_player->getPlayerEntity(), "spotlightTest2", new SpotLightComponent());
	dynamic_cast<SpotLightComponent*>(m_player->getPlayerEntity()->getComponent("spotlightTest2"))->translation({ 0,1.f,0 });
	dynamic_cast<SpotLightComponent*>(m_player->getPlayerEntity()->getComponent("spotlightTest2"))->setColor(XMFLOAT3(1, 1, 1));
	dynamic_cast<SpotLightComponent*>(m_player->getPlayerEntity()->getComponent("spotlightTest2"))->setIntensity(3.f);

	//Tests and demonstration how to add and remove lights
	for (int i = 0; i < 8; i++)
	{
		addComponent(m_player->getPlayerEntity(),"lightTest" + std::to_string(i), new LightComponent());
	}

	for (int i = 0; i < 8; i++)
	{
		removeLightComponent(static_cast<LightComponent*>(m_player->getPlayerEntity()->getComponent("lightTest" + std::to_string(i))));
	}

	Entity* audioTestDelete = addEntity("deleteTestAudio");
	addComponent(audioTestDelete, "deleteSound", new AudioComponent(L"PickupTunnels.wav", true, 0.5f));
	delete m_entities["deleteTestAudio"];
	m_entities.erase("deleteTestAudio");

	// Audio test
	Entity* audioTest = addEntity("audioTest");
	addComponent(audioTest, "testSound", new AudioComponent(L"NightAmbienceSimple_02.wav", true, 0.2f));
	m_nightSlide = 0.01f;
	m_nightVolume = 0.2f;

}

void Scene::updateScene(const float& dt)
{
	// AUDIO TEST
	m_nightVolume += dt * m_nightSlide;
	if (m_nightVolume < 0.f)
	{
		m_nightVolume = 0.f;
		m_nightSlide = -m_nightSlide;
	}
	else if (m_nightVolume > 0.2f)
	{
		m_nightVolume = 0.2f;
		m_nightSlide = -m_nightSlide;
	}
	AudioComponent* ac = dynamic_cast<AudioComponent*>(m_entities["audioTest"]->getComponent("testSound"));
	ac->setVolume(m_nightVolume);
}

Entity* Scene::getEntity(std::string key)
{
	if (key == PLAYER_ENTITY_NAME)
		return m_player->getPlayerEntity();
	else if (m_entities.find(key) != m_entities.end())
		return m_entities.at(key);
	else
		return nullptr;
}

Entity* Scene::addEntity(std::string identifier)
{

	//if (m_entities)
	//{
	if (m_entities.find(identifier) != m_entities.end())// If one with that name is already found
	{
		return nullptr;
	}


	m_entities[identifier] = new Entity(identifier);

	return m_entities[identifier];
	//}
}

bool Scene::addComponent(Entity* entity, std::string componentIdentifier, Component* component)
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

void Scene::addMeshComponent(MeshComponent* component)
{
	component->setRenderId(++m_meshCount);
	m_meshComponentMap[m_meshCount] = component;
}

void Scene::createNewPhysicsComponent(Entity* entity, bool dynamic, std::string meshName, PxGeometryType::Enum geometryType, std::string materialName, bool isUnique)
{
	std::vector<Component*> tempComponentVector;
	PhysicsComponent* physComp = new PhysicsComponent(&Physics::get());
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

void Scene::addLightComponent(LightComponent* component)
{
	if (m_lightCount < 8)
	{
		component->setLightID(component->getIdentifier());
		m_lightComponentMap[component->getIdentifier()] = component;
	}
	else
		ErrorLogger::get().logError("Maximum lights achieved, failed to add one.");
}

void Scene::removeLightComponent(LightComponent* component)
{
	getEntity(component->getParentEntityIdentifier())->removeComponent(component);
	
	int nrOfErased = m_lightComponentMap.erase(component->getIdentifier());
	if (nrOfErased > 0) //if it deleted more than 0 elements
	{
		m_lightCount -= nrOfErased;
	}
}

void Scene::removeLightComponentFromMap(LightComponent* component)
{


	int nrOfErased = m_lightComponentMap.erase(component->getIdentifier());
	if (nrOfErased > 0) //if it deleted more than 0 elements
	{
		m_lightCount -= nrOfErased;
	}
}

std::unordered_map<std::string, Entity*>* Scene::getEntityMap()
{
	return &m_entities;
}

std::unordered_map<std::string, LightComponent*>* Scene::getLightMap()
{
	return &m_lightComponentMap;
}

std::unordered_map<unsigned int long, MeshComponent*>* Scene::getMeshComponentMap()
{
	return &m_meshComponentMap;
}

