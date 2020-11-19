#include "3DPCH.h"
#include "Scene.h"
#include"PickupComponent.h"
#include"RotateComponent.h"
#include"Pickup.h"
#include"ParticleComponent.h"
#include"Particles\ScorePickupParticle.h"
#include"Renderer.h"

Scene::Scene()
{
	// Player
	m_player = Engine::get().getPlayerPtr();
	m_entities[PLAYER_ENTITY_NAME] = m_player->getPlayerEntity();
	m_sceneEntryPosition = { 0, 0, 0 };



	MeshComponent* meshComponent = dynamic_cast<MeshComponent*>(m_player->getPlayerEntity()->getComponent("mesh"));
	addMeshComponent(meshComponent);
}

Scene::~Scene()
{
	for (std::pair<std::string, Entity*> entityElement : m_entities)
	{
		if (entityElement.first != PLAYER_ENTITY_NAME)
		{
			delete entityElement.second;
			entityElement.second = nullptr;
		}
		else
		{
			entityElement.second = nullptr;
		}
	}

	m_entities.clear();

	//Clean boss
	if (m_boss)
		delete m_boss;
}

void Scene::createParticleEntity(void* particleComponent, Vector3 position)
{
	ParticleComponent* pc = static_cast<ParticleComponent*>(particleComponent);
	std::string name = "tempParticleEntity" + std::to_string(m_tempParticleID++);
	Entity* particleEntity = this->addEntity(name);
	particleEntity->setPosition(position);
	pc->setTransform(particleEntity);
	particleEntity->addComponent("particle", pc);
	pc->activate();

	m_tempParticleComponent.emplace_back(pc);
}

void Scene::sendPhysicsMessage(PhysicsData& physicsData, bool& removed)
{
	Entity* entity = m_entities[physicsData.entityIdentifier];
	if (physicsData.triggerType == TriggerType::PICKUP)
	{
		if (physicsData.associatedTriggerEnum == (int)PickupType::SCORE)
		{
			this->createParticleEntity(physicsData.pointer, entity->getTranslation());
		}
	}
	//std::vector<Component*> vec;
	//m_entities[physicsData.entityIdentifier]->getComponentsOfType(vec, ComponentType::MESH);
	/*for (size_t i = 0; i < vec.size(); i++)
	{
		int id = static_cast<MeshComponent*>(vec[i])->getRenderId();
		m_meshComponentMap.erase(id);
	}*/
	//if (physicsData.entityIdentifier != "")
	removeEntity(physicsData.entityIdentifier);
	removed = true;
}

void Scene::loadPickups()
{
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
	if (name == "")
		name = "score_" + std::to_string(m_nrOfScore++);

	Entity* pickupPtr = addEntity(name);

	ParticleComponent* particleComponent = new ParticleComponent(pickupPtr, new ScorePickupParticle());

	pickupPtr->setPosition(position);
	addComponent(pickupPtr, "mesh", new MeshComponent("star.lrm", ShaderProgramsEnum::TEMP_TEST));
	addComponent(pickupPtr, "pickup", new PickupComponent(PickupType::SCORE, 1.f * (float)tier, 6, particleComponent));
	static_cast<TriggerComponent*>(pickupPtr->getComponent("pickup"))->initTrigger(pickupPtr, { 1, 1, 1 });
	addComponent(pickupPtr, "rotate", new RotateComponent(pickupPtr, { 0.f, 1.f, 0.f }));
}

void Scene::addCheckpoint(const Vector3& position)
{
	Entity* checkPoint = addEntity("checkpoint"+std::to_string(m_nrOfCheckpoints++));
	addComponent(checkPoint, "mesh", new MeshComponent("Flag_pPlane2.lrm"));
	checkPoint->setPosition(position + Vector3(0,-0.2f,0));
	checkPoint->scale(1.5, 1.5, 1.5);

	addComponent(checkPoint, "checkpoint", new CheckpointComponent(checkPoint));
	static_cast<TriggerComponent*>(checkPoint->getComponent("checkpoint"))->initTrigger(checkPoint, { 4, 4, 4 });

	addComponent(checkPoint, "sound", new AudioComponent(L"OnPickup.wav", false, 0.1f));
}

void Scene::addBarrelDrop(Vector3 Position)
{
	Entity* rollingBarrel = addEntity("barrel"); //+ std::to_string(m_nrOfBarrelDrops++));

	if (rollingBarrel)
	{
		addComponent(rollingBarrel, "mesh",
			new MeshComponent("testCube_pCube1.lrm"));

		rollingBarrel->setPosition(Position);
		createNewPhysicsComponent(rollingBarrel, true, "", PxGeometryType::eSPHERE, "wood", true);
		static_cast<PhysicsComponent*>(rollingBarrel->getComponent("physics"))->setMass(100.0f);
		addComponent(rollingBarrel, "barrel", new BarrelComponent());
		static_cast<TriggerComponent*>(rollingBarrel->getComponent("barrel"))->initTrigger(rollingBarrel, { 1,1,1 });
		m_despawnBarrelTimer.restart();
		addedBarrel = true;
	}
}

void Scene::addSlowTrap(const Vector3& position, Vector3 scale)
{
	Entity* slowTrap = addEntity("trap"+std::to_string(m_nrOftraps++));
	addComponent(slowTrap,"mesh", new MeshComponent("testCube_pCube1.lrm"));
	slowTrap->setPosition(position + Vector3(0, -0.25f, 0));
	slowTrap->scale(scale);

	addComponent(slowTrap, "trap", new SlowTrapComponent(slowTrap, TrapType::SLOW));
	static_cast<TriggerComponent*>(slowTrap->getComponent("trap"))->initTrigger(slowTrap, { scale });

	addComponent(slowTrap, "sound", new AudioComponent(L"OnPickup.wav", false));

}

