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
	m_sceneID = Physics::get().getNewSceneID();

	MeshComponent* meshComponent = dynamic_cast<MeshComponent*>(m_player->getPlayerEntity()->getComponent("mesh"));
	addMeshComponent(meshComponent);
	setScoreVec();
	sortScore();

}

Scene::~Scene()
{
	for (std::pair<std::string, Entity*> entityElement : m_entities)
	{
		if (entityElement.first != PLAYER_ENTITY_NAME)
		{
			delete entityElement.second;
			entityElement.second = nullptr;
			//m_entities[entityElement.first] = nullptr;
		}
		else
		{
			entityElement.second = nullptr;
		}
	}

	m_entities.clear();

	//Clean boss
	if (m_boss)
	{
		for (int i = 0; i < m_boss->m_bossSegments.size(); i++)
		{
			BossSegment* segment = m_boss->m_bossSegments.at(i);
			segment->Detach(this);
		}

		m_boss->Detach(this);
		delete m_boss;
	}
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

void Scene::loadMainMenu(Scene* sceneObject, bool* finished)
{
	sceneObject->m_sceneEntryPosition = Vector3(0.f, 2.f, 0.f);

	/*Entity* music = sceneObject->addEntity("lobbyMusic");
	if (music)
	{
		sceneObject->addComponent(music, "Music", new AudioComponent(L"LobbyMusic.wav", true, 0.1f));
	}*/

	Entity* floor = sceneObject->addEntity("Floor");
	if (floor)
	{
		sceneObject->addComponent(floor, "mesh", new MeshComponent("testCube_pCube1.lrm",
			Material({ L"DarkGrayTexture.png" })));
		floor->scale({ 30, 1, 30 });
		floor->translate({ 0,-2,0 });
		sceneObject->createNewPhysicsComponent(floor, false, "", PxGeometryType::eBOX, "earth", false);
	}

	Entity* test = sceneObject->addEntity("test");
	if (test)
	{
		sceneObject->addComponent(test, "mesh",
			new MeshComponent("GlowCube.lrm",
				EMISSIVE,
				Material({ L"DarkGrayTexture.png", L"GlowTexture.png" })));

		test->setScale({ 5, 5, 5 });
		test->setPosition({ 9, 2, 10 });

		sceneObject->createNewPhysicsComponent(test, true);
		static_cast<PhysicsComponent*>(test->getComponent("physics"))->makeKinematic();

		sceneObject->addComponent(test, "flipp",
			new FlippingComponent(test, 1, 1));
	}

	Entity* sign = sceneObject->addEntity("sign");
	if (sign)
	{
		sceneObject->addComponent(sign, "mesh",
			new MeshComponent("Wellcome_pCube15.lrm", Material({ L"Wellcome.png" })));
		sign->setScale(Vector3(10.f, 5.f, 0.2f));

		sceneObject->createNewPhysicsComponent(sign, true, "", PxGeometryType::eBOX, "default", true);
		static_cast<PhysicsComponent*>(sign->getComponent("physics"))->makeKinematic();

		sceneObject->addComponent(sign, "sweep",
			new SweepingComponent(sign, Vector3(0.f, 5.f, 10.f), Vector3(0.f, 5.5f, 10.f), 5.f));
	}



	Entity* skybox = sceneObject->addEntity("SkyBox");
	skybox->m_canCull = false;
	if (skybox)
	{
		Material skyboxMat;
		skyboxMat.addTexture(L"Skybox_Texture.dds", true);
		sceneObject->addComponent(skybox, "cube",
			new MeshComponent("skyboxCube.lrm", ShaderProgramsEnum::SKYBOX, skyboxMat));

		//Disable shadow casting
		dynamic_cast<MeshComponent*>(skybox->getComponent("cube"))->setCastsShadow(false);
	}

	sceneObject->createParisWheel(Vector3(30.f, 7.f, 0.f), 90.f, 30.f, 4);

	sceneObject->createSpotLight(Vector3(0.f, 21.f, -20.f), Vector3(10.f, 0.f, 0.f), Vector3(0.5f, 0.1f, 0.3f), 3.f);
	*finished = true;
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
	addPickup(Vector3(-30.f, 30.f, 105.f));
	addPickup(Vector3(8.5f, 40.f, 172.f));
}

void Scene::loadScore()
{
	addScore(Vector3(0.f, 9.f, 20.f));
	addScore(Vector3(0.f, 15.f, 45.f));

	addScore(Vector3(-16.54f, 15.5f, 105.f));
	addScore(Vector3(16.54f, 30.f, 105.f));
	addScore(Vector3(-30.f, 40.f, 146.f));
	addScore(Vector3(8.5f, 18.f, 159.5f));
	addScore(Vector3(-11.f, 40.f, 222.5f));
}

void Scene::addScore(const Vector3& position, const int tier, std::string name)
{
	if (name == "")
		name = "score_" + std::to_string(m_nrOfScore++);

	Material mat;
	mat.setEmissiveStrength(100.f);
	Entity* pickupPtr = addEntity(name);

	ParticleComponent* particleComponent = new ParticleComponent(pickupPtr, new ScorePickupParticle());

	pickupPtr->setPosition(position);
	addComponent(pickupPtr, "mesh", new MeshComponent("star.lrm", ShaderProgramsEnum::TEMP_TEST, mat));
	addComponent(pickupPtr, "pickup", new PickupComponent(PickupType::SCORE, 1.f * (float)tier, 6, particleComponent));
	static_cast<TriggerComponent*>(pickupPtr->getComponent("pickup"))->initTrigger( m_sceneID, pickupPtr, { 1, 1, 1 });
	addComponent(pickupPtr, "rotate", new RotateComponent(pickupPtr, { 0.f, 1.f, 0.f }));
}

void Scene::addCheckpoint(const Vector3& position)
{
	Entity* checkPoint = addEntity("checkpoint"+std::to_string(m_nrOfCheckpoints++));
	addComponent(checkPoint, "mesh", new MeshComponent("Flag_pPlane2.lrm"));
	checkPoint->setPosition(position + Vector3(0,1.35f,0));
	checkPoint->scale(1.5, 1.5, 1.5);

	addComponent(checkPoint, "checkpoint", new CheckpointComponent(checkPoint));
	static_cast<TriggerComponent*>(checkPoint->getComponent("checkpoint"))->initTrigger( m_sceneID, checkPoint, { 4, 4, 4 });

	addComponent(checkPoint, "sound", new AudioComponent(L"OnPickup.wav", false, 0.1f));

}

void Scene::addBarrelDrop(Vector3 Position)
{
	Entity* rollingBarrel = addEntity("barrel");

	if (rollingBarrel)
	{
		addComponent(rollingBarrel, "mesh",
			new MeshComponent("Barrel.lrm", Material({ L"DarkGrayTexture.png" })));

		rollingBarrel->setPosition(Position);
		createNewPhysicsComponent(rollingBarrel, true, "", PxGeometryType::eSPHERE, "wood", true);
		static_cast<PhysicsComponent*>(rollingBarrel->getComponent("physics"))->setMass(100.0f);
		addComponent(rollingBarrel, "barrel", new BarrelComponent());
		static_cast<TriggerComponent*>(rollingBarrel->getComponent("barrel"))->initTrigger( m_sceneID, rollingBarrel, { 1,1,1 });
		m_despawnBarrelTimer.restart();
		addedBarrel = true;
	}
}

int Scene::getSceneID()
{
	return this->m_sceneID;
}


void Scene::addSlowTrap(const Vector3& position, Vector3 scale, Vector3 hitBox)
{
	Entity* slowTrap = addEntity("trap"+std::to_string(m_nrOftraps++));
	addComponent(slowTrap,"mesh", new MeshComponent("testCube_pCube1.lrm"));
	slowTrap->setPosition(position + Vector3(0, -0.25f, 0));
	slowTrap->scale(scale);

	addComponent(slowTrap, "trap", new SlowTrapComponent(slowTrap, TrapType::SLOW));
	static_cast<TriggerComponent*>(slowTrap->getComponent("trap"))->initTrigger(m_sceneID,slowTrap, {hitBox});

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
		pushWall->rotate(0.f, 1.57f, 0.f);

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
			new MeshComponent("testCube_pCube1.lrm", Material()));

		pushWallTrigger->setPosition(0, 18, 50);

		addComponent(pushWallTrigger, "trigger", pushComponentTrigger);
		pushComponentTrigger->initTrigger( m_sceneID, pushWallTrigger, { 1,1,1 });
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
	static_cast<TriggerComponent*>(pickupPtr->getComponent("pickup"))->initTrigger( m_sceneID, pickupPtr, { 1, 1, 1 });
	addComponent(pickupPtr, "rotate", new RotateComponent(pickupPtr, { 0.f, 1.f, 0.f }));
}

