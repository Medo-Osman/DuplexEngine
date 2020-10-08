#include "3DPCH.h"
#include "Engine.h"
#include"ApplicationLayer.h"

Engine::Engine()
{
	m_settings.width = m_startWidth;
	m_settings.height = m_startHeight;
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
	for (auto& entities : m_entities)
	{
		entities.second->update(dt);
	}

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
	if (component->getType() == ComponentType::MESH)
	{
		MeshComponent* meshComponent = dynamic_cast<MeshComponent*>(component);

		addMeshComponent(meshComponent);
	}


	entity->addComponent(componentIdentifier, component);

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
		addComponent(floor, "mesh", new MeshComponent("testCube_pCube1.lrm", ShaderProgramsEnum::TEMP_TEST));
		floor->scale({ 500.f, 1.f,500.f });
		floor->translation({ 150.f, -0.6f,150.f });

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
	m_camera.setProjectionMatrix(80.f, (float)m_settings.height / (float)m_settings.width, 0.01f, 1000.0f);
	ApplicationLayer::getInstance().m_input.Attach(m_player);
	if (addEntity("meshPlayer"))
	{
		addComponent(m_entities["meshPlayer"], "mesh", new MeshComponent("testTania_tania_geo.lrm", ShaderProgramsEnum::TEMP_TEST));
		m_entities["meshPlayer"]->translation({ 5, 10.f, 0 });
		m_entities["meshPlayer"]->scaleUniform(0.02f);
		this->createNewPhysicsComponent(m_entities["meshPlayer"], true, "", PxGeometryType::eBOX, "human");
		m_player->setPlayerEntity(m_entities["meshPlayer"]);
	}
	else
	{
		ErrorLogger::get().logError("No player model added or already exists when adding");
	}
	m_player->setCameraTranformPtr(m_camera.getTransform());

	// Temp entity init
	addEntity("first");
	addComponent(m_entities["first"], "test", new TestComponent());

	//Build the hardcoded test stage
	buildTestStage();

}