void Scene::addPushTrap(Vector3 wallPosition1, Vector3 wallPosition2, Vector3 triggerPosition)
{
	Entity* pushWall = addEntity("wall");
	if (pushWall)
	{
		addComponent(pushWall, "mesh",
			new MeshComponent("testCube_pCube1.lrm"));
		pushWall->setScale(Vector3(10, 5, 1));
		pushWall->rotate(0, 1.57, 0);

		createNewPhysicsComponent(pushWall, true, "", PxGeometryType::eBOX, "default", true);
		static_cast<PhysicsComponent*>(pushWall->getComponent("physics"))->makeKinematic();

		addComponent(pushWall, "sweep",
			new SweepingComponent(pushWall, Vector3(wallPosition1), Vector3(wallPosition2), 1, true));
	}

	Entity* pushWallTrigger = addEntity("pushTrigger");
	if (pushWallTrigger)
	{
		PushTrapComponent* pushComponentTrigger = new PushTrapComponent(pushWall);
		addComponent(pushWallTrigger, "mesh",
			new MeshComponent("testCube_pCube1.lrm", Material({ L"Wellcome.png" })));

		pushWallTrigger->setPosition(0, 18, 50);

		addComponent(pushWallTrigger, "trigger", pushComponentTrigger);
		pushComponentTrigger->initTrigger(pushWallTrigger, { 1,1,1 });
	}
}

void Scene::addPickup(const Vector3& position, const int tier, std::string name)
{
	int nrOfPickups = (int)PickupType::COUNT - 1; //-1 due to Score being in pickupTypes
	int pickupEnum = rand() % nrOfPickups;

	Entity* pickupPtr;
	if (name == "")
		name = "pickup_" + std::to_string(m_nrOfPickups++);

	pickupPtr = addEntity(name);
	pickupPtr->setPosition(position);
	addComponent(pickupPtr, "mesh", new MeshComponent("testCube_pCube1.lrm", ShaderProgramsEnum::TEMP_TEST));
	addComponent(pickupPtr, "pickup", new PickupComponent((PickupType)pickupEnum, 1.f, 6));
	static_cast<TriggerComponent*>(pickupPtr->getComponent("pickup"))->initTrigger(pickupPtr, { 1, 1, 1 });
	addComponent(pickupPtr, "rotate", new RotateComponent(pickupPtr, { 0.f, 1.f, 0.f }));
}

void Scene::loadLobby()
{
	m_sceneEntryPosition = Vector3(0.f, 2.f, 0.f);

	

	Entity* music = addEntity("lobbyMusic");
	if (music)
	{
		addComponent(music, "lobbyMusic", new AudioComponent(L"LobbyMusic.wav", true, 0.1f));
	}

	Entity* floor = addEntity("Floor");
	if (floor)
	{
		addComponent(floor, "mesh", new MeshComponent("testCube_pCube1.lrm",
			Material({ L"DarkGrayTexture.png" })));
		floor->scale({ 30, 1, 30 });
		floor->translate({ 0,-2,0 });
		createNewPhysicsComponent(floor, false, "", PxGeometryType::eBOX, "earth", false);
	}




	Entity* test = addEntity("test");
	if (test)
	{
		addComponent(test, "mesh",
			new MeshComponent("GlowCube.lrm",
			EMISSIVE,
			Material({ L"DarkGrayTexture.png", L"GlowTexture.png" })));

		test->setScale({ 5, 5, 5 });
		test->setPosition({ 9, 2, 10 });

		createNewPhysicsComponent(test, true);
		static_cast<PhysicsComponent*>(test->getComponent("physics"))->makeKinematic();

		addComponent(test, "flipp",
			new FlippingComponent(test, 1, 1));

		// 3D Audio Test
		test->addComponent("3Dsound", new AudioComponent(L"fireplace.wav", true, 3.f, 0.f, true, test));
	}

	Entity* test2 = addEntity("test2");
	if (test2)
	{
		addComponent(test2, "mesh",
			new MeshComponent("GlowCube.lrm",
				Material({ L"DarkGrayTexture.png" })));

		test2->setScale({ 5, 5, 5 });
		test2->setPosition({ -9, 2, 10 });

		createNewPhysicsComponent(test2, true);
		static_cast<PhysicsComponent*>(test2->getComponent("physics"))->makeKinematic();

		addComponent(test2, "flipp",
			new FlippingComponent(test2, 1, 1));
	}




	Entity* sign = addEntity("sign");
	if(sign)
	{
		addComponent(sign, "mesh",
			new MeshComponent("Wellcome_pCube15.lrm", Material({ L"Wellcome.png" })));
		sign->setScale(Vector3(10, 5, 0.2));

		createNewPhysicsComponent(sign, true,"",PxGeometryType::eBOX,"default", true);
		static_cast<PhysicsComponent*>(sign->getComponent("physics"))->makeKinematic();

		addComponent(sign, "sweep",
			new SweepingComponent(sign, Vector3(0, 5, 10), Vector3(0, 5.5, 10), 5));
	}


	Entity* skybox = addEntity("SkyBox");

	skybox->m_canCull = false;
	if (skybox)
	{
		Material skyboxMat;
		skyboxMat.addTexture(L"Skybox_Texture.dds", true);
		addComponent(skybox, "cube",
			new MeshComponent("skyboxCube.lrm", ShaderProgramsEnum::SKYBOX, skyboxMat));
				//Disable shadow casting
		dynamic_cast<MeshComponent*>(skybox->getComponent("cube"))->setCastsShadow(false);

	}

	createParisWheel(Vector3(30, 7, 0), 90, 30, 4);

	Entity* multiMatTest = addEntity("multiMatTest");
	if (multiMatTest)
	{
		addComponent(multiMatTest, "mesh", new MeshComponent("test_mesh_guy.lrm", 
			{
				DEFAULT,
				DEFAULT,
				TEMP_TEST,
				EMISSIVE
			}
			,
			{
				Material({ L"DarkGrayTexture.png" }),
				Material({ L"Green.jpg" }),
				Material({ L"GrayTexture.png" }),
				Material({ L"ibl_brdf_lut.png", L"ibl_brdf_lut.png" }),
				Material({ L"T_GridTestTex.bmp" }),
				Material({ L"T_tempTestDog.jpeg" }),
				Material({ L"ButtonStart.png" }),
				Material({ L"Controlls.png" }),
				Material({ L"DarkGrayTexture.png" }),
				Material({ L"GrayTexture.png" }),
				Material({ L"T_tempTestXWing.png" }),
				Material({ L"GrayTexture.png" })
			}
			));
		multiMatTest->translate({ 0,0,-6 });
		createNewPhysicsComponent(multiMatTest);
	}

	createSpotLight(Vector3(0, 21, -20), Vector3(10, 0, 0), Vector3(0.5, 0.1, 0.3), 3);
}

