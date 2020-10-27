#include "3DPCH.h"
#include "Scene.h"
#include"PickupComponent.h"
#include"RotateComponent.h"
#include"Pickup.h"



Scene::Scene()
{
	m_player = Engine::get().getPlayerPtr();

	MeshComponent* meshComponent = dynamic_cast<MeshComponent*>(m_player->getPlayerEntity()->getComponent("mesh"));
	addMeshComponent(meshComponent);
}

Scene::~Scene()
{
}

void Scene::loadPickups()
{
	Engine* engine = &Engine::get();
	Entity* entity;


	addPickup(Vector3(-30, 30, 105));
	addPickup(Vector3(8.5, 40, 172));
}

void Scene::loadScore()
{
	addScore(Vector3(0, 9, 20));
	addScore(Vector3(0, 15, 45));


	addScore(Vector3(-16.54, 15.5, 105));
	addScore(Vector3(16.54, 30, 105));
	addScore(Vector3(-30, 40, 146));
	addScore(Vector3(8.5, 18, 159.5));
	addScore(Vector3(-11, 40, 222.5));
}

void Scene::addScore(const Vector3& position, const int tier, std::string name)
{
	Engine* engine = &Engine::get();
	Entity* pickupPtr;
	if (name == "")
		name = "score_" + std::to_string(m_nrOfScore++);

	pickupPtr = engine->addEntity(name);
	pickupPtr->setPosition(position);
	engine->addComponent(pickupPtr, "mesh", new MeshComponent("star.lrm", ShaderProgramsEnum::TEMP_TEST));
	engine->addComponent(pickupPtr, "pickup", new PickupComponent(PickupType::SCORE, 1.f * (float)tier, 6));
	static_cast<TriggerComponent*>(pickupPtr->getComponent("pickup"))->initTrigger(pickupPtr, { 1, 1, 1 });
	engine->addComponent(pickupPtr, "rotate", new RotateComponent(pickupPtr, { 0.f, 1.f, 0.f }));
}

void Scene::addPickup(const Vector3& position, const int tier, std::string name)
{
	int nrOfPickups = (int)PickupType::COUNT - 1; //-1 due to Score being in pickupTypes
	int pickupEnum = rand() % nrOfPickups;

	Engine* engine = &Engine::get();
	Entity* pickupPtr;
	if (name == "")
		name = "pickup_" + std::to_string(m_nrOfPickups++);

	pickupPtr = engine->addEntity(name);
	pickupPtr->setPosition(position);
	engine->addComponent(pickupPtr, "mesh", new MeshComponent("testCube_pCube1.lrm", ShaderProgramsEnum::TEMP_TEST));
	engine->addComponent(pickupPtr, "pickup", new PickupComponent((PickupType)pickupEnum, 1.f, 6));
	static_cast<TriggerComponent*>(pickupPtr->getComponent("pickup"))->initTrigger(pickupPtr, { 1, 1, 1 });
	engine->addComponent(pickupPtr, "rotate", new RotateComponent(pickupPtr, { 0.f, 1.f, 0.f }));
}