void Scene::loadLobby(Scene* sceneObject, bool* finished)
{
	sceneObject->m_sceneEntryPosition = Vector3(0.f, 2.f, 0.f);

	/*Entity* music = sceneObject->addEntity("lobbyMusic");
	if (music)
	{
		sceneObject->addComponent(music, "lobbyMusic", new AudioComponent(L"LobbyMusic.wav", true, 0.1f));
	}*/

	Entity* floor = sceneObject->addEntity("Floor");
	if (floor)
	{
		sceneObject->addComponent(floor, "mesh", new MeshComponent("testCube_pCube1.lrm",
			Material({ L"DarkGrayTexture.png" })));
		floor->scale({ 30, 1, 30 });
		floor->translate({ 0,-2,0 });
		sceneObject->createNewPhysicsComponent(floor, false, "", PxGeometryType::eBOX, "earth", false);
	}

	Material mat({ L"DarkGrayTexture.png", L"GlowTexture.png" });
	Entity* test = sceneObject->addEntity("test"); // Emissive Test Material 1
	if (test)
	{
		mat.setEmissiveStrength(100.f);
		sceneObject->addComponent(test, "mesh",
			new MeshComponent("GlowCube.lrm",
				EMISSIVE,
				mat
			)
		);

		test->setScale({ 5, 5, 5 });
		test->setPosition({ 8, 2, 5 });

		sceneObject->createNewPhysicsComponent(test, true);
		static_cast<PhysicsComponent*>(test->getComponent("physics"))->makeKinematic();

		sceneObject->addComponent(test, "flipp",
			new FlippingComponent(test, 1, 1));

		// 3D Audio Test
		test->addComponent("3Dsound", new AudioComponent(L"fireplace.wav", true, 3.f, 0.f, true, test));
	}

	Entity* test2 = sceneObject->addEntity("test2"); // Emissive Test Material 2
	if (test2)
	{
		mat = Material({ L"DarkGrayTexture.png", L"GlowTexture.png" });
		mat.setEmissiveStrength(50.f);
		sceneObject->addComponent(test2, "mesh",
			new MeshComponent("GlowCube.lrm",
				EMISSIVE,
				mat
			)
		);

		test2->setScale({ 5, 5, 5 });
		test2->setPosition({ 0, 2, 5 });

		sceneObject->createNewPhysicsComponent(test2, true);
		static_cast<PhysicsComponent*>(test2->getComponent("physics"))->makeKinematic();

		sceneObject->addComponent(test2, "flipp",
			new FlippingComponent(test2, 1, 1));
	}

	Entity* test3 = sceneObject->addEntity("test3"); // Emissive Test Material 3
	if (test3)
	{
		mat = Material({ L"DarkGrayTexture.png", L"GlowTexture.png" });
		mat.setEmissiveStrength(20.f);
		sceneObject->addComponent(test3, "mesh",
			new MeshComponent("GlowCube.lrm",
				EMISSIVE,
				mat
			)
		);

		test3->setScale({ 5, 5, 5 });
		test3->setPosition({ -8, 2, 5 });

		sceneObject->createNewPhysicsComponent(test3, true);
		static_cast<PhysicsComponent*>(test3->getComponent("physics"))->makeKinematic();

		sceneObject->addComponent(test3, "flipp",
			new FlippingComponent(test3, 1, 1));
	}

	Entity* test4 = sceneObject->addEntity("test4"); // Emissive Test Material 4
	if (test4)
	{
		mat = Material({ L"DarkGrayTexture.png", L"ButtonStart.png" });
		mat.setEmissiveStrength(90.f);
		sceneObject->addComponent(test4, "mesh",
			new MeshComponent("GlowCube.lrm",
				EMISSIVE,
				mat
			)
		);

		test4->setScale({ 5, 5, 5 });
		test4->setPosition({ -16, 2, 5 });

		sceneObject->createNewPhysicsComponent(test4, true);
		static_cast<PhysicsComponent*>(test4->getComponent("physics"))->makeKinematic();

		sceneObject->addComponent(test4, "flipp",
			new FlippingComponent(test4, 1, 1));
	}

	Entity* sign = sceneObject->addEntity("sign");
	if(sign)
	{
		sceneObject->addComponent(sign, "mesh",
			new MeshComponent("Wellcome_pCube15.lrm", Material({ L"Wellcome.png" })));
		sign->setScale(Vector3(10.f, 5.f, 0.2f));

		sceneObject->createNewPhysicsComponent(sign, true,"",PxGeometryType::eBOX,"default", true);
		static_cast<PhysicsComponent*>(sign->getComponent("physics"))->makeKinematic();

		sceneObject->addComponent(sign, "sweep",
			new SweepingComponent(sign, Vector3(0.f, 5.f, 10.f), Vector3(0.f, 5.5f, 10.f), 5.f));
	}


	Entity* skybox = sceneObject->addEntity("SkyBox");

	skybox->m_canCull = false;
	if (skybox)
	{
		Material skyboxMat;
		skyboxMat.addTexture(L"Skybox_Texture.dds", true);
		sceneObject->addComponent(skybox, "cube",
			new MeshComponent("skyboxCube.lrm", ShaderProgramsEnum::SKYBOX, skyboxMat));
				//Disable shadow casting
		dynamic_cast<MeshComponent*>(skybox->getComponent("cube"))->setCastsShadow(false);

	}

	sceneObject->createParisWheel(Vector3(30, 7, 0), 90, 30, 4);

	Entity* multiMatTest = sceneObject->addEntity("multiMatTest");
	if (multiMatTest)
	{
		sceneObject->addComponent(multiMatTest, "mesh", new MeshComponent("test_mesh_guy.lrm",
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
				Material({ L"ibl_brdf_lut.png", L"ibl_brdf_lut.png" }, MATERIAL_CONST_BUFFER({1.0f, 0.5f, 0.f, 0, 100.f})),
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
		sceneObject->createNewPhysicsComponent(multiMatTest);
	}

	sceneObject->createSpotLight(Vector3(0.f, 21.f, -20.f), Vector3(10.f, 0.f, 0.f), Vector3(0.5f, 0.1f, 0.3f), 3.f);
	*finished = true; //Inform the main thread that the loading is complete.
}

void Scene::loadScene(Scene* sceneObject, std::string path, bool* finished)
{
	std::cout << "Started scene loading ================================" << std::endl;
	sceneObject->m_sceneEntryPosition = Vector3(0.f, 2.f, 0.f);

	size_t dot = path.rfind('.', path.length());
	if (dot == std::string::npos)
		path.append(".lrl");

	std::ifstream fileStream(sceneObject->m_LEVELS_PATH + path, std::ifstream::in | std::ifstream::binary);

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

		Entity* newEntity = sceneObject->addEntity(entName);

		//assert(newEntity);

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
		bool allMeshesHavePhys = false;
		if (hasPhysics)
		{
			isDynamic = readDataFromChar<bool>(levelData, offset);
			geoType = readDataFromChar<PxGeometryType::Enum>(levelData, offset);
			physMatName = readStringFromChar(levelData, offset);
			if (physMatName == "")
				physMatName = "default";
			allMeshesHavePhys = readDataFromChar<bool>(levelData, offset);
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

			sceneObject->addComponentFromFile(newEntity, compData, compDataSize, needsDynamicPhys, needsKinematics);


			delete[] compData;
		}

		if (needsDynamicPhys)
			isDynamic = true;

		if (allMeshesHavePhys)
		{
			std::vector<Component*> tempComponentVector;
			newEntity->getComponentsOfType(tempComponentVector, ComponentType::MESH);
			newEntity->getComponentsOfType(tempComponentVector, ComponentType::ANIM_MESH);

			for (int m = 0; m < tempComponentVector.size(); m++)
			{
				if (hasPhysics)
				{
					MeshComponent* mPtr = (MeshComponent*)tempComponentVector.at(m);
					sceneObject->createNewPhysicsComponent(newEntity, isDynamic, mPtr->getFilePath(), geoType, physMatName);
					if (needsKinematics)
						static_cast<PhysicsComponent*>(newEntity->getComponent("physics"))->makeKinematic();
				}
			}
		}
		else
		{
			if (hasPhysics && (newEntity->hasComponentsOfType(ComponentType::MESH) || newEntity->hasComponentsOfType(ComponentType::ANIM_MESH)))
			{
				sceneObject->createNewPhysicsComponent(newEntity, isDynamic,"", geoType, physMatName);
				if (needsKinematics)
					static_cast<PhysicsComponent*>(newEntity->getComponent("physics"))->makeKinematic();
			}
		}

		/*
		if (hasPhysics && (newEntity->hasComponentsOfType(ComponentType::MESH) || newEntity->hasComponentsOfType(ComponentType::ANIM_MESH)) )
		{
			sceneObject->createNewPhysicsComponent(newEntity, isDynamic, "", geoType, physMatName);
			if(needsKinematics)
				static_cast<PhysicsComponent*>(newEntity->getComponent("physics"))->makeKinematic();
		}
		*/
	}

	int nrOfPrefabs = 0;
	nrOfPrefabs = readDataFromChar<int>(levelData, offset);
	for (int i = 0; i < nrOfPrefabs; i++)
	{
		int prefabDataSize = readDataFromChar<int>(levelData, offset);

		char* prefabData = new char[prefabDataSize];
		memcpy(prefabData, levelData + offset, prefabDataSize);
		offset += prefabDataSize;

		sceneObject->addPrefabFromFile(prefabData);

		delete[] prefabData;
	}

	/*
	Entity* skybox = sceneObject->addEntity("SkyBox");
	if (skybox)
	{
		skybox->m_canCull = false;
		
		Material skyboxMat;
		skyboxMat.addTexture(L"Skybox_Texture.dds", true);
		sceneObject->addComponent(skybox, "cube", new MeshComponent("skyboxCube.lrm", ShaderProgramsEnum::SKYBOX, skyboxMat));
		//Disable shadow casting
		dynamic_cast<MeshComponent*>(skybox->getComponent("cube"))->setCastsShadow(false);
	}
	*/

	delete[] levelData;
	*finished = true; //Inform the main thread that the loading is complete.
}

void Scene::addComponentFromFile(Entity* entity, char* compData, int sizeOfData, bool& needsDynamicPhys, bool& needsKinematicPhys)
{
	int offset = 0;

	ComponentType compType = readDataFromChar<ComponentType>(compData, offset);

	std::string compName = readStringFromChar(compData, offset);

	char* paramData = new char[sizeOfData - offset];
	memcpy(paramData, compData + offset, sizeOfData - offset);

	Component* newComponent = nullptr;
																						// TODO: edvin will add a swinging component
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
	case ComponentType::ROTATEAROUND: // This simply doesn't work, and won't unless we make a system for identifying other entities in the scene
		newComponent = new RotateAroundComponent(paramData, entity, entity);
		needsDynamicPhys = true;
		needsKinematicPhys = true;
		break;
	case ComponentType::ROTATE:
		newComponent = new RotateComponent(entity, paramData);
		needsDynamicPhys = true;
		needsKinematicPhys = true;
		break;
	case ComponentType::LIGHT:
	{
		LightType lightType = readDataFromChar<LightType>(compData, offset);
		switch (lightType)
		{
		case Point:
			newComponent = new LightComponent(paramData);
			break;
		case Spot:
			newComponent = new SpotLightComponent(paramData);
			break;
		}
		break;
	}
	case ComponentType::SWEEPING:
		newComponent = new SweepingComponent(paramData, entity);
		needsDynamicPhys = true;
		needsKinematicPhys = true;
		break;
	case ComponentType::FLIPPING:
		newComponent = new FlippingComponent(paramData, entity);
		needsDynamicPhys = true;
		needsKinematicPhys = true;
		break;
	case ComponentType::CHECKPOINT:
		newComponent = new InvalidComponent();
		break;
	default:
		newComponent = new InvalidComponent();
		break;
	}

	addComponent(entity, compName, newComponent);
}