void Scene::loadScene(std::string path)
{
	m_sceneEntryPosition = Vector3(0.f, 2.f, 0.f);
	
	size_t dot = path.rfind('.', path.length());
	if (dot == std::string::npos)
		path.append(".lrl");
	
	std::ifstream fileStream(m_LEVELS_PATH + path, std::ifstream::in | std::ifstream::binary);

	// Check filestream failure
	if (!fileStream)
	{
		// Error message
		std::string errormsg("loadScene failed to open filestream: "); errormsg.append(path);
		ErrorLogger::get().logError(errormsg.c_str());
		// Properly clear and close file buffer
		fileStream.clear();
		fileStream.close();

		return;
	}

	// Read the amount of entities.
	int nrOfEntities;
	fileStream.read((char*)&nrOfEntities, sizeof(int));

	// Read how big the package is.
	int sizeOfPackage;
	fileStream.read((char*)&sizeOfPackage, sizeof(int));

	// Read the package.
	char* levelData = new char[sizeOfPackage];
	fileStream.read(levelData, sizeOfPackage);

	char overByte;
	fileStream.read(&overByte, 1);
	if (!fileStream.eof())
	{
		std::string errormsg("loadScene : Filestream did not reach end of: "); errormsg.append(path);
		ErrorLogger::get().logError(errormsg.c_str());
		return;
	}

	fileStream.close();

	int offset = 0;
	for (int i = 0; i < nrOfEntities; i++)
	{
		// Read name
		std::string entName = readStringFromChar(levelData, offset);

		Entity* newEntity = addEntity(entName);

		// Read transform values
		Vector3 pos = readDataFromChar<Vector3>(levelData, offset);
		Quaternion rotQuat = readDataFromChar<Quaternion>(levelData, offset);
		Vector3 scale = readDataFromChar<Vector3>(levelData, offset);

		newEntity->setPosition(pos);
		newEntity->setRotationQuat(rotQuat);
		newEntity->setScale(scale);

		bool isDynamic;
		PxGeometryType::Enum geoType;
		std::string physMatName;
		bool needsKinematics = false;
		bool needsDynamicPhys = false;
		bool hasPhysics = readDataFromChar<bool>(levelData, offset);
		if (hasPhysics)
		{
			isDynamic = readDataFromChar<bool>(levelData, offset);
			geoType = readDataFromChar<PxGeometryType::Enum>(levelData, offset);
			physMatName = readStringFromChar(levelData, offset);
			if (physMatName == "")
				physMatName = "default";
		}

		// An int with the number of components
		int nrOfComps = readDataFromChar<int>(levelData, offset);

		// Loop through all the comps, get their data and call addComponentFromFile
		for (int u = 0; u < nrOfComps; u++)
		{
			int compDataSize = readDataFromChar<int>(levelData, offset);

			char* compData = new char[compDataSize];
			memcpy(compData, levelData + offset, compDataSize);
			offset += compDataSize;

			addComponentFromFile(newEntity, compData, compDataSize, needsDynamicPhys, needsKinematics);

			delete[] compData;
		}

		if (needsDynamicPhys)
			isDynamic = true;

		// TEMP:
		if (hasPhysics && (newEntity->hasComponentsOfType(ComponentType::MESH) /*|| newEntity->hasComponentsOfType(ComponentType::ANIM_MESH)*/))
		{
			createNewPhysicsComponent(newEntity, isDynamic, "", geoType, physMatName);
			if(needsKinematics)
				static_cast<PhysicsComponent*>(newEntity->getComponent("physics"))->makeKinematic();
		}
			
	}

	delete[] levelData;
}

void Scene::addComponentFromFile(Entity* entity, char* compData, int sizeOfData, bool& needsDynamicPhys, bool& needsKinematicPhys)
{
	int offset = 0;

	ComponentType compType = readDataFromChar<ComponentType>(compData, offset);

	/*ComponentType compType;
	memcpy(&compType, compData + offset, sizeof(ComponentType));
	offset += sizeof(ComponentType);*/

	/*int compNameSize;
	memcpy(&compNameSize, compData + offset, sizeof(int));
	offset += sizeof(int);

	char* compName = new char[compNameSize];
	memcpy(compName, compData + offset, compNameSize);
	offset += compNameSize;*/

	std::string compName = readStringFromChar(compData, offset);

	char* paramData = new char[sizeOfData - offset];
	memcpy(paramData, compData + offset, sizeOfData - offset);

	Component* newComponent = nullptr;

	switch (compType)
	{
	case ComponentType::MESH:
		newComponent = new MeshComponent(paramData);
		break;
	case ComponentType::ANIM_MESH:
		newComponent = new AnimatedMeshComponent(paramData);
		break;
	case ComponentType::AUDIO:
		newComponent = new AudioComponent(paramData, entity);
		break;
	case ComponentType::PHYSICS:
		newComponent = new InvalidComponent();
		break;
	case ComponentType::CHARACTERCONTROLLER:
		newComponent = new InvalidComponent();
		break;
	case ComponentType::TRIGGER:
		newComponent = new InvalidComponent();
		break;
	case ComponentType::TEST:
		newComponent = new InvalidComponent();
		break;
	case ComponentType::INVALID:
		newComponent = new InvalidComponent();
		break;
	case ComponentType::UNASSIGNED:
		newComponent = new InvalidComponent();
		break;
	case ComponentType::ROTATEAROUND:
		newComponent = new InvalidComponent();
		break;
	case ComponentType::ROTATE:													//TODO: some components need to make physics kinematic.
		newComponent = new RotateComponent(entity, Vector3(1.0f, 0.0f, 0.f));
		needsDynamicPhys = true;
		needsKinematicPhys = true;
		break;
	case ComponentType::LIGHT:
		newComponent = new InvalidComponent();
		break;
	case ComponentType::SWEEPING:
		newComponent = new InvalidComponent();
		needsDynamicPhys = true;
		needsKinematicPhys = true;
		break;
	case ComponentType::FLIPPING:
		newComponent = new InvalidComponent();
		needsDynamicPhys = true;
		needsKinematicPhys = true;
		break;
	case ComponentType::CHECKPOINT:
		newComponent = new InvalidComponent();
		break;
	default:
		//newComponent = nullptr;
		newComponent = new InvalidComponent();
		break;
	}
	//assert(newComponent != nullptr);

	addComponent(entity, compName, newComponent);
	//addComponent(entity, "mesh", newComponent);
}