void Scene::loadScene(std::string path)
{
	Engine* engine = &Engine::get();
	Entity* entity;

	this->loadPickups();
	this->loadScore();


	Entity* floor = engine->addEntity("floor"); // Floor:
	if (floor)
	{
		engine->addComponent(floor, "mesh",
			new MeshComponent("testCube_pCube1.lrm", Material({ L"T_GridTestTex.bmp" })));

		floor->setPosition({ 0, 6, 0 });
		floor->scale({ 20, 2, 20 });
		engine->createNewPhysicsComponent(floor, false, "", PxGeometryType::eBOX, "earth", false);
	}

	// Start:
	createStaticPlatform(Vector3(0, 6.5, 20), Vector3(0, 0, 0), Vector3(10, 1, 20), "testCube_pCube1.lrm");
	createStaticPlatform(Vector3(0, 9, 29.5), Vector3(0, 0, 0), Vector3(10, 4, 1), "testCube_pCube1.lrm");
	createStaticPlatform(Vector3(0, 11.5, 39), Vector3(0, 0, 0), Vector3(10, 1, 20), "testCube_pCube1.lrm");
	createStaticPlatform(Vector3(0, 16, 48.5), Vector3(0, 0, 0), Vector3(10, 8, 1), "testCube_pCube1.lrm");
	createStaticPlatform(Vector3(0, 20.5, 58), Vector3(0, 0, 0), Vector3(10, 1, 20), "testCube_pCube1.lrm");
	// Left:
	createStaticPlatform(Vector3(-10.2, 20.5, 73.2), Vector3(0, -45, 0), Vector3(5, 1, 20), "testCube_pCube1.lrm");
	createStaticPlatform(Vector3(-16.54, 20.5, 81), Vector3(0, 0, 0), Vector3(5, 1, 5), "testCube_pCube1.lrm");
	createStaticPlatform(Vector3(-16.54, 17, 83), Vector3(0, 0, 0), Vector3(5, 6, 1), "testCube_pCube1.lrm");
	createStaticPlatform(Vector3(-16.54, 13.5, 102.5), Vector3(0, 0, 0), Vector3(5, 1, 40), "testCube_pCube1.lrm");
	//createStaticPlatform	(Vector3(-16.54, 21.75, 105),	Vector3(0, 0, 0),		Vector3(10, 10.5, 1),	"testCube_pCube1.lrm");
	createStaticPlatform(Vector3(-16.54, 18, 128), Vector3(0, 0, 0), Vector3(1, 1, 1), "SquarePlatform.lrm");
	createStaticPlatform(Vector3(-14, 23, 135), Vector3(0, 45, 0), Vector3(1, 1, 1), "SquarePlatform.lrm");
	createStaticPlatform(Vector3(-7, 28, 137.5), Vector3(0, 90, 0), Vector3(1, 1, 1), "SquarePlatform.lrm");
	createStaticPlatform(Vector3(6, 28, 137.5), Vector3(0, 0, 0), Vector3(10, 1, 5), "testCube_pCube1.lrm");
	createStaticPlatform(Vector3(8.5, 28, 142.5), Vector3(0, 0, 0), Vector3(5, 1, 5), "testCube_pCube1.lrm");
	createParisWheel(Vector3(8.5, 28, 159.5), 0, 30, 4);
	createStaticPlatform(Vector3(8.5, 37.7, 175), Vector3(0, 0, 0), Vector3(5, 1, 10), "testCube_pCube1.lrm");
	createStaticPlatform(Vector3(7.47, 37.7, 180), Vector3(0, -45, 0), Vector3(5, 1, 5), "testCube_pCube1.lrm");
	createFlippingPlatform(Vector3(2.2, 42, 185.5), Vector3(0, -225, 0), 3, 3);
	createStaticPlatform(Vector3(-3.18, 37.7, 190.61), Vector3(0, -45, 0), Vector3(5, 1, 5), "testCube_pCube1.lrm");
	// Right:
	createStaticPlatform(Vector3(10.2, 20.5, 73.2), Vector3(0, 45, 0), Vector3(5, 1, 20), "testCube_pCube1.lrm");
	createStaticPlatform(Vector3(16.54, 20.5, 86), Vector3(0, 0, 0), Vector3(5, 1, 15), "testCube_pCube1.lrm");
	createStaticPlatform(Vector3(16.54, 24, 93), Vector3(0, 0, 0), Vector3(5, 6, 1), "testCube_pCube1.lrm");
	createStaticPlatform(Vector3(16.54, 27.5, 100), Vector3(0, 0, 0), Vector3(5, 1, 15), "testCube_pCube1.lrm");
	createStaticPlatform(Vector3(11.54, 27.5, 105), Vector3(0, 0, 0), Vector3(5, 1, 5), "testCube_pCube1.lrm");
	//createStaticPlatform	(Vector3(2, 30, 105),			Vector3(0, 0, 0),		Vector3(1, 1, 1),		"SquarePlatform.lrm");
	createStaticPlatform(Vector3(-16.54, 27.5, 105), Vector3(0, 0, 0), Vector3(10, 1, 5), "testCube_pCube1.lrm");
	createStaticPlatform(Vector3(-30, 27.5, 107.5), Vector3(0, 0, 0), Vector3(5, 1, 10), "testCube_pCube1.lrm");
	createStaticPlatform(Vector3(-30, 32.6, 126.4), Vector3(-20, 0, 0), Vector3(5, 1, 30), "testCube_pCube1.lrm");
	createStaticPlatform(Vector3(-30, 37.7, 145.32), Vector3(0, 0, 0), Vector3(5, 1, 10), "testCube_pCube1.lrm");
	createStaticPlatform(Vector3(-30, 42, 160), Vector3(0, 0, 0), Vector3(5, 15, 1), "testCube_pCube1.lrm");
	createFlippingPlatform(Vector3(-36, 37.7, 160), Vector3(0, 0, 0), 1, 2);
	createFlippingPlatform(Vector3(-24, 37.7, 160), Vector3(0, 180, 0), 2, 1);
	createStaticPlatform(Vector3(-30, 37.7, 175), Vector3(0, 0, 0), Vector3(5, 1, 10), "testCube_pCube1.lrm");
	createStaticPlatform(Vector3(-23.67, 37.7, 185.3), Vector3(0, 45, 0), Vector3(5, 1, 20), "testCube_pCube1.lrm");
	// End:
	createFlippingPlatform(Vector3(-11, 37.7, 200), Vector3(0, 180, 0), 2, 2);
	createStaticPlatform(Vector3(-11, 37.7, 215), Vector3(0, 0, 0), Vector3(5, 1, 10), "testCube_pCube1.lrm");
	createStaticPlatform(Vector3(-11, 37.7, 222.5), Vector3(0, 90, 0), Vector3(5, 1, 15), "testCube_pCube1.lrm");
	createSweepingPlatform(Vector3(-5, 37.7, 228), Vector3(-5, 50, 270));
	createSweepingPlatform(Vector3(-11, 37.7, 228), Vector3(-11, 50, 270));
	createSweepingPlatform(Vector3(-17, 37.7, 228), Vector3(-17, 50, 270));
	createStaticPlatform(Vector3(-11, 50, 275), Vector3(0, 90, 0), Vector3(5, 1, 15), "testCube_pCube1.lrm");
	createStaticPlatform(Vector3(-11, 51.68, 282.02), Vector3(-20, 0, 0), Vector3(5, 1, 10), "testCube_pCube1.lrm");
	createStaticPlatform(Vector3(-11, 53.4, 289), Vector3(0, 0, 0), Vector3(5, 1, 5), "testCube_pCube1.lrm");
	/////////////////////////////////////////////////////////////////////////////////////

	Entity* skybox = engine->addEntity("SkyBox");
	if (skybox)
	{
		Material skyboxMat;
		skyboxMat.addTexture(L"Skybox_Texture.dds", true);
		engine->addComponent(skybox, "cube", new MeshComponent("Skybox_Mesh_pCube1.lrm", ShaderProgramsEnum::SKYBOX, skyboxMat));

	}
}