void Scene::addPrefabFromFile(char* params)
{
	int offset = 0;

	PrefabType type;
	type = (PrefabType)readDataFromChar<int>(params, offset);

	Vector3 pos = readDataFromChar<Vector3>(params, offset);

	switch (type)
	{
	case PARIS_WHEEL:
	{
		float param1, param2; int param3;
		param1 = readDataFromChar<float>(params, offset);
		param2 = readDataFromChar<float>(params, offset);
		param3 = readDataFromChar<int>(params, offset);
		createParisWheel(pos, param1, param2, param3);
		break;
	}

	case FLIPPING_PLATFORM:
	{
		Vector3 param1; float param2, param3;
		param1 = readDataFromChar<Vector3>(params, offset);
		param2 = readDataFromChar<float>(params, offset);
		param3 = readDataFromChar<float>(params, offset);
		createFlippingPlatform(pos, param1, param2, param3);
		break;
	}
	case SWEEPING_PLATFORM:
	{
		createSweepingPlatform(pos, readDataFromChar<Vector3>(params, offset));
		break;
	}
	case PICKUP:
		addPickup(pos, readDataFromChar<int>(params, offset));
		break;
	case SCORE:
		addScore(pos, readDataFromChar<int>(params, offset));
		break;
	case pfCHECKPOINT:
		addCheckpoint(pos);
		break;
	case SLOWTRAP:
	{
		Vector3 scale, hitBox;
		scale = readDataFromChar<Vector3>(params, offset);
		hitBox = readDataFromChar<Vector3>(params, offset);
		addSlowTrap(pos, scale, hitBox);
		break;
	}
	case PUSHTRAP:
	{
		Vector3 wallpos1, wallpos2;
		wallpos1 = readDataFromChar<Vector3>(params, offset);
		wallpos2 = readDataFromChar<Vector3>(params, offset);
		addPushTrap(wallpos1, wallpos2, pos);
		break;
	}
	case BARRELDROP:
		addBarrelDrop(pos);
		break;
	case GOAL_TRIGGER:

		break;

	case SWINGING_HAMMER:
	{
		Vector3 rot = readDataFromChar<Vector3>(params, offset);
		float swingSpeed = readDataFromChar<float>(params, offset);
		createSwingingHammer(pos, rot, swingSpeed);
		break;
	}
	case pfSKYBOX:
	{
		std::string strTexName = readStringFromChar(params, offset);
		std::wstring texName = std::wstring(strTexName.begin(), strTexName.end());
		createSkybox(texName);
		break;
	}
	}
	
}


void Scene::loadTestLevel(Scene* sceneObject, bool* finished)
{
	sceneObject->loadPickups();
	sceneObject->loadScore();
	sceneObject->addCheckpoint(Vector3(0.f, 9.f, 5.f));
	sceneObject->addCheckpoint(Vector3(14.54f, 30.f, 105.f));
	sceneObject->addCheckpoint(Vector3(14.54f, 30.f, 105.f));
	sceneObject->addCheckpoint(Vector3(-30.f, 40.f, 144.f));
	sceneObject->addCheckpoint(Vector3(-11.f, 40.f, 218.5f));
	sceneObject->createRespawnBox(Vector3(0, 18, 15), Vector3(1, 1, 1), true);

	sceneObject->addSlowTrap(Vector3(0.f, 13.f, 30.f), Vector3(3.f,3.f,3.f), Vector3(1.5f, 1.5f, 1.5f));
	sceneObject->addPushTrap(Vector3(-5.f, 20.f, 58.f), Vector3(5.f, 20.f, 58.f), Vector3(0.f, 18.f, 50.f));

	sceneObject->m_sceneEntryPosition = Vector3(0.f, 8.1f, -1.f);
	
	Entity* endSceneTrigger = sceneObject->addEntity("endSceneTrigger");
	if (endSceneTrigger)
	{
		sceneObject->addComponent(endSceneTrigger, "mesh",
			new MeshComponent("testCube_pCube1.lrm", Material({ L"BlackTexture.png" })));
		endSceneTrigger->setPosition(9,7,0);
		endSceneTrigger->setScale(2, 2, 2);

		sceneObject->addComponent(endSceneTrigger, "endSceneTrigger",
			new TriggerComponent());

		TriggerComponent* tc = static_cast<TriggerComponent*>(endSceneTrigger->getComponent("endSceneTrigger"));
		tc->initTrigger(sceneObject->m_sceneID, endSceneTrigger, XMFLOAT3(2.0f,2.0f,2.0f));
		tc->setEventData(TriggerType::EVENT, (int)EventType::SWAPSCENE);
		tc->setIntData((int)ScenesEnum::ENDSCENE);
	}

	sceneObject->createTimedSweepPlatform({ 0, 7.5f, 5.f }, { 0, 7.5f, 10.f }, true, 2.5f);




	Entity* barrelDropTrigger = sceneObject->addEntity("dropTrigger");
	if (barrelDropTrigger)
	{
		BarrelTriggerComponent* barrelComponentTrigger = new BarrelTriggerComponent();
		sceneObject->addComponent(barrelDropTrigger, "mesh",
			new MeshComponent("testCube_pCube1.lrm", Material()));

		barrelDropTrigger->setPosition(-30.f, 30.f, 105.f);

		sceneObject->addComponent(barrelDropTrigger, "trigger", barrelComponentTrigger);
		barrelComponentTrigger->initTrigger(sceneObject->m_sceneID, barrelDropTrigger, { 1.f,1.f,1.f });
	}

	/*for (int i = 0; i < 3; i++)
	{
		Entity* stressObject = sceneObject->addEntity("stressObject" + std::to_string(i));
		if (stressObject)
		{
			sceneObject->addComponent(stressObject, "mesh",
				new MeshComponent("highPolyTestSpider_highPolyTestSpider1.lrm", Material({ L"Wellcome.png" })));

			stressObject->setPosition(0, 6*i*0.15f + 6, 0);

		}
	}*/



	Entity* floor = sceneObject->addEntity("floor"); // Floor:
	if (floor)
	{
		sceneObject->addComponent(floor, "mesh",
			new MeshComponent("testCube_pCube1.lrm", Material({ L"DarkGrayTexture.png" })));
		//new MeshComponent("testCube_pCube1.lrm", ShaderProgramsEnum::OBJECTSPACEGRID , ObjectSpaceGrid));

		floor->setPosition({ 0, 6, 0 });
		floor->scale({ 20, 2, 20 });
		sceneObject->createNewPhysicsComponent(floor, false, "", PxGeometryType::eBOX, "earth", false);
	}

	Entity* test = sceneObject->addEntity("test");
	if (test)
	{
		sceneObject->addComponent(test, "mesh",
			new MeshComponent("GlowCube.lrm",
				EMISSIVE,
				Material({ L"DarkGrayTexture.png", L"GlowTexture.png" })));

		test->setScale({ 5, 5, 5 });
		test->setPosition({ 0, 10, -10 });

		sceneObject->createNewPhysicsComponent(test, true);
		static_cast<PhysicsComponent*>(test->getComponent("physics"))->makeKinematic();

		sceneObject->addComponent(test, "3Dsound", new AudioComponent(L"fireplace.wav", true, 3.f, 0.f, true, test));
	}

	sceneObject->createSwingingHammer(Vector3(0, 9.3, 20), Vector3(0, 0, 0), 1);
	sceneObject->createStaticPlatform(Vector3(0, 13, 20), Vector3(0, 0, 0), Vector3(4, 1, 4), "testCube_pCube1.lrm");
	// Start:
	sceneObject->createStaticPlatform(Vector3(0, 6.5, 20), Vector3(0, 0, 0), Vector3(4, 1, 20), "testCube_pCube1.lrm");
	sceneObject->createStaticPlatform(Vector3(0, 8.5, 29.5), Vector3(0, 0, 0), Vector3(10, 3, 1), "testCube_pCube1.lrm");
	sceneObject->createStaticPlatform(Vector3(0, 10.5, 39), Vector3(0, 0, 0), Vector3(10, 1, 20), "testCube_pCube1.lrm");
	sceneObject->createStaticPlatform(Vector3(0, 14, 48.5), Vector3(0, 0, 0), Vector3(10, 6, 1), "testCube_pCube1.lrm");
	sceneObject->createStaticPlatform(Vector3(0, 17, 58), Vector3(0, 0, 0), Vector3(10, 1, 20), "testCube_pCube1.lrm");
	sceneObject->createStaticPlatform(Vector3(0, 19, 66), Vector3(0, 0, 0), Vector3(10, 4, 4), "testCube_pCube1.lrm");
	// Left:
	sceneObject->createStaticPlatform(Vector3(-10.2f, 20.5f, 73.2f), Vector3(0, -45, 0), Vector3(5, 1, 20), "testCube_pCube1.lrm");
	sceneObject->createStaticPlatform(Vector3(-16.54f, 20.5f, 81.f), Vector3(0, 0, 0), Vector3(5, 1, 5), "testCube_pCube1.lrm");
	sceneObject->createStaticPlatform(Vector3(-16.54f, 17, 83.f), Vector3(0, 0, 0), Vector3(5, 6, 1), "testCube_pCube1.lrm");
	sceneObject->createStaticPlatform(Vector3(-16.54f, 13.5f, 102.5f), Vector3(0, 0, 0), Vector3(5, 1, 40), "testCube_pCube1.lrm");
	//createStaticPlatform	(Vector3(-16.54, 21.75, 105),	Vector3(0, 0, 0),		Vector3(10, 10.5, 1),	"testCube_pCube1.lrm");
	sceneObject->createStaticPlatform(Vector3(-16.54f, 18, 128.f), Vector3(0, 0, 0), Vector3(1, 1, 1), "SquarePlatform.lrm");
	sceneObject->createStaticPlatform(Vector3(-14, 23, 135), Vector3(0, 45, 0), Vector3(1, 1, 1), "SquarePlatform.lrm");
	sceneObject->createStaticPlatform(Vector3(-7, 28, 137.5f), Vector3(0, 90, 0), Vector3(1, 1, 1), "SquarePlatform.lrm");
	sceneObject->createStaticPlatform(Vector3(6, 28, 137.5f), Vector3(0, 0, 0), Vector3(10, 1, 5), "testCube_pCube1.lrm");
	sceneObject->createStaticPlatform(Vector3(8.5f, 28, 142.5f), Vector3(0, 0, 0), Vector3(5, 1, 5), "testCube_pCube1.lrm");
	sceneObject->createParisWheel(Vector3(8.5f, 28, 159.5f), 0.f, 30.f, 4);
	sceneObject->createStaticPlatform(Vector3(8.5f, 37.7f, 175.f), Vector3(0.f, 0.f, 0.f), Vector3(5, 1, 10), "testCube_pCube1.lrm");
	sceneObject->createStaticPlatform(Vector3(7.47f, 37.7f, 180.f), Vector3(0.f, -45, 0.f), Vector3(5, 1, 5), "testCube_pCube1.lrm");
	sceneObject->createFlippingPlatform(Vector3(2.2f, 42.f, 185.5f), Vector3(0.f, -225, 0.f), 3, 3);
	sceneObject->createStaticPlatform(Vector3(-3.18f, 37.7f, 190.61f), Vector3(0.f, -45.f, 0.f), Vector3(5, 1, 5), "testCube_pCube1.lrm");
	// Right:
	sceneObject->createStaticPlatform(Vector3(10.2f, 20.5f, 73.2f), Vector3(0, 45, 0), Vector3(5, 1, 20), "testCube_pCube1.lrm");
	sceneObject->createStaticPlatform(Vector3(16.54f, 20.5f, 86.f), Vector3(0, 0, 0), Vector3(5, 1, 15), "testCube_pCube1.lrm");
	sceneObject->createStaticPlatform(Vector3(16.54f, 24.f, 93.f), Vector3(0, 0, 0), Vector3(5, 6, 1), "testCube_pCube1.lrm");
	sceneObject->createStaticPlatform(Vector3(16.54f, 27.5f, 100.f), Vector3(0, 0, 0), Vector3(5, 1, 15), "testCube_pCube1.lrm");
	sceneObject->createStaticPlatform(Vector3(11.54f, 27.5f, 105.f), Vector3(0, 0, 0), Vector3(5, 1, 5), "testCube_pCube1.lrm");
	//createStaticPlatform	(Vector3(2, 30, 105),			Vector3(0, 0, 0),		Vector3(1, 1, 1),		"SquarePlatform.lrm");
	sceneObject->createStaticPlatform(Vector3(-14.f, 27.5f, 105.f), Vector3(0, 0, 0), Vector3(10, 1, 5), "testCube_pCube1.lrm");
	sceneObject->createStaticPlatform(Vector3(-30.f, 27.5f, 107.5f), Vector3(0, 0, 0), Vector3(5, 1, 10), "testCube_pCube1.lrm");
	sceneObject->createStaticPlatform(Vector3(-30.f, 32.6f, 126.4f), Vector3(-20, 0, 0), Vector3(5, 1, 30), "testCube_pCube1.lrm");
	sceneObject->createStaticPlatform(Vector3(-30.f, 37.7f, 145.32f), Vector3(0, 0, 0), Vector3(5, 1, 10), "testCube_pCube1.lrm");
	sceneObject->createStaticPlatform(Vector3(-30.f, 42.f, 160.f), Vector3(0, 0, 0), Vector3(5, 15, 1), "testCube_pCube1.lrm");
	sceneObject->createFlippingPlatform(Vector3(-36.f, 37.7f, 160.f), Vector3(0, 0, 0), 1, 2);
	sceneObject->createFlippingPlatform(Vector3(-24.f, 37.7f, 160.f), Vector3(0, 180, 0), 2, 1);
	sceneObject->createStaticPlatform(Vector3(-30.f, 37.7f, 175.f), Vector3(0, 0, 0), Vector3(5, 1, 10), "testCube_pCube1.lrm");
	sceneObject->createStaticPlatform(Vector3(-23.67f, 37.7f, 185.3f), Vector3(0, 45, 0), Vector3(5, 1, 20), "testCube_pCube1.lrm");
	sceneObject->createStaticPlatform(Vector3(-23.67f, 45.f, 185.3f), Vector3(0, 45, 0), Vector3(10, 10.5, 1), "testCube_pCube1.lrm");
	// End:
	sceneObject->createFlippingPlatform(Vector3(-11.f, 37.7f, 200.f), Vector3(0, 180, 0), 2, 2);
	sceneObject->createStaticPlatform(Vector3(-11.f, 37.7f, 215.f), Vector3(0, 0, 0), Vector3(5, 1, 10), "testCube_pCube1.lrm");
	sceneObject->createStaticPlatform(Vector3(-11.f, 37.7f, 222.5f), Vector3(0, 90, 0), Vector3(5, 1, 15), "testCube_pCube1.lrm");


	sceneObject->createSweepingPlatform(Vector3(-5.f, 37.7f, 228.f), Vector3(-5.f, 43.85f, 246.f));
	sceneObject->createSweepingPlatform(Vector3(-17.f, 43.85f, 246.f), Vector3(-17.f, 37.7f, 228.f));
	sceneObject->createSweepingPlatform(Vector3(-5.f, 43.85f, 251.f), Vector3(-5.f, 50.f, 270.f));
	sceneObject->createSweepingPlatform(Vector3(-17.f, 50.f, 270.f), Vector3(-17.f, 43.85f, 251.f));

	sceneObject->createStaticPlatform(Vector3(-11.f, 50.f, 275.f), Vector3(0.f, 90.f, 0.f), Vector3(5.f, 1.f, 15.f), "testCube_pCube1.lrm");
	sceneObject->createStaticPlatform(Vector3(-11.f, 51.68f, 282.02f), Vector3(-20.f, 0.f, 0.f), Vector3(5.f, 1.f, 10.f), "testCube_pCube1.lrm");
	sceneObject->createStaticPlatform(Vector3(-11.f, 53.4f, 289.f), Vector3(0.f, 0.f, 0.f), Vector3(5.f, 1.f, 5.f), "testCube_pCube1.lrm");


	Entity* clownMask = sceneObject->addEntity("ClownMask");
	if (clownMask)
	{
		sceneObject->addComponent(clownMask, "mesh",
			new MeshComponent("ClownMask_ClownEye_R1.lrm", Material({ L"DarkGrayTexture.png" })));

		clownMask->setPosition(Vector3(-11.5f, 60.f, 290.f));
		clownMask->setRotation(XMConvertToRadians(7.f), XMConvertToRadians(180.f), XMConvertToRadians(0.f));
	}
	Entity* goalTrigger = sceneObject->addEntity("trigger");
	if (goalTrigger)
	{
		sceneObject->addComponent(goalTrigger, "mesh",
			new MeshComponent("testCube_pCube1.lrm", Material({ L"BlackTexture.png" })));
		goalTrigger->setPosition(-11.5f, 60.563f, 292.347f);
		goalTrigger->setScale(13.176f, 15.048f, 1.f);
		goalTrigger->setRotation(XMConvertToRadians(-10.102f), XMConvertToRadians(0.f), XMConvertToRadians(0.f));

		sceneObject->addComponent(goalTrigger, "trigger",
			new TriggerComponent());

		TriggerComponent* tc = static_cast<TriggerComponent*>(goalTrigger->getComponent("trigger"));
		tc->initTrigger( sceneObject->m_sceneID, goalTrigger, XMFLOAT3(9.0f, 8.0f, 0.5f));
		tc->setEventData(TriggerType::EVENT, (int)EventType::SWAPSCENE);
		tc->setIntData((int)ScenesEnum::ARENA);
	}
	/////////////////////////////////////////////////////////////////////////////////////

	Entity* skybox = sceneObject->addEntity("SkyBox");
	skybox->m_canCull = false;
	if (skybox)
	{
		Material skyboxMat;
		skyboxMat.addTexture(L"Skybox_Texture.dds", true);
		sceneObject->addComponent(skybox, "cube", new MeshComponent("skyboxCube.lrm", ShaderProgramsEnum::SKYBOX, skyboxMat));
		//Disable shadow casting
		dynamic_cast<MeshComponent*>(skybox->getComponent("cube"))->setCastsShadow(false);
	}

	/////////////////////////////////////////////////////////////////////////////////////

	// MUSIC
	/*Entity* music = sceneObject->addEntity("music");
	sceneObject->addComponent(music, "music", new AudioComponent(L"BestSongPLS.wav", true, 0.1f));*/

	// Lights
	// - Point Light
	sceneObject->createSpotLight(Vector3(16.54f, 21.f, 92.7f), Vector3(-90.f, 0.f, 0.f), Vector3(0.f, 1.f, 0.f), 3.f);
	sceneObject->createSpotLight(Vector3(-30.f, 35.f, 159.7f), Vector3(-90.f, 0.f, 0.f), Vector3(0.f, 1.f, 0.f), 3.f);
	sceneObject->createSpotLight(Vector3(-32.f, 39.f, 176.f), Vector3(0.f, 45.f, 0.f), Vector3(0.f, 1.f, 0.f), 0.1f);

	sceneObject->createSpotLight(Vector3(-5.f, 22.f, 68.5f), Vector3(0.f, -45.f, 0.f), Vector3(0.f, 0.f, 1.f), 0.1f);
	sceneObject->createSpotLight(Vector3(8.5f, 60.f, 159.5f), Vector3(90.f, 0.f, 0.f), Vector3(0.f, 0.f, 1.f), 0.2f);

	sceneObject->createSpotLight(Vector3(-11.f, 50.f, 275.f), Vector3(-35.f, 0.f, 0.f), Vector3(1.f, 0.f, 0.f), 0.3f);

	*finished = true;
}