void Scene::loadTestLevel()
{
	Entity* entity;

	loadPickups();
	loadScore();


	addCheckpoint(Vector3(0, 9, 5));
	addCheckpoint(Vector3(14.54, 30, 105));
	addCheckpoint(Vector3(14.54, 30, 105));
	addCheckpoint(Vector3(-30, 40, 144));
	addCheckpoint(Vector3(-11, 40, 218.5));

	addSlowTrap(Vector3(0, 13, 30), Vector3(3,3,3));
	addPushTrap(Vector3(-5, 20, 58), Vector3(5, 20, 58), Vector3(0, 18, 50));
	
	m_sceneEntryPosition = Vector3(0.f, 8.1f, -1.f);


	Entity* barrelDropTrigger = addEntity("dropTrigger");
	if (barrelDropTrigger)
	{
		BarrelTriggerComponent* barrelComponentTrigger = new BarrelTriggerComponent();
		addComponent(barrelDropTrigger, "mesh",
			new MeshComponent("testCube_pCube1.lrm", Material({ L"Wellcome.png" })));

		barrelDropTrigger->setPosition(-30, 30, 105);

		addComponent(barrelDropTrigger, "trigger", barrelComponentTrigger);
		barrelComponentTrigger->initTrigger(barrelDropTrigger, { 1,1,1 });
	}



	Entity* floor = addEntity("floor"); // Floor:
	if (floor)
	{
		addComponent(floor, "mesh",
			new MeshComponent("testCube_pCube1.lrm", Material({ L"DarkGrayTexture.png" })));
		//new MeshComponent("testCube_pCube1.lrm", ShaderProgramsEnum::OBJECTSPACEGRID , ObjectSpaceGrid));

		floor->setPosition({ 0, 6, 0 });
		floor->scale({ 20, 2, 20 });
		createNewPhysicsComponent(floor, false, "", PxGeometryType::eBOX, "earth", false);
	}

	Entity* test = addEntity("test");
	if (test)
	{
		addComponent(test, "mesh",
			new MeshComponent("GlowCube.lrm",
				EMISSIVE,
				Material({ L"DarkGrayTexture.png", L"GlowTexture.png" })));

		test->setScale({ 5, 5, 5 });
		test->setPosition({ 0, 10, -10 });

		createNewPhysicsComponent(test, true);
		static_cast<PhysicsComponent*>(test->getComponent("physics"))->makeKinematic();
	}

	// Start:
	createStaticPlatform(Vector3(0, 6.5, 20), Vector3(0, 0, 0), Vector3(10, 1, 20), "testCube_pCube1.lrm");
	createStaticPlatform(Vector3(0, 8.5, 29.5), Vector3(0, 0, 0), Vector3(10, 3, 1), "testCube_pCube1.lrm");
	createStaticPlatform(Vector3(0, 10.5, 39), Vector3(0, 0, 0), Vector3(10, 1, 20), "testCube_pCube1.lrm");
	createStaticPlatform(Vector3(0, 14, 48.5), Vector3(0, 0, 0), Vector3(10, 6, 1), "testCube_pCube1.lrm");
	createStaticPlatform(Vector3(0, 17, 58), Vector3(0, 0, 0), Vector3(10, 1, 20), "testCube_pCube1.lrm");
	createStaticPlatform(Vector3(0, 19, 66), Vector3(0, 0, 0), Vector3(10, 4, 4), "testCube_pCube1.lrm");
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
	createStaticPlatform(Vector3(-14, 27.5, 105), Vector3(0, 0, 0), Vector3(10, 1, 5), "testCube_pCube1.lrm");
	createStaticPlatform(Vector3(-30, 27.5, 107.5), Vector3(0, 0, 0), Vector3(5, 1, 10), "testCube_pCube1.lrm");
	createStaticPlatform(Vector3(-30, 32.6, 126.4), Vector3(-20, 0, 0), Vector3(5, 1, 30), "testCube_pCube1.lrm");
	createStaticPlatform(Vector3(-30, 37.7, 145.32), Vector3(0, 0, 0), Vector3(5, 1, 10), "testCube_pCube1.lrm");
	createStaticPlatform(Vector3(-30, 42, 160), Vector3(0, 0, 0), Vector3(5, 15, 1), "testCube_pCube1.lrm");
	createFlippingPlatform(Vector3(-36, 37.7, 160), Vector3(0, 0, 0), 1, 2);
	createFlippingPlatform(Vector3(-24, 37.7, 160), Vector3(0, 180, 0), 2, 1);
	createStaticPlatform(Vector3(-30, 37.7, 175), Vector3(0, 0, 0), Vector3(5, 1, 10), "testCube_pCube1.lrm");
	createStaticPlatform(Vector3(-23.67, 37.7, 185.3), Vector3(0, 45, 0), Vector3(5, 1, 20), "testCube_pCube1.lrm");
	createStaticPlatform(Vector3(-23.67, 45, 185.3), Vector3(0, 45, 0), Vector3(10, 10.5, 1), "testCube_pCube1.lrm");
	// End:
	createFlippingPlatform(Vector3(-11, 37.7, 200), Vector3(0, 180, 0), 2, 2);
	createStaticPlatform(Vector3(-11, 37.7, 215), Vector3(0, 0, 0), Vector3(5, 1, 10), "testCube_pCube1.lrm");
	createStaticPlatform(Vector3(-11, 37.7, 222.5), Vector3(0, 90, 0), Vector3(5, 1, 15), "testCube_pCube1.lrm");


	createSweepingPlatform(Vector3(-5, 37.7, 228), Vector3(-5, 43.85, 246));
	createSweepingPlatform(Vector3(-17, 43.85, 246), Vector3(-17, 37.7, 228));
	createSweepingPlatform(Vector3(-5, 43.85, 251), Vector3(-5, 50, 270));
	createSweepingPlatform(Vector3(-17, 50, 270), Vector3(-17, 43.85, 251));

	createStaticPlatform(Vector3(-11, 50, 275), Vector3(0, 90, 0), Vector3(5, 1, 15), "testCube_pCube1.lrm");
	createStaticPlatform(Vector3(-11, 51.68, 282.02), Vector3(-20, 0, 0), Vector3(5, 1, 10), "testCube_pCube1.lrm");
	createStaticPlatform(Vector3(-11, 53.4, 289), Vector3(0, 0, 0), Vector3(5, 1, 5), "testCube_pCube1.lrm");


	Entity* clownMask = addEntity("ClownMask");
	if (clownMask)
	{
		addComponent(clownMask, "mesh",
			new MeshComponent("ClownMask_ClownEye_R1.lrm", Material({ L"DarkGrayTexture.png" })));

		clownMask->setPosition(Vector3(-11.5, 60, 290));
		clownMask->setRotation(XMConvertToRadians(7), XMConvertToRadians(180), XMConvertToRadians(0));
	}
	Entity* goalTrigger = addEntity("trigger");
	if (goalTrigger)
	{
		addComponent(goalTrigger, "mesh",
			new MeshComponent("testCube_pCube1.lrm", Material({ L"BlackTexture.png" })));
		goalTrigger->setPosition(-11.5, 60.563, 292.347);
		goalTrigger->setScale(13.176, 15.048, 1);
		goalTrigger->setRotation(XMConvertToRadians(-10.102), XMConvertToRadians(0), XMConvertToRadians(0));

		addComponent(goalTrigger, "trigger",
			new TriggerComponent());

		TriggerComponent* tc = static_cast<TriggerComponent*>(goalTrigger->getComponent("trigger"));
		tc->initTrigger(goalTrigger, XMFLOAT3(9.0f, 8.0f, 0.5f));
		tc->setEventData(TriggerType::EVENT, (int)EventType::SWAPSCENE);
		tc->setIntData((int)ScenesEnum::ARENA);
	}
	/////////////////////////////////////////////////////////////////////////////////////

	Entity* skybox = addEntity("SkyBox");
	skybox->m_canCull = false;
	if (skybox)
	{
		Material skyboxMat;
		skyboxMat.addTexture(L"Skybox_Texture.dds", true);
		addComponent(skybox, "cube", new MeshComponent("skyboxCube.lrm", ShaderProgramsEnum::SKYBOX, skyboxMat));
		//Disable shadow casting
		dynamic_cast<MeshComponent*>(skybox->getComponent("cube"))->setCastsShadow(false);
	}
	
	/////////////////////////////////////////////////////////////////////////////////////

	// MUSIC
	Entity* music = addEntity("music");
	addComponent(music, "music", new AudioComponent(L"BestSongPLS.wav", true, 0.1f));

	// Lights
	// - Point Light
	createSpotLight(Vector3(16.54, 21, 92.7), Vector3(-90, 0, 0), Vector3(0, 1, 0), 3);
	createSpotLight(Vector3(-30, 35, 159.7), Vector3(-90, 0, 0), Vector3(0, 1, 0), 3);
	createSpotLight(Vector3(-32, 39, 176), Vector3(0, 45, 0), Vector3(0, 1, 0), 0.1);

	createSpotLight(Vector3(-5, 22, 68.5), Vector3(0, -45, 0), Vector3(0, 0, 1), 0.1);
	createSpotLight(Vector3(8.5, 60, 159.5), Vector3(90, 0, 0), Vector3(0, 0, 1), 0.2);

	createSpotLight(Vector3(-11, 50, 275), Vector3(-35, 0, 0), Vector3(1, 0, 0), 0.3);
}