void Scene::updateScene(const float& dt)
{
}


// Private functions:
void Scene::createParisWheel(Vector3 position, float rotation, float rotationSpeed, int nrOfPlatforms)
{
	Engine* engine = &Engine::get();
	nrOfParisWheels++;

	Entity* ParisWheel = engine->addEntity("ParisWheel-" + std::to_string(nrOfParisWheels));
	if (ParisWheel)
	{
		engine->addComponent(ParisWheel, "mesh",
			new MeshComponent("ParisWheel.lrm", Material({ L"GrayTexture.png" })));
		ParisWheel->setPosition(position);
		ParisWheel->setRotation(0, XMConvertToRadians(rotation), 0);
	}

	Entity* center = engine->addEntity("Empty-" + std::to_string(nrOfParisWheels));
	if (center)
	{
		center->setRotation(0, XMConvertToRadians(rotation), XMConvertToRadians(90));
		center->setPosition(position);
	}

	float test = 360 / nrOfPlatforms;
	for (int i = 0; i < 360; i += test)
	{
		Entity* ParisWheelPlatform = engine->addEntity("ParisWheelPlatform-" + std::to_string(nrOfParisWheels) + "_" + std::to_string(i));
		if (ParisWheelPlatform)
		{
			engine->addComponent(ParisWheelPlatform, "mesh",
				new MeshComponent("ParisWheelPlatform.lrm", Material({ L"GrayTexture.png" })));

			ParisWheelPlatform->setRotation(0, XMConvertToRadians(rotation), 0);

			engine->createNewPhysicsComponent(ParisWheelPlatform, true);
			static_cast<PhysicsComponent*>(ParisWheelPlatform->getComponent("physics"))->makeKinematic();

			engine->addComponent(ParisWheelPlatform, "rotate",
				new RotateAroundComponent(center, center->getRotationMatrix(), ParisWheelPlatform, 12, rotationSpeed, i));


		}
	}
}