void Scene::loadEndScene(Scene* sceneObject, bool* finished)
{
	Entity* floor = sceneObject->addEntity("Floor");
	if (floor)
	{
		sceneObject->addComponent(floor, "mesh", new MeshComponent("testCube_pCube1.lrm",
			Material({ L"DarkGrayTexture.png" })));
		floor->scale({ 30, 1, 30 });
		floor->translate({ 0,-2,0 });
		sceneObject->createNewPhysicsComponent(floor, false, "", PxGeometryType::eBOX, "earth", false);
	}

	Entity* pedestalPlaceFour = sceneObject->addEntity("pedastal1");
	if (pedestalPlaceFour)
	{
		sceneObject->addComponent(pedestalPlaceFour, "mesh", new MeshComponent("testCube_pCube1.lrm",
			Material({ L"BlackTexture.png" })));
		pedestalPlaceFour->scale({ 2.5, 1, 2.5 });
		pedestalPlaceFour->translate({ 10, -1,0 });
		sceneObject->createNewPhysicsComponent(pedestalPlaceFour, false, "", PxGeometryType::eBOX, "earth", false);
	}

	Entity* pedestalPlaceThree = sceneObject->addEntity("pedastal2");
	if (pedestalPlaceThree)
	{
		sceneObject->addComponent(pedestalPlaceThree, "mesh", new MeshComponent("testCube_pCube1.lrm",
			Material({ L"BlackTexture.png" })));
		pedestalPlaceThree->scale({ 2.5, 2, 2.5 });
		pedestalPlaceThree->translate({ 2.5,-0.5,0 });
		sceneObject->createNewPhysicsComponent(pedestalPlaceThree, false, "", PxGeometryType::eBOX, "earth", false);
	}

	Entity* pedestalPlaceTwo = sceneObject->addEntity("pedastal3");
	if (pedestalPlaceTwo)
	{
		sceneObject->addComponent(pedestalPlaceTwo, "mesh", new MeshComponent("testCube_pCube1.lrm",
			Material({ L"BlackTexture.png" })));
		pedestalPlaceTwo->scale({ 2.5, 3, 2.5 });
		pedestalPlaceTwo->translate({ 7.5,0,0 });
		sceneObject->createNewPhysicsComponent(pedestalPlaceTwo, false, "", PxGeometryType::eBOX, "earth", false);
	}

	Entity* pedestalPlaceOne = sceneObject->addEntity("pedastal4");
	if (pedestalPlaceOne)
	{
		sceneObject->addComponent(pedestalPlaceOne, "mesh", new MeshComponent("testCube_pCube1.lrm",
			Material({ L"BlackTexture.png" })));
		pedestalPlaceOne->scale({ 2.5, 4, 2.5 });
		pedestalPlaceOne->translate({ 5,0,0 });
		sceneObject->createNewPhysicsComponent(pedestalPlaceOne, false, "", PxGeometryType::eBOX, "earth", false);
	}

	Entity* PlayerOne = sceneObject->addEntity("Playerdummy1");
	if (PlayerOne)
	{
		AnimatedMeshComponent* animMeshComp = new AnimatedMeshComponent("platformerGuy.lrsm", ShaderProgramsEnum::SKEL_ANIM);
		animMeshComp->addAndPlayBlendState({ {"platformer_guy_idle", 0}, {"Running4.1", 1} }, "runOrIdle", 0.f, true, true);
		PlayerOne->addComponent("mesh", animMeshComp);
		sceneObject->addMeshComponent(animMeshComp);
		PlayerOne->scale({ 2, 2, 2. });

	}
	Entity* PlayerTwo = sceneObject->addEntity("Playerdummy2");
	if (PlayerTwo)
	{
		AnimatedMeshComponent* animMeshComp = new AnimatedMeshComponent("platformerGuy.lrsm", ShaderProgramsEnum::SKEL_ANIM);
		animMeshComp->addAndPlayBlendState({ {"platformer_guy_idle", 0}, {"Running4.1", 1} }, "runOrIdle", 0.f, true,true);
		PlayerTwo->addComponent("mesh", animMeshComp);
		sceneObject->addMeshComponent(animMeshComp);
		PlayerTwo->scale({ 2, 2, 2 });
	}
	Entity* PlayerThree = sceneObject->addEntity("Playerdummy3");
	if (PlayerThree)
	{
		AnimatedMeshComponent* animMeshComp = new AnimatedMeshComponent("platformerGuy.lrsm", ShaderProgramsEnum::SKEL_ANIM);
		animMeshComp->addAndPlayBlendState({ {"platformer_guy_idle", 0}, {"Running4.1", 1} }, "runOrIdle", 0.f, true, true);
		PlayerThree->addComponent("mesh", animMeshComp);
		sceneObject->addMeshComponent(animMeshComp);
		PlayerThree->scale({ 2, 2, 2 });
	}

	sceneObject->setPlayersPosition(PlayerOne);
	sceneObject->setPlayersPosition(PlayerTwo);
	sceneObject->setPlayersPosition(PlayerThree);
	
	*finished = true;
}