void Scene::loadArena()
{
	Engine* engine = &Engine::get();
	Entity* entity;

	m_sceneEntryPosition = Vector3(0, 0, 0);

	Entity* bossEnt = addEntity("boss");
	if (bossEnt)
	{
		AnimatedMeshComponent* animMeshComp = new AnimatedMeshComponent("platformerGuy.lrsm", ShaderProgramsEnum::SKEL_ANIM);
		animMeshComp->addAndPlayBlendState({ {"platformer_guy_idle", 0}, {"Running4.1", 1} }, "runOrIdle", 0.f, true);
		bossEnt->addComponent("mesh", animMeshComp);
		addMeshComponent(animMeshComp);
		bossEnt->scale({ 4, 4, 4 });
		bossEnt->translate({ 10,8,0 });

		m_boss = new Boss();
		m_boss->Attach(this);
		m_boss->initialize(bossEnt, false);
		m_boss->addAction(new MoveToAction(bossEnt, m_boss, Vector3(-30, 9, 0), 13.f));
		m_boss->addAction(new ShootProjectileAction(bossEnt, m_boss, 3, 0));
		m_boss->addAction(new MoveToAction(bossEnt, m_boss, Vector3(-30, 9, -30), 13.f));
		m_boss->addAction(new ShootProjectileAction(bossEnt, m_boss, 3, 0));
		m_boss->addAction(new MoveToAction(bossEnt, m_boss, Vector3(30, 9, -30), 13.f));
		m_boss->addAction(new ShootProjectileAction(bossEnt, m_boss, 3, 0));
		m_boss->addAction(new MoveToAction(bossEnt, m_boss, Vector3(0, 9, 0), 13.f));
		m_boss->addAction(new ShootProjectileAction(bossEnt, m_boss, 3, 0));

		Physics::get().Attach(m_boss, true, false);
	}

	Material gridTest = Material({ L"BlackGridBlueLines.png" });
	entity = addEntity("floor");
	if (entity)
	{
		addComponent(entity, "mesh", new MeshComponent("testCube_pCube1.lrm", gridTest));
		entity->scale({ 157, 2, 157 });
		entity->setPosition({ 0,-2,0 });
		createNewPhysicsComponent(entity, false, "", PxGeometryType::eBOX, "earth", false);
	}
	createStaticPlatform(Vector3(0, 24, 78), Vector3(0, 0, 0), Vector3(157, 50, 1), "testCube_pCube1.lrm", L"DarkGrayTexture.png");
	createStaticPlatform(Vector3(0, 24, -78), Vector3(0, 0, 0), Vector3(157, 50, 1), "testCube_pCube1.lrm", L"DarkGrayTexture.png");
	createStaticPlatform(Vector3(78, 24, 0), Vector3(0, 0, 0), Vector3(1, 50, 157), "testCube_pCube1.lrm", L"DarkGrayTexture.png");
	createStaticPlatform(Vector3(-78, 24, 0), Vector3(0, 0, 0), Vector3(1, 50, 157), "testCube_pCube1.lrm", L"DarkGrayTexture.png");

	entity = addEntity("bossSign");
	if (entity)
	{
		addComponent(entity, "mesh", new MeshComponent("BossSign_pCube20.lrm", Material({ L"BossSign.png" })));
		entity->setPosition({ 0, -8.3, 8 });
	}


	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Entity* clownMask = addEntity("ClownMask");
	if (clownMask)
	{
		addComponent(clownMask, "mesh",
			new MeshComponent("ClownMask_ClownEye_R1.lrm", Material({ L"DarkGrayTexture.png" })));

		clownMask->setPosition(Vector3(0, 10, 73));
		clownMask->setRotation(XMConvertToRadians(7), XMConvertToRadians(180), XMConvertToRadians(0));
	}
	Entity* goalTrigger = addEntity("trigger");
	if (goalTrigger)
	{
		addComponent(goalTrigger, "mesh",
			new MeshComponent("testCube_pCube1.lrm", Material({ L"BlackTexture.png" })));
		goalTrigger->setPosition(0, 10.563, 75.347);
		goalTrigger->setScale(13.176, 15.048, 1);
		goalTrigger->setRotation(XMConvertToRadians(-10.102), XMConvertToRadians(0), XMConvertToRadians(0));

		addComponent(goalTrigger, "trigger",
			new TriggerComponent());

		TriggerComponent* tc = static_cast<TriggerComponent*>(goalTrigger->getComponent("trigger"));
		tc->initTrigger(goalTrigger, XMFLOAT3(9.0f, 8.0f, 0.5f));
		tc->setEventData(TriggerType::EVENT, (int)EventType::SWAPSCENE);
		tc->setIntData((int)ScenesEnum::LOBBY);
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Entity* skybox = addEntity("SkyBox");
	skybox->m_canCull = false;
	if (skybox)
	{
		Material skyboxMat;
		skyboxMat.addTexture(L"Skybox_Texture.dds", true);
		addComponent(skybox, "cube", new MeshComponent("skyboxCube.lrm", ShaderProgramsEnum::SKYBOX, skyboxMat));
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Light
	createPointLight({ 0.f, 10.f, 0.f }, { 1.f, 0.f, 0.f }, 5.f);

	// Audio test
	/*Entity* audioTestDelete = addEntity("deleteTestAudio");
	addComponent(audioTestDelete, "deleteSound", new AudioComponent(L"PickupTunnels.wav", true, 0.5f));
	delete m_entities["deleteTestAudio"];
	m_entities.erase("deleteTestAudio");*/

	// Night Ambient Sound
	Entity* audioTest = addEntity("audioTest");
	addComponent(audioTest, "testSound", new AudioComponent(L"NightAmbienceSimple_02.wav", true, 0.2f));
	m_nightSlide = 0.01f;
	m_nightVolume = 0.2f;
}

// Private functions:
void Scene::createParisWheel(Vector3 position, float rotation, float rotationSpeed, int nrOfPlatforms)
{
	nrOfParisWheels++;

	Entity* ParisWheel = addEntity("ParisWheel-" + std::to_string(nrOfParisWheels));
	if (ParisWheel)
	{
		addComponent(ParisWheel, "mesh",
			new MeshComponent("ParisWheel.lrm", Material({ L"DarkGrayTexture.png" })));
			//new MeshComponent("ParisWheel.lrm", ShaderProgramsEnum::OBJECTSPACEGRID, ObjectSpaceGrid));
		ParisWheel->setPosition(position);
		ParisWheel->setRotation(0, XMConvertToRadians(rotation), 0);
	}

	Entity* center = addEntity("Empty-" + std::to_string(nrOfParisWheels));
	if (center)
	{
		center->setRotation(0, XMConvertToRadians(rotation), XMConvertToRadians(90));
		center->setPosition(position);
	}

	float test = 360 / nrOfPlatforms;
	for (int i = 0; i < 360; i += test)
	{
		Entity* ParisWheelPlatform = addEntity("ParisWheelPlatform-" + std::to_string(nrOfParisWheels) + "_" + std::to_string(i));
		if (ParisWheelPlatform)
		{
			addComponent(ParisWheelPlatform, "mesh",
				new MeshComponent("ParisWheelPlatform.lrm", Material({ L"DarkGrayTexture.png" })));
				//new MeshComponent("ParisWheelPlatform.lrm", ShaderProgramsEnum::OBJECTSPACEGRID, ObjectSpaceGrid));

			ParisWheelPlatform->setRotation(0, XMConvertToRadians(rotation), 0);

			createNewPhysicsComponent(ParisWheelPlatform, true);
			static_cast<PhysicsComponent*>(ParisWheelPlatform->getComponent("physics"))->makeKinematic();

			addComponent(ParisWheelPlatform, "rotate",
				new RotateAroundComponent(center, center->getRotationMatrix(), ParisWheelPlatform, 12, rotationSpeed, i));
		}
	}
}

void Scene::createFlippingPlatform(Vector3 position, Vector3 rotation, float upTime, float downTime)
{
	nrOfFlippingPlatforms++;

	Entity* flippingPlatform = addEntity("FlippingCube-" + std::to_string(nrOfFlippingPlatforms));
	if (flippingPlatform)
	{
		addComponent(flippingPlatform, "mesh",
			new MeshComponent("SquarePlatform.lrm", Material({ L"DarkGrayTexture.png" })));
			//new MeshComponent("SquarePlatform.lrm", ShaderProgramsEnum::OBJECTSPACEGRID, ObjectSpaceGrid));

		flippingPlatform->setPosition({ position });
		flippingPlatform->setRotation(XMConvertToRadians(rotation.x), XMConvertToRadians(rotation.y), XMConvertToRadians(rotation.z));

		createNewPhysicsComponent(flippingPlatform, true);
		static_cast<PhysicsComponent*>(flippingPlatform->getComponent("physics"))->makeKinematic();

		addComponent(flippingPlatform, "flipp",
			new FlippingComponent(flippingPlatform, upTime, downTime));
	}
}

void Scene::createStaticPlatform(Vector3 position, Vector3 rotation, Vector3 scale, std::string meshPath, std::wstring texPath)
{
	m_nrOfStaticPlatforms++;

	Entity* staticPlatform = addEntity("StaticPlatform-" + std::to_string(m_nrOfStaticPlatforms));
	if (staticPlatform)
	{
		addComponent(staticPlatform, "mesh",
			new MeshComponent(meshPath.c_str(), Material({ L"DarkGrayTexture.png" })));
		//	new MeshComponent(meshPath.c_str(), ShaderProgramsEnum::OBJECTSPACEGRID, ObjectSpaceGrid));


		staticPlatform->setPosition(position);// -Vector3(0, 25, 0));
		staticPlatform->setRotation(XMConvertToRadians(rotation.x), XMConvertToRadians(rotation.y), XMConvertToRadians(rotation.z));
		staticPlatform->setScale(scale);

		createNewPhysicsComponent(staticPlatform);
	}
}

void Scene::loadMaterialTest()
{
	Entity* entity;

	std::wstring materialNames[7] = {
	L"Bronze_Quilt/bronze_quilt",
	L"Coastline_Flat_Stone_Wall_Mixed/coastline_flat_stone_wall_mixed",
	L"Japanese_Temple_Wood_Ceiling/japanese_temple_wood_ceiling",
	L"Metal_Grinded/metal_grinded",
	L"Steel_Battered/steel_battered",
	L"Terracotta_Glossy/terracotta_glossy",
	L"Wood_American_Cherry/wood_american_cherry"
	};

	for (size_t i = 0; i < 7; i++)
	{
		std::string currentSphereName = ("PBRSphereMaterial" + std::to_string(i));
		m_entities[currentSphereName] = addEntity(currentSphereName);
		if (m_entities[currentSphereName])
		{
			entity = m_entities[currentSphereName];
			Material PBRMatTextured;
			PBRMatTextured.addTexture(L"skybox1IR.dds", true);
			PBRMatTextured.addTexture(L"skybox1.dds", true);
			PBRMatTextured.addTexture(L"ibl_brdf_lut.png");

			PBRMatTextured.addTexture((materialNames[i] + L"_Base_Color.dds").c_str());
			PBRMatTextured.addTexture((materialNames[i] + L"_Normal.dds").c_str());
			PBRMatTextured.addTexture((materialNames[i] + L"_Roughness.dds").c_str());
			PBRMatTextured.addTexture((materialNames[i] + L"_Metallic.dds").c_str());
			PBRMatTextured.addTexture((materialNames[i] + L"_Ambient_Occlusion.dds").c_str());

			PBRMatTextured.setTextured(1);

			addComponent(entity, "mesh", new MeshComponent("Sphere_2m_Sphere.lrm", ShaderProgramsEnum::PBRTEST, { PBRMatTextured }));

			float moveDistance = -5.f;
			entity->translate({ moveDistance * i + 30.f, 2.f, 20.f });
			entity->rotate({ 1.5708, 0.f, 0.f });
		}
	}

	int xCounter = 0;
	int yCounter = 0;

	for (size_t i = 0; i < 25; i++)
	{
		std::string currentSphereName = ("PBRSphere" + std::to_string(i));
		m_entities[currentSphereName] = addEntity(currentSphereName);
		if (m_entities[currentSphereName])
		{
			entity = m_entities[currentSphereName];
			Material PBRMatUntextured;
			PBRMatUntextured.addTexture(L"skybox1IR.dds", true);
			PBRMatUntextured.addTexture(L"skybox1.dds", true);
			PBRMatUntextured.addTexture(L"ibl_brdf_lut.png");

			xCounter++;

			if (i % 5 == 0)
			{
				yCounter++;
				xCounter = 0;
			}
			PBRMatUntextured.setMetallic(yCounter * 0.2f - 0.2f);
			PBRMatUntextured.setRoughness(xCounter * 0.18 + 0.1);
			PBRMatUntextured.setTextured(0);

			addComponent(entity, "mesh", new MeshComponent("Sphere_2m_Sphere.lrm", ShaderProgramsEnum::PBRTEST, PBRMatUntextured));

			float moveDistance = 5.f;
			entity->translate({ moveDistance * xCounter, moveDistance * yCounter - 3.f, 0.f });
			entity->rotate({ 1.5708, 0.f, 0.f });
		}
	}

	Entity* skybox = addEntity("SkyBox");
	skybox->m_canCull = false;
	if (skybox)
	{
		Material skyboxMat;
		skyboxMat.addTexture(L"skybox1.dds", true);
		addComponent(skybox, "cube", new MeshComponent("skyboxCube.lrm", ShaderProgramsEnum::SKYBOX, skyboxMat));

	}

	XMVECTOR pointLightPositions[4] =
	{
		{ 0.f, 5.f, 10.f},
		{ 20.f, 5.f, 10.f},
		{ 0.f, 25.f, 10.f},
		{ 20.f, 25.f, 10.f},
	};

	float lightIntensity = 500.f;

	float pointLightIntensities[4] =
	{
		lightIntensity,
		lightIntensity,
		lightIntensity,
		lightIntensity,
	};

	for (size_t i = 0; i < 4; i++)
	{
		std::string currentPointLightName = ("PointLight" + std::to_string(i));
		m_entities[currentPointLightName] = addEntity(currentPointLightName);
		if (m_entities[currentPointLightName])
		{
			entity = m_entities[currentPointLightName];
			std::string currentPointLightComponentName = ("PointLightTestPointLight" + std::to_string(i));
			addComponent(m_entities[currentPointLightName], currentPointLightComponentName, new LightComponent());
			dynamic_cast<LightComponent*>(m_entities[currentPointLightName]->getComponent(currentPointLightComponentName))->setColor(XMFLOAT3(1, 1, 1));
			dynamic_cast<LightComponent*>(m_entities[currentPointLightName]->getComponent(currentPointLightComponentName))->setIntensity(pointLightIntensities[i]);
			//engine->addComponent(entity, "mesh", new MeshComponent("testCube_pCube1.lrm", Material({ L"T_CircusTent_D.png" })));
			entity->translate(pointLightPositions[i]);
		}
	}
}

void Scene::updateScene(const float& dt)
{
	if (m_boss)
	{
		m_boss->update(dt);

		ShootProjectileAction* ptr = dynamic_cast<ShootProjectileAction*>(m_boss->getCurrentAction());

		if (ptr)
			ptr->setTarget(static_cast<CharacterControllerComponent*>(m_player->getPlayerEntity()->getComponent("CCC"))->getFootPosition() + Vector3(0, 1, 0));
	
		//Check projectiles and their lifetime so they do not continue on forever in case they missed.
		checkProjectiles();

		for (int i = 0; i < deferredPointInstantiationList.size(); i++)
		{
			addScore(deferredPointInstantiationList[i]);
		}

		deferredPointInstantiationList.clear();
	}


	if (addedBarrel)
	{

		if (m_despawnBarrelTimer.timeElapsed() >= 3)
		{
			static_cast<PhysicsComponent*>(m_entities["barrel"]->getComponent("physics"))->clearForce();
			static_cast<PhysicsComponent*>(m_entities["barrel"]->getComponent("physics"))->setPosition({ -30, 50, 130 });
			m_despawnBarrelTimer.restart();
		}
		addedBarrel = false;
	}

	for (int i = 0; i < m_tempParticleComponent.size(); i++)
	{
		if (!m_tempParticleComponent[i]->getParticlePointer())
		{
			this->removeEntity(m_tempParticleComponent[i]->getParentEntityIdentifier());
			m_tempParticleComponent.erase(m_tempParticleComponent.begin() + i);
		}
	}

	// AUDIO TEST
	/*m_nightVolume += dt * m_nightSlide;
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
	ac->setVolume(m_nightVolume);*/
}

Vector3 Scene::getEntryPosition()
{
	return m_sceneEntryPosition;
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
	if (m_entities.find(identifier) != m_entities.end())// If one with that name is already found
		return nullptr;

	m_entities[identifier] = new Entity(identifier);
	return m_entities[identifier];
}

void Scene::removeEntity(std::string identifier)
{
	std::vector<Component*> meshCompVec;
	m_entities[identifier]->getComponentsOfType(meshCompVec,ComponentType::MESH);
	for (auto meshComponent : meshCompVec)
	{
		int index = static_cast<MeshComponent*>(meshComponent)->getRenderId();
		m_meshComponentMap.erase(index);
		m_entities[identifier]->removeComponent(meshComponent);
	}
	delete m_entities[identifier];
	m_entities.erase(identifier);
}

bool Scene::addComponent(Entity* entity, std::string componentIdentifier, Component* component)
{
	entity->addComponent(componentIdentifier, component);

	if (component->getType() == ComponentType::MESH || component->getType() == ComponentType::ANIM_MESH)
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

void Scene::bossEventUpdate(BossMovementType type, BossStructures::BossActionData data)
{

	if(type == BossMovementType::ShootProjectile)
		createProjectile(data.origin, data.direction, data.speed);

	if (type == BossMovementType::DropPoints)
	{
		Entity* projectile = static_cast<Entity*>(data.pointer0);
		//addScore(data.origin+Vector3(0,2,0));
		deferredPointInstantiationList.push_back(data.origin + Vector3(0, 1, 0));
		
		ProjectileComponent* component = dynamic_cast<ProjectileComponent*>(projectile->getComponent("projectile"));
		m_projectiles.erase(component->m_id);
		removeEntity(projectile->getIdentifier());
	}
		

}

void Scene::createSweepingPlatform(Vector3 startPos, Vector3 endPos)
{
	m_nrOfSweepingPlatforms++;

	Entity* sweepingPlatform = addEntity("SweepingPlatform-" + std::to_string(m_nrOfSweepingPlatforms));
	if (sweepingPlatform)
	{
		addComponent(sweepingPlatform, "mesh",
			new MeshComponent("SquarePlatform.lrm", Material({ L"DarkGrayTexture.png" })));
			//new MeshComponent("SquarePlatform.lrm", ShaderProgramsEnum::OBJECTSPACEGRID, ObjectSpaceGrid));

		sweepingPlatform->setPosition(startPos);

		createNewPhysicsComponent(sweepingPlatform, true);
		static_cast<PhysicsComponent*>(sweepingPlatform->getComponent("physics"))->makeKinematic();

		addComponent(sweepingPlatform, "sweep",
			new SweepingComponent(sweepingPlatform, startPos, endPos, 5));
	}
}

void Scene::createSpotLight(Vector3 position, Vector3 rotation, Vector3 color, float intensity)
{
	m_nrOfSpotLight++;

	Entity* sLight = addEntity("spotLight-" + std::to_string(m_nrOfSpotLight));
	if (sLight)
	{
		addComponent(sLight, "spot-" + std::to_string(m_nrOfSpotLight), new SpotLightComponent());
		sLight->setPosition(position);
		sLight->setRotation(XMConvertToRadians(rotation.x), XMConvertToRadians(rotation.y), XMConvertToRadians(rotation.z));
		dynamic_cast<LightComponent*>(sLight->getComponent("spot-" + std::to_string(m_nrOfSpotLight)))->setColor(XMFLOAT3(color));
		dynamic_cast<LightComponent*>(sLight->getComponent("spot-" + std::to_string(m_nrOfSpotLight)))->setIntensity(intensity);
	}
}

void Scene::createPointLight(Vector3 position, Vector3 color, float intensity)
{
	m_nrOfPointLight++;

	Entity* pLight = addEntity("pointLight-" + std::to_string(m_nrOfPointLight));
	if (pLight)
	{
		LightComponent* pointLight = new LightComponent();
		pointLight->setColor(XMFLOAT3(color));
		pointLight->setIntensity(intensity);
		addComponent(pLight, "point-" + std::to_string(m_nrOfPointLight), pointLight);
		pLight->setPosition(position);
	}
}

void Scene::createProjectile(Vector3 origin, Vector3 dir, float speed)
{
	m_nrOfProjectiles++;

	Entity* projectileEntity = addEntity("Projectile-" + std::to_string(m_nrOfProjectiles));
	if (projectileEntity)
	{
		addComponent(projectileEntity, "mesh",
			new MeshComponent("Sphere_2m_Sphere.lrm", EMISSIVE, Material({ L"red.png", L"red.png" })));
		//new MeshComponent("SquarePlatform.lrm", ShaderProgramsEnum::OBJECTSPACEGRID, ObjectSpaceGrid));

		projectileEntity->setPosition(origin);
		projectileEntity->scale(0.5f, 0.5f, 0.5f);

		createNewPhysicsComponent(projectileEntity, true);
		static_cast<PhysicsComponent*>(projectileEntity->getComponent("physics"))->makeKinematic();

		addComponent(projectileEntity, "projectile",
			new ProjectileComponent(projectileEntity, projectileEntity, origin, dir, speed, 20.f));
		
		static_cast<TriggerComponent*>(projectileEntity->getComponent("projectile"))->initTrigger(projectileEntity, { 0.5f, 0.5f, 0.5f });	


		static_cast<ProjectileComponent*>(projectileEntity->getComponent("projectile"))->m_id = m_nrOfProjectiles;
		m_projectiles[static_cast<ProjectileComponent*>(projectileEntity->getComponent("projectile"))->m_id] = projectileEntity;
	}
}

void Scene::checkProjectiles()
{
	std::vector<UINT> idsToRemove;

	for (auto projectileEntity : m_projectiles)
	{
		ProjectileComponent* comp = static_cast<ProjectileComponent*>(projectileEntity.second->getComponent("projectile"));
		if (comp->m_timer.timeElapsed() > comp->getLifeTime())
		{
			idsToRemove.push_back(projectileEntity.first);
		}
	}

	//can not remove mid-loop
	for (int i = 0; i < idsToRemove.size(); i++)
	{
		removeEntity(m_projectiles[idsToRemove[i]]->getIdentifier());
		m_projectiles.erase(idsToRemove[i]);
	}
}