void Scene::createFlippingPlatform(Vector3 position, Vector3 rotation, float upTime, float downTime)
{
	Engine* engine = &Engine::get();
	nrOfFlippingPlatforms++;

	Entity* flippingPlatform = engine->addEntity("FlippingCube-" + std::to_string(nrOfFlippingPlatforms));
	if (flippingPlatform)
	{
		engine->addComponent(flippingPlatform, "mesh",
			new MeshComponent("SquarePlatform.lrm", Material({ L"GrayTexture.png" })));

		flippingPlatform->setPosition({ position });
		flippingPlatform->setRotation(XMConvertToRadians(rotation.x), XMConvertToRadians(rotation.y), XMConvertToRadians(rotation.z));

		engine->createNewPhysicsComponent(flippingPlatform, true);
		static_cast<PhysicsComponent*>(flippingPlatform->getComponent("physics"))->makeKinematic();

		engine->addComponent(flippingPlatform, "flipp",
			new FlippingComponent(flippingPlatform, upTime, downTime));
	}
}

void Scene::createStaticPlatform(Vector3 position, Vector3 rotation, Vector3 scale, std::string meshPath)
{
	Engine* engine = &Engine::get();
	m_nrOfStaticPlatforms++;

	Entity* staticPlatform = engine->addEntity("StaticPlatform-" + std::to_string(m_nrOfStaticPlatforms));
	if (staticPlatform)
	{
		engine->addComponent(staticPlatform, "mesh",
			new MeshComponent(meshPath.c_str(), Material({ L"GrayTexture.png" })));

		staticPlatform->setPosition(position);
		staticPlatform->setRotation(XMConvertToRadians(rotation.x), XMConvertToRadians(rotation.y), XMConvertToRadians(rotation.z));
		staticPlatform->setScale(scale);
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

		engine->createNewPhysicsComponent(staticPlatform);
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

void Scene::createSweepingPlatform(Vector3 startPos, Vector3 endPos)
{
	Engine* engine = &Engine::get();
	m_nrOfSweepingPlatforms++;

	Entity* sweepingPlatform = engine->addEntity("SweepingPlatform-" + std::to_string(m_nrOfSweepingPlatforms));
	if (sweepingPlatform)
	{
		engine->addComponent(sweepingPlatform, "mesh",
			new MeshComponent("SquarePlatform.lrm", Material({ L"GrayTexture.png" })));

		sweepingPlatform->setPosition(startPos);

		engine->createNewPhysicsComponent(sweepingPlatform, true);
		static_cast<PhysicsComponent*>(sweepingPlatform->getComponent("physics"))->makeKinematic();

		engine->addComponent(sweepingPlatform, "sweep",
			new SweepingComponent(sweepingPlatform, startPos, endPos, 5));
	}
}