void Scene::loadArena(Scene* sceneObject, bool* finished)
{
	Engine* engine = &Engine::get();
	Entity* entity;

	sceneObject->m_sceneEntryPosition = Vector3(0, 0, 0);


	Entity* bossEnt = sceneObject -> addEntity("boss");
	if (bossEnt)
	{
		AnimatedMeshComponent* animMeshComp = new AnimatedMeshComponent("platformerGuy.lrsm", ShaderProgramsEnum::SKEL_ANIM);
		animMeshComp->addAndPlayBlendState({ {"platformer_guy_idle", 0.f}, {"Running4.1", 1.f} }, "runOrIdle", 0.f, true, true);
		bossEnt->addComponent("mesh", animMeshComp);
		sceneObject->addMeshComponent(animMeshComp);
		bossEnt->scale({ 4, 4, 4 });
		bossEnt->translate({ 10,8,0 });

		sceneObject->m_boss = new Boss();
		sceneObject->m_boss->Attach(sceneObject);
		sceneObject->m_boss->initialize(bossEnt, false);
		sceneObject->m_boss->addAction(new MoveToAction(bossEnt, sceneObject->m_boss, Vector3(-30, 9, 0), 13.f));
		sceneObject->m_boss->addAction(new ShootProjectileAction(bossEnt, sceneObject->m_boss, 3, 0));
		sceneObject->m_boss->addAction(new MoveToAction(bossEnt, sceneObject->m_boss, Vector3(-30, 9, -30), 13.f));
		sceneObject->m_boss->addAction(new ShootProjectileAction(bossEnt, sceneObject->m_boss, 3, 0));
		sceneObject->m_boss->addAction(new MoveToAction(bossEnt, sceneObject->m_boss, Vector3(30, 9, -30), 13.f));
		sceneObject->m_boss->addAction(new ShootProjectileAction(bossEnt, sceneObject->m_boss, 3, 0));
		sceneObject->m_boss->addAction(new MoveToAction(bossEnt, sceneObject->m_boss, Vector3(0, 9, 0), 13.f));
		sceneObject->m_boss->addAction(new ShootProjectileAction(bossEnt, sceneObject->m_boss, 3, 0));

		Physics::get().Attach(sceneObject->m_boss, true, false);

	}

	Material gridTest = Material({ L"BlackGridBlueLines.png" });
	entity = sceneObject->addEntity("floor");
	if (entity)
	{
		sceneObject->addComponent(entity, "mesh", new MeshComponent("testCube_pCube1.lrm", gridTest));
		entity->scale({ 157, 2, 157 });
		entity->setPosition({ 0,-2,0 });
		sceneObject->createNewPhysicsComponent(entity, false, "", PxGeometryType::eBOX, "earth", false);
	}
	sceneObject->createStaticPlatform(Vector3(0, 24, 78), Vector3(0, 0, 0), Vector3(157, 50, 1), "testCube_pCube1.lrm", L"DarkGrayTexture.png");
	sceneObject->createStaticPlatform(Vector3(0, 24, -78), Vector3(0, 0, 0), Vector3(157, 50, 1), "testCube_pCube1.lrm", L"DarkGrayTexture.png");
	sceneObject->createStaticPlatform(Vector3(78, 24, 0), Vector3(0, 0, 0), Vector3(1, 50, 157), "testCube_pCube1.lrm", L"DarkGrayTexture.png");
	sceneObject->createStaticPlatform(Vector3(-78, 24, 0), Vector3(0, 0, 0), Vector3(1, 50, 157), "testCube_pCube1.lrm", L"DarkGrayTexture.png");

	entity = sceneObject->addEntity("bossSign");
	if (entity)
	{
		sceneObject->addComponent(entity, "mesh", new MeshComponent("BossSign_pCube20.lrm", Material({ L"BossSign.png" })));
		entity->setPosition({ 0.f, -8.3f, 8.f });
	}


	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Entity* clownMask = sceneObject->addEntity("ClownMask");
	if (clownMask)
	{
		sceneObject->addComponent(clownMask, "mesh",
			new MeshComponent("ClownMask_ClownEye_R1.lrm", Material({ L"DarkGrayTexture.png" })));

		clownMask->setPosition(Vector3(0, 10, 73));
		clownMask->setRotation(XMConvertToRadians(7), XMConvertToRadians(180), XMConvertToRadians(0));
	}
	Entity* goalTrigger = sceneObject->addEntity("trigger");
	if (goalTrigger)
	{
		sceneObject->addComponent(goalTrigger, "mesh",
			new MeshComponent("testCube_pCube1.lrm", Material({ L"BlackTexture.png" })));
		goalTrigger->setPosition(0.f, 10.563f, 75.347f);
		goalTrigger->setScale(13.176f, 15.048f, 1.f);
		goalTrigger->setRotation(XMConvertToRadians(-10.102f), XMConvertToRadians(0), XMConvertToRadians(0));

		sceneObject->addComponent(goalTrigger, "trigger",
			new TriggerComponent());

		TriggerComponent* tc = static_cast<TriggerComponent*>(goalTrigger->getComponent("trigger"));
		tc->initTrigger( sceneObject->m_sceneID, goalTrigger, XMFLOAT3(9.0f, 8.0f, 0.5f));
		tc->setEventData(TriggerType::EVENT, (int)EventType::SWAPSCENE);
		tc->setIntData((int)ScenesEnum::LOBBY);
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Entity* skybox = sceneObject->addEntity("SkyBox");
	skybox->m_canCull = false;
	if (skybox)
	{
		Material skyboxMat;
		skyboxMat.addTexture(L"Skybox_Texture.dds", true);
		sceneObject->addComponent(skybox, "cube", new MeshComponent("skyboxCube.lrm", ShaderProgramsEnum::SKYBOX, skyboxMat));
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Light
	sceneObject->createPointLight({ 0.f, 10.f, 0.f }, { 1.f, 0.f, 0.f }, 5.f);

	// Audio test
	/*Entity* audioTestDelete = addEntity("deleteTestAudio");
	addComponent(audioTestDelete, "deleteSound", new AudioComponent(L"PickupTunnels.wav", true, 0.5f));
	delete m_entities["deleteTestAudio"];
	m_entities.erase("deleteTestAudio");*/

	// Night Ambient Sound
	Entity* audioTest = sceneObject->addEntity("audioTest");
	sceneObject->addComponent(audioTest, "testSound", new AudioComponent(L"NightAmbienceSimple_02.wav", true, 0.2f));
	sceneObject->m_nightSlide = 0.01f;
	sceneObject->m_nightVolume = 0.2f;

	*finished = true; //Inform the main thread that the loading is complete.
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

	int test = 360 / nrOfPlatforms;
	assert(test > 0);

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
				new RotateAroundComponent(center, ParisWheelPlatform, 12.f, rotationSpeed, (float)i));
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

void Scene::loadMaterialTest(Scene* sceneObject, bool* finished)
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
		sceneObject->m_entities[currentSphereName] = sceneObject->addEntity(currentSphereName);
		if (sceneObject->m_entities[currentSphereName])
		{
			entity = sceneObject->m_entities[currentSphereName];
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

			sceneObject->addComponent(entity, "mesh", new MeshComponent("Sphere_2m_Sphere.lrm", ShaderProgramsEnum::PBRTEST, { PBRMatTextured }));

			float moveDistance = -5.f;
			entity->translate({ moveDistance * i + 30.f, 2.f, 20.f });
			entity->rotate({ 1.5708f, 0.f, 0.f });
		}
	}

	int xCounter = 0;
	int yCounter = 0;

	for (size_t i = 0; i < 25; i++)
	{
		std::string currentSphereName = ("PBRSphere" + std::to_string(i));
		sceneObject->m_entities[currentSphereName] = sceneObject->addEntity(currentSphereName);
		if (sceneObject->m_entities[currentSphereName])
		{
			entity = sceneObject->m_entities[currentSphereName];
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
			PBRMatUntextured.setRoughness(xCounter * 0.18f + 0.1f);
			PBRMatUntextured.setTextured(0);

			sceneObject->addComponent(entity, "mesh", new MeshComponent("Sphere_2m_Sphere.lrm", ShaderProgramsEnum::PBRTEST, PBRMatUntextured));

			float moveDistance = 5.f;
			entity->translate({ moveDistance * xCounter, moveDistance * yCounter - 3.f, 0.f });
			entity->rotate({ 1.5708f, 0.f, 0.f });
		}
	}

	Entity* skybox = sceneObject->addEntity("SkyBox");
	skybox->m_canCull = false;
	if (skybox)
	{
		Material skyboxMat;
		skyboxMat.addTexture(L"skybox1.dds", true);
		sceneObject->addComponent(skybox, "cube", new MeshComponent("skyboxCube.lrm", ShaderProgramsEnum::SKYBOX, skyboxMat));
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
		sceneObject->m_entities[currentPointLightName] = sceneObject->addEntity(currentPointLightName);
		if (sceneObject->m_entities[currentPointLightName])
		{
			entity = sceneObject->m_entities[currentPointLightName];
			std::string currentPointLightComponentName = ("PointLightTestPointLight" + std::to_string(i));
			sceneObject->addComponent(sceneObject->m_entities[currentPointLightName], currentPointLightComponentName, new LightComponent());
			dynamic_cast<LightComponent*>(sceneObject->m_entities[currentPointLightName]->getComponent(currentPointLightComponentName))->setColor(XMFLOAT3(1, 1, 1));
			dynamic_cast<LightComponent*>(sceneObject->m_entities[currentPointLightName]->getComponent(currentPointLightComponentName))->setIntensity(pointLightIntensities[i]);
			//engine->addComponent(entity, "mesh", new MeshComponent("testCube_pCube1.lrm", Material({ L"T_CircusTent_D.png" })));
			entity->translate(pointLightPositions[i]);
		}
	}

	*finished = true;
}

void Scene::loadBossTest(Scene* sceneObject, bool* finished)
{
	sceneObject->imageStyle.position = Vector2(400.f, 50.f);
	sceneObject->imageStyle.scale = Vector2(1.f, 0.8f);
	sceneObject->m_bossHP_barGuiIndex = GUIHandler::get().addGUIImage(L"BossHP-bar.png", sceneObject->imageStyle);

	sceneObject->imageStyle.position = Vector2(400.f, 50.f);
	sceneObject->imageStyle.scale = Vector2(1.f, 0.8f);
	sceneObject->m_bossHP_barBackgroundGuiIndex = GUIHandler::get().addGUIImage(L"BossHP-barBG.png", sceneObject->imageStyle);


	sceneObject->createStaticPlatform(Vector3(0, 15, 20), Vector3(0, 0, 0), Vector3(10, 1, 20), "testCube_pCube1.lrm");
	sceneObject->m_sceneEntryPosition = Vector3(0, 17, 20);

	//Generate the boss, if boss does not exist in the scene it will not be updated in sceneUpdate() either.
	Entity* bossEnt = sceneObject->addEntity("boss");
	sceneObject->m_bossMusicComp = new AudioComponent(L"BossMusic.wav", true, 0.1f, 0.f, false);

	if (bossEnt)
	{
		bossEnt->scale({ 1, 1, 1 });
		bossEnt->translate({ 0,2.5,0 });

		sceneObject->addComponent(bossEnt, "sound", sceneObject->m_bossMusicComp);

		sceneObject->m_boss = new Boss();
		sceneObject->m_boss->Attach(sceneObject);
		sceneObject->m_boss->initialize(bossEnt, true);
		sceneObject->m_boss->setNrOfMaxStars(10);
		sceneObject->m_endBossAtPecentNrOfStarts = 50; // if this is 0 the end secene will be triggered

		//// Init grid structure
		BossStructures::PlatformArray* platformArray = &sceneObject->m_boss->platformArray;
		float spacingBetweenPlatforms = 1.f;
		int total = 0;
		float scaling = 5.f;
		Vector3 platformPos = Vector3(15, 0, 15);
		for (int x = 0; x < platformArray->columns.size(); x++)
		{
			for (int y = 0; y < platformArray->columns.size(); y++)
			{
				Entity* platform = sceneObject->addEntity("bossPlatform-" + std::to_string(total++));
				Vector3 offset = sceneObject->m_sceneEntryPosition;
				if (platform)
				{
					platform->setPosition(Vector3(((float)x) * scaling + (x)*0.05f, 2, (float)(y) * scaling + (y)*0.05f) + platformPos);
					platform->scale(scaling, 9.f, scaling);
					platform->addComponent("grow", new GrowingComponent(platform, platform->getScaling(), 8.f));
					static_cast<GrowingComponent*>(platform->getComponent("grow"))->setDone(true);
					platform->addComponent("shrink", new ShrinkingComponent(platform, Vector3(0.01, 0.01, 0.01), 8.f));
					static_cast<ShrinkingComponent*>(platform->getComponent("shrink"))->setDone(true);

					Material emissiveMat({ L"DarkGrayTexture.png", L"BlueEmissive.png" });
					emissiveMat.setEmissiveStrength(40);
					sceneObject->addComponent(platform, "mesh", new MeshComponent("BossPlatform.lrm", 
						{
							EMISSIVE,
							DEFAULT,
							DEFAULT
						},
						{
							emissiveMat,
							Material({ L"DarkGrayTexture.png" }),
							Material({ L"DarkGrayTexture.png" })
						}));

					sceneObject->createNewPhysicsComponent(platform);

					(*platformArray)[x][y] = platform;

				}

			}
		}
		//// Grid structure finished


		//Added the platform to spawn on, as well as a checkpoint on it.
		sceneObject->addCheckpoint(sceneObject->m_sceneEntryPosition + Vector3(0, 0, 0));
		Physics::get().Attach(sceneObject->m_boss, true, false);


		//Generate a set of segments on the boss, primarily to display the interface.
		for (int i = 0; i < 2; i++)
		{
			Entity* segmentEntity = sceneObject->addEntity("projectileSegment" + std::to_string(i));
			sceneObject->addComponent(segmentEntity, "mesh", new MeshComponent("Boss_Bot.lrm", Material({ L"DarkGrayTexture.png" })));
			segmentEntity->setScale({ 1,1,1 });

			BossSegment* projectileSegment = new BossSegment();
			projectileSegment->initializeSegment(segmentEntity, false);
			projectileSegment->m_entityOffset = Vector3(0, 3 * sceneObject->m_boss->m_bossSegments.size(), 0);
			sceneObject->m_boss->addSegment(projectileSegment);
			sceneObject->createNewPhysicsComponent(segmentEntity, true, "mesh");
			segmentEntity->getComponentsByType<PhysicsComponent>(ComponentType::PHYSICS)->makeKinematic();
			projectileSegment->Attach(sceneObject);

			//This is to add actions to the segment just generated, they will however shoot independently of the boss moving aroud.
			//ShootLaserAction* action = new ShootLaserAction(segmentEntity, projectileSegment, 5);
			//projectileSegment->addAction(action);
		}

		//Add the head of the boss, just another mesh in the boss entity.
		MeshComponent* headComponent = new MeshComponent("Boss_Top.lrm", Material({ L"DarkGrayTexture.png" }));
		headComponent->setPosition(0, (2 * sceneObject->m_boss->m_bossSegments.size()), 0);
		sceneObject->addComponent(bossEnt, "meshHead", headComponent);

		//Add custom action
		//sceneObject->m_boss->addAction(new ShootLaserAction(bossEnt, sceneObject->m_boss, 4.f));
	}

	Entity* skybox = sceneObject->addEntity("SkyBox");
	skybox->m_canCull = false;
	if (skybox)
	{
		Material skyboxMat;
		skyboxMat.addTexture(L"Skybox_Texture.dds", true);
		sceneObject->addComponent(skybox, "cube", new MeshComponent("skyboxCube.lrm", ShaderProgramsEnum::SKYBOX, skyboxMat));
		//Disable shadow casting
		dynamic_cast<MeshComponent*>(skybox->getComponent("cube"))->setCastsShadow(false);
	}

	*finished = true;
}

void Scene::loadEmpty(Scene* sceneObject, bool* finished)
{
	/*Entity* skybox = sceneObject->addEntity("SkyBox");
	skybox->m_canCull = false;
	if (skybox)
	{
		Material skyboxMat;
		skyboxMat.addTexture(L"Skybox_Texture.dds", true);
		sceneObject->addComponent(skybox, "cube", new MeshComponent("skyboxCube.lrm", ShaderProgramsEnum::SKYBOX, skyboxMat));
	}*/


	*finished = true;
}

void Scene::loadAlmostEmpty(Scene* sceneObject, bool* finished)
{
	sceneObject->m_sceneEntryPosition = Vector3(0, 10, 0);
	sceneObject->createStaticPlatform(Vector3(0, -2, 0), Vector3(0, 0, 0), Vector3(4, 1, 20), "testCube_pCube1.lrm");

	sceneObject->addCheckpoint({ 0, 8, 0 });

	*finished = true;
}

void Scene::onSceneLoaded()
{
	for (auto& entity : m_entities)
		entity.second->onSceneLoad();
}

void Scene::updateScene(const float& dt)
{
	if (m_boss)
	{
		m_boss->update(dt);
		Vector3 targetPos = static_cast<CharacterControllerComponent*>(m_player->getPlayerEntity()->getComponent("CCC"))->getFootPosition() + Vector3(0, 1, 0);

		//This is the primary action loop of the boss.
		//If boss it not moving, pick a new target at random, wait for two seconds, then shoot. Start over.
		if (m_boss->getActionQueue()->size() == 0 && m_boss->getCurrnentNrOfStars() >= 0)
		{
			BossStructures::IntVec platformTargetIndex = m_boss->getNewPlatformTarget();
			m_boss->addAction(new MoveToTargetInGridAction(m_boss->m_bossEntity, m_boss, &m_boss->platformArray, Vector2(platformTargetIndex.x, platformTargetIndex.y), 10.f, &m_boss->currentPlatformIndex, m_boss->getActionQueue()));
			//m_boss->addAction(new WaitAction(m_boss->m_bossEntity, m_boss, 2)); //Wait before moving again
			m_boss->addAction(new ShootLaserAction(m_boss->m_bossSegments.at(0)->m_bossEntity, m_boss, 1));
			m_boss->addAction(new MoveToTargetInGridAction(m_boss->m_bossEntity, m_boss, &m_boss->platformArray, Vector2(platformTargetIndex.x, platformTargetIndex.y), 10.f, &m_boss->currentPlatformIndex, m_boss->getActionQueue()));
			//m_boss->addAction(new WaitAction(m_boss->m_bossEntity, m_boss, 5)); //Wait before moving again
			m_boss->addAction(new ThunderAction(m_boss->m_bossEntity, m_boss, &m_boss->platformArray, &m_displacedPlatforms));
		}
		
		ShootProjectileAction* ptr = dynamic_cast<ShootProjectileAction*>(m_boss->getCurrentAction());
		if (ptr)
			ptr->setTarget(static_cast<CharacterControllerComponent*>(m_player->getPlayerEntity()->getComponent("CCC"))->getFootPosition() + Vector3(0, 1, 0));

		//Check lasers
		checkLasers(dt);

		//Check projectiles and their lifetime so they do not continue on forever in case they missed.
		checkProjectiles();

		//Check if the boss platforms are correctly placed
		checkPlatforms(dt);

		for (int i = 0; i < deferredPointInstantiationList.size(); i++)
		{
			addScore(deferredPointInstantiationList[i]);
		}

		

		deferredPointInstantiationList.clear();
	}
	

	if (addedBarrel)
	{
		static_cast<PhysicsComponent*>(m_entities["barrel"]->getComponent("physics"))->clearForce();
		static_cast<PhysicsComponent*>(m_entities["barrel"]->getComponent("physics"))->setPosition({ -30, 50, 130 });
		m_despawnBarrelTimer.restart();

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

	if (tempComponentVector.empty())
		entity->getComponentsOfType(tempComponentVector, ComponentType::ANIM_MESH);

	assert(!tempComponentVector.empty());

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
	physComp->initActorAndShape(m_sceneID, entity, meshComponent, geometryType, dynamic, materialName, isUnique);
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

void Scene::setPlayersPosition(Entity* entity)
{
	int position = -1;
	for (int i = 0; i < m_scores.size(); i++)
	{

		if (m_scores.at(i).second == entity->getIdentifier())
		{
			position = i;
		}

	}
	if (position==0)
	{
		entity->translate(2.5, 0.5, 0);
	}
	if (position == 1)
	{
		entity->translate(7, 1.5, 0);
	}
	if (position == 2)
	{
		entity->translate(5, 2, 0);
	}
}

void Scene::removeLightComponent(LightComponent* component)
{
	getEntity(component->getParentEntityIdentifier())->removeComponent(component);

	int nrOfErased = (int)m_lightComponentMap.erase(component->getIdentifier());
	if (nrOfErased > 0) //if it deleted more than 0 elements
	{
		m_lightCount -= nrOfErased;
	}
}

void Scene::removeLightComponentFromMap(LightComponent* component)
{


	int nrOfErased = (int)m_lightComponentMap.erase(component->getIdentifier());
	if (nrOfErased > 0) //if it deleted more than 0 elements
	{
		m_lightCount -= nrOfErased;
	}
}

std::vector<std::pair<int, std::string>>* Scene::getScores()
{
	return &m_scores;
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

	if (type == BossMovementType::ShootLaser)
	{
		createLaser(data);
	}

	if (type == BossMovementType::ClearColumn)
	{
		for (int i = 0; i < m_boss->platformArray.columns.size(); i++)
		{
			displacePlatform(m_boss->platformArray[m_boss->currentPlatformIndex.x][i]);
		}
	}

	if (type == BossMovementType::ClearRow)
	{
		for (int i = 0; i < m_boss->platformArray.columns.size(); i++)
		{
			displacePlatform(m_boss->platformArray[i][m_boss->currentPlatformIndex.y]);
		}
	}

	if (type == BossMovementType::MovePlatform) //Move a platform that the boss has moved over for a period of time.
	{
		Entity* entity;
		entity = static_cast<Entity*>(data.pointer0);

		displacePlatform(entity);
	}

	if (type == BossMovementType::DropPoints)
	{
		Entity* projectile = static_cast<Entity*>(data.pointer0);
		Component* component = projectile->getComponent("projectile");

		m_boss->dropStar(1);
		float c = float(m_boss->getCurrnentNrOfStars()) / float(m_boss->getNrOfMaxStars());

		if (m_boss->getCurrnentNrOfStars() > -1)
		{
			imageStyle.scale = Vector2(c, 0.8f);
			GUIHandler::get().setImageStyle(m_bossHP_barGuiIndex, imageStyle);

			if (component != nullptr)
			{
				ProjectileComponent* projComponent = dynamic_cast<ProjectileComponent*>(projectile->getComponent("projectile"));
				m_projectiles.erase(projComponent->m_id);
				removeEntity(projectile->getIdentifier());
				deferredPointInstantiationList.push_back(data.origin + Vector3(0, 1, 0));
			}
			else
				addScore(data.origin + Vector3(0, 5, 0));
		}



			if (c <= m_endBossAtPecentNrOfStarts * 0.01)
			{
				m_bossMusicComp->setVolume(0.02f);
				removeBoss();
				if (m_endBossAtPecentNrOfStarts != 0)
					createPortal();
				else
					createEndScenePortal();
			}

	}

	if (type == BossMovementType::SpawnParticlesOnPlatform)
	{

		Entity* platformEntity = static_cast<Entity*>(data.pointer0);
		MeshComponent* platformMeshComponent = (MeshComponent*)(platformEntity->getComponent("mesh"));
		Material* platformMaterial = platformMeshComponent->getMaterialPtr(0);

		platformMaterial->swapTexture(L"RedEmissive.png", 1);
		
		{
			Entity* bossEnt = static_cast<Entity*>(data.pointer1);
			AudioComponent* sound = nullptr;
			sound = new AudioComponent(L"warningSound.wav", false, 1.f, 0.f, true, bossEnt);
			addComponent(bossEnt, "3Dsound", sound);
			sound->playSound();
		}
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

void Scene::createTimedSweepPlatform(Vector3 startPos, Vector3 endPos, bool startEnd, float interval)
{
	Entity* crashBPlatform = addEntity("crashBPlatform-" + std::to_string(m_nrOfSweepingPlatforms++));
	if (crashBPlatform)
	{
		BarrelTriggerComponent* barrelComponentTrigger = new BarrelTriggerComponent();
		addComponent(crashBPlatform, "mesh",
			new MeshComponent("testCube_pCube1.lrm", Material({ L"Wellcome.png" })));

		crashBPlatform->setPosition(startPos);

		createNewPhysicsComponent(crashBPlatform, true);

		crashBPlatform->addComponent("sweepBoy", new SweepingComponent(crashBPlatform, startPos, endPos, 1.f, true, true, interval, startEnd));
	}

}

void Scene::createSpotLight(Vector3 position, Vector3 rotation, Vector3 color, float intensity)
{
	m_nrOfSpotLight++;

	Entity* sLight = addEntity("spotLight-" + std::to_string(m_nrOfSpotLight));
	if (sLight)
	{
		addComponent(sLight, "spot-" + std::to_string(m_nrOfSpotLight), new SpotLightComponent(Vector3(), color, intensity));
		sLight->setPosition(position);
		sLight->setRotation(XMConvertToRadians(rotation.x), XMConvertToRadians(rotation.y), XMConvertToRadians(rotation.z));
		//dynamic_cast<LightComponent*>(sLight->getComponent("spot-" + std::to_string(m_nrOfSpotLight)))->setColor(XMFLOAT3(color));
		//dynamic_cast<LightComponent*>(sLight->getComponent("spot-" + std::to_string(m_nrOfSpotLight)))->setIntensity(intensity);
	}
}

void Scene::createPointLight(Vector3 position, Vector3 color, float intensity)
{
	m_nrOfPointLight++;

	Entity* pLight = addEntity("pointLight-" + std::to_string(m_nrOfPointLight));
	if (pLight)
	{
		LightComponent* pointLight = new LightComponent(Vector3(), color, intensity);
		//pointLight->setColor(XMFLOAT3(color));
		//pointLight->setIntensity(intensity);
		addComponent(pLight, "point-" + std::to_string(m_nrOfPointLight), pointLight);
		pLight->setPosition(position);
	}
}

void Scene::createSwingingHammer(Vector3 position, Vector3 rotation, float swingSpeed)
{
	m_nrOfSweepingPlatforms++;

	Entity* hammerFrame = addEntity("HammerFrame-" + std::to_string(m_nrOfSweepingPlatforms));
	if (hammerFrame)
	{
		addComponent(hammerFrame, "mesh",
			new MeshComponent("Hammer_Frame_pCube7.lrm", Material({ L"DarkGrayTexture.png" })));

		hammerFrame->setPosition(position);
		hammerFrame->setRotation(XMConvertToRadians(rotation.x), XMConvertToRadians(rotation.y), XMConvertToRadians(rotation.z));

		createNewPhysicsComponent(hammerFrame, false, "", PxGeometryType::eTRIANGLEMESH);

		
	}

	Entity* hammer = addEntity("Hammer-" + std::to_string(m_nrOfSweepingPlatforms));
	if (hammer)
	{
		addComponent(hammer, "mesh",
			new MeshComponent("Hammer_pCylinder3.lrm", Material({ L"DarkGrayTexture.png" })));


		hammer->setPosition({ position.x, position.y + 3.5f, position.z });
		hammer->setRotation(XMConvertToRadians(rotation.x), XMConvertToRadians(rotation.y), XMConvertToRadians(rotation.z));

		createNewPhysicsComponent(hammer, true, "", PxGeometryType::eTRIANGLEMESH);
		static_cast<PhysicsComponent*>(hammer->getComponent("physics"))->makeKinematic();

		addComponent(hammer, "swing",
			new SwingComponent(hammer, rotation, swingSpeed));
	}
}

void Scene::createSkybox(std::wstring textureName)
{
	Entity* skybox = addEntity("SkyBox");
	skybox->m_canCull = false;
	if (skybox)
	{
		Material skyboxMat;
		skyboxMat.addTexture(textureName.c_str(), true);
		addComponent(skybox, "cube", new MeshComponent("skyboxCube.lrm", ShaderProgramsEnum::SKYBOX, skyboxMat));
		//Disable shadow casting
		dynamic_cast<MeshComponent*>(skybox->getComponent("cube"))->setCastsShadow(false);
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

		static_cast<TriggerComponent*>(projectileEntity->getComponent("projectile"))->initTrigger( m_sceneID, projectileEntity, { 0.5f, 0.5f, 0.5f });


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

void Scene::createLaser(BossStructures::BossActionData data)
{
	Entity* laserEntity = addEntity("laser" + std::to_string(m_nrOfLasers++));
	AudioComponent* sound = nullptr;
	if (m_nrOfLasers % 4 == 0)
	{
		sound = new AudioComponent(L"laserSound.wav", false, 1.f, 0.f, true, laserEntity);
	}

	if (laserEntity)
	{
		laserEntity->setRotation(data.rotation);
		laserEntity->setPosition(data.origin);
		laserEntity->m_canCull = false;
		Material mat = Material({ L"red.png", L"red.png" });
		mat.setEmissiveStrength(700.f);

		MeshComponent* mComp = new MeshComponent("Boss_Laser.lrm", EMISSIVE, mat);
		mComp->setCastsShadow(false);
		addComponent(laserEntity, "mesh", mComp);
		laserEntity->setScale(1, 1, 10);

		if (sound != nullptr)
		{
			addComponent(laserEntity, "3Dsound", sound);
		}

		BossStructures::BossLaser* laserObject = new BossStructures::BossLaser();
		laserObject->entity = laserEntity;
		laserObject->id = m_nrOfLasers;
		laserObject->lifeTime = 0.5f;
		laserObject->timer.restart();

		m_lasers[laserObject->id] = laserObject;
	}

	if (m_nrOfLasers % 4 == 0)
	{
		sound->playSound();
	}
}

void Scene::checkLasers(float dt)
{
	std::vector<UINT> idsToRemove;

	for (auto laserStruct : m_lasers)
	{
		float currTime = laserStruct.second->timer.timeElapsed();
		float lifeTime = laserStruct.second->lifeTime;
		if (currTime > lifeTime)
		{
			idsToRemove.push_back(laserStruct.first);
		}

		float maxSize = 2;
		Entity* entity = laserStruct.second->entity;
		float size = (currTime / lifeTime) * maxSize;
		entity->setScale({ 1+size, 1 + size, entity->getScaling().z });
	}

	//can not remove mid-loop
	for (int i = 0; i < idsToRemove.size(); i++)
	{
		removeEntity(m_lasers[idsToRemove[i]]->entity->getIdentifier());
		delete m_lasers[idsToRemove[i]];
		m_lasers.erase(idsToRemove[i]);
	}
}

void Scene::checkPlatforms(float dt)
{
	std::vector<UINT> idsToReset;
	for (auto displacedPlatformStruct : m_displacedPlatforms)
	{
		float currTime = displacedPlatformStruct.second->timer.timeElapsed();
		float lifeTime = displacedPlatformStruct.second->stayDisplacedFor;
		Entity* entity = m_displacedPlatforms.at(displacedPlatformStruct.first)->entity;

		if (static_cast<ShrinkingComponent*>(displacedPlatformStruct.second->entity->getComponent("shrink"))->doneTweening() && !displacedPlatformStruct.second->displaced)
		{
			displacedPlatformStruct.second->displaced = true;
			physicallyMovePlatform(displacedPlatformStruct.second->entity);
		}


		if (currTime > lifeTime && displacedPlatformStruct.second->displaced)
		{
			Entity* platformEntity = displacedPlatformStruct.second->entity;
			MeshComponent* platformMeshComponent = (MeshComponent*)(platformEntity->getComponent("mesh"));
			Material* platformMaterial = platformMeshComponent->getMaterialPtr(0);

			platformMaterial->swapTexture(L"BlueEmissive.png", 1);

			PhysicsComponent* comp = static_cast<PhysicsComponent*>(entity->getComponent("physics"));
			Vector3 currPos = comp->getActorPosition();
			comp->setPosition(currPos - displacedPlatformStruct.second->offsetBy);
			displacedPlatformStruct.second->displaced = false;
			static_cast<GrowingComponent*>(entity->getComponent("grow"))->setDone(false);
			idsToReset.push_back(displacedPlatformStruct.first);

			MeshComponent* meshComp = static_cast<MeshComponent*>(entity->getComponent("mesh"));
			meshComp->setVisible(true);

		}
	}

	for (int i = 0; i < idsToReset.size(); i++)
	{
		m_displacedPlatforms.erase(idsToReset[i]);
	}
}

void Scene::displacePlatform(Entity* entity)
{
	Vector3 offsetAmount = Vector3(0, 300.f, 0);
	PhysicsComponent* comp = static_cast<PhysicsComponent*>(entity->getComponent("physics"));
	Vector3 currPos = comp->getActorPosition();

	if (!findPlatformAlready(entity) && static_cast<GrowingComponent*>(entity->getComponent("grow"))->doneTweening())
	{

		BossStructures::PlatformDisplace* displacedPlatform = new BossStructures::PlatformDisplace();
		displacedPlatform->id = m_nrOfDisplacedPlatforms++;
		displacedPlatform->entity = entity;
		displacedPlatform->offsetBy = offsetAmount;
		displacedPlatform->targetSize = Vector3(0.1f, 0.1f, 0.1f);
		displacedPlatform->originalSize = entity->getScaling();

		m_displacedPlatforms[m_nrOfDisplacedPlatforms] = displacedPlatform;
		static_cast<ShrinkingComponent*>(entity->getComponent("shrink"))->setDone(false);
	}
}

void Scene::physicallyMovePlatform(Entity* entity)
{
	Vector3 offsetAmount = Vector3(0, 300.f, 0);
	PhysicsComponent* comp = static_cast<PhysicsComponent*>(entity->getComponent("physics"));
	Vector3 currPos = comp->getActorPosition();
	comp->setPosition(currPos + offsetAmount);

	MeshComponent* meshComp = static_cast<MeshComponent*>(entity->getComponent("mesh"));
	meshComp->setVisible(false);
}

bool Scene::findPlatformAlready(Entity* entity)
{
	bool found = false;
	for (auto displacedPlatformStruct : m_displacedPlatforms)
		if (displacedPlatformStruct.second->entity == entity)
			found = true;


	return found;
}

void Scene::removeBoss()
{
	for (int i = 0; i < m_boss->getActionQueue()->size(); i++)
		delete m_boss->getActionQueue()->at(i);

	m_boss->getActionQueue()->clear();
	
	UINT id = m_boss->addAction(new MoveToAction(m_boss->m_bossEntity, m_boss, Vector3(0, 10000, 0), 100000));
	GUIHandler::get().setVisible(m_bossHP_barGuiIndex, false);
	GUIHandler::get().setVisible(m_bossHP_barBackgroundGuiIndex, false);
}

void Scene::createPortal()
{
	float a = m_boss->getNrOfPlatforms();
	a = float(ceil(a / 2));
	a = 5 * a;
	a = a + 10;
	Entity* goalTriggerFrame = addEntity("triggerFrame");
	if (goalTriggerFrame)
	{
		goalTriggerFrame->setPosition(Vector3(10, 3, a));
		goalTriggerFrame->setRotation(Vector3(0, XMConvertToRadians(90), 0));
		addComponent(goalTriggerFrame, "mesh",
			new MeshComponent("Portal_pCube41.lrm", Material({ L"DarkGrayTexture.png" })));
	}

	Entity* goalTrigger = addEntity("trigger");
	if (goalTrigger)
	{
		goalTrigger->setPosition(Vector3(10, 4.5, a));
		goalTrigger->setRotation(Vector3(0, XMConvertToRadians(90), 0));

		Material emissiveMat({ L"DarkGrayTexture.png", L"PortalEmissive.jpg" });
		emissiveMat.setEmissiveStrength(30);
		addComponent(goalTrigger, "mesh",
			new MeshComponent("portalMagic_pCylinder8.lrm",
				EMISSIVE, emissiveMat));


		addComponent(goalTrigger, "trigger",
			new TriggerComponent());
		addComponent(goalTrigger, "3Dsound", new AudioComponent(L"PortalSound.wav", true, 1.f, 0.f, true, goalTrigger));

		TriggerComponent* tc = static_cast<TriggerComponent*>(goalTrigger->getComponent("trigger"));
		tc->initTrigger(m_sceneID, goalTrigger, XMFLOAT3(2.5f, 2.5f, 2.5f));
		tc->setEventData(TriggerType::EVENT, (int)EventType::SWAPSCENE);
		tc->setIntData((int)ScenesEnum::ARENA);
	}
}

void Scene::createEndScenePortal()
{
	float a = m_boss->getNrOfPlatforms();
	a = float(ceil(a / 2));
	a = 5 * a;
	a = a + 10;

	Entity* endSceneTriggerFrame = addEntity("endTrriggerFrame");
	if (endSceneTriggerFrame)
	{
		endSceneTriggerFrame->setPosition(Vector3(10, 3, a));
		endSceneTriggerFrame->setRotation(Vector3(0, XMConvertToRadians(90), 0));
		addComponent(endSceneTriggerFrame, "mesh",
			new MeshComponent("Portal_pCube41.lrm", Material({ L"DarkGrayTexture.png" })));
	}

	Entity* endSceneTrigger = addEntity("endTrigger");
	if (endSceneTrigger)
	{
		endSceneTrigger->setPosition(Vector3(10, 4.5, a));
		endSceneTrigger->setRotation(Vector3(0, XMConvertToRadians(90), 0));

		Material emissiveMat({ L"DarkGrayTexture.png", L"PortalEmissive.jpg" });
		emissiveMat.setEmissiveStrength(30);
		addComponent(endSceneTrigger, "mesh", 
			new MeshComponent("portalMagic_pCylinder8.lrm",
			EMISSIVE, emissiveMat));


		addComponent(endSceneTrigger, "endSceneTrigger",
			new TriggerComponent());
		addComponent(endSceneTrigger, "3Dsound", new AudioComponent(L"PortalSound.wav", true, 2.f, 0.f, true, endSceneTrigger));

		TriggerComponent* tc = static_cast<TriggerComponent*>(endSceneTrigger->getComponent("endSceneTrigger"));
		tc->initTrigger(m_sceneID, endSceneTrigger, XMFLOAT3(0.15f, 1.5f, 1.5f));
		tc->setEventData(TriggerType::EVENT, (int)EventType::SWAPSCENE);
		tc->setIntData((int)ScenesEnum::ENDSCENE);
	}
}

void Scene::createRespawnBox(Vector3 position, Vector3 scale, bool boxVisible)
{
	Entity* respawnTrigger = addEntity("respawnTrigger"+std::to_string(m_nrOfRespawnBoxes++));
	if (respawnTrigger)
	{
		Material mat = Material({ L"T_Missing_D.png", L"ibl_brdf_lut.png" });
		mat.setEmissiveStrength(75);
		addComponent(respawnTrigger, "mesh",
			//new MeshComponent("testCube_pCube1.lrm", Material({ L"T_Missing_D.png" })));
			new MeshComponent("testCube_pCube1.lrm",
				EMISSIVE,
				mat));
		MeshComponent* meshComp = respawnTrigger->getComponentsByType<MeshComponent>(ComponentType::MESH);
		meshComp->setVisible(boxVisible);
		meshComp->setCastsShadow(boxVisible);

		respawnTrigger->setPosition(position);
		respawnTrigger->setScale(scale);

		addComponent(respawnTrigger, "respawnTrigger",
			new TriggerComponent());

		TriggerComponent* tc = static_cast<TriggerComponent*>(respawnTrigger->getComponent("respawnTrigger"));
		tc->initTrigger(m_sceneID, respawnTrigger, scale/2);
		tc->setEventData(TriggerType::RESPAWN);
	}
}

void Scene::setScoreVec()
{
	//m_scores.push_back(std::make_pair(m_nrOfScore, "Player"));
	m_scores.push_back(std::make_pair(m_nrOfScorePlayerOne, "Playerdummy1"));
	m_scores.push_back(std::make_pair(m_nrOfScorePlayerTwo, "Playerdummy2"));
	m_scores.push_back(std::make_pair(m_nrOfScorePlayerThree, "Playerdummy3"));
}

void Scene::sortScore()
{
	std::sort(m_scores.begin(), m_scores.end());
}