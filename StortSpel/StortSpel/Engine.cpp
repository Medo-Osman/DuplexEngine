#include "3DPCH.h"
#include "Engine.h"
#include"ApplicationLayer.h"

Engine::Engine()
{
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
}

void Engine::update(const float& dt)
{
	m_player->updatePlayer(dt);

	// AUDIO TEST
	nightVolume += dt * nightSlide;
	if (nightVolume < 0.f)
	{
		nightVolume = 0.f;
		nightSlide = -nightSlide;
	}
	else if (nightVolume > 0.5f)
	{
		nightVolume = 0.5f;
		nightSlide = -nightSlide;
	}
	AudioComponent* ac = dynamic_cast<AudioComponent*>(m_entities["audioTest"]->getComponent("testSound"));
	ac->setVolume(nightVolume);

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

std::map<unsigned int long, MeshComponent*>* Engine::getMeshComponentMap()
{
	return &m_meshComponentMap;
}

void Engine::buildTestStage()
{
	// Cube 1
	if (addEntity("cube-test"))
		addComponent(m_entities["cube-test"], "mesh", new MeshComponent("testCube_pCube1.lrm", ShaderProgramsEnum::TEMP_TEST));

	// Tent
	if (addEntity("tent"))
	{
		addComponent(m_entities["tent"], "mesh", new MeshComponent("BigTopTent_Cylinder.lrm"));
		m_entities["tent"]->rotate({ XMConvertToRadians(-90.f), 0.f, 0.f });
		m_entities["tent"]->move({ -10.f, 0.f, 0.f });
	}

	// Floor
	Entity* floor = addEntity("floor");
	if (floor)
	{
		addComponent(floor, "mesh", new MeshComponent("testCube_pCube1.lrm", ShaderProgramsEnum::TEMP_TEST));
		floor->scale({ 500,0.02,500 });
		floor->move({ 0,-0.6,0 });
	}

	//Cube 2
	Entity* cube = addEntity("cube-test2");
	if (cube)
	{
		addComponent(cube, "mesh", new MeshComponent("testCube_pCube1.lrm", ShaderProgramsEnum::TEMP_TEST));
		cube->scaleUniform({ 3.f });
		cube->move({ 0.f, 5.f, 5.f });
		cube->rotate({ 0.f, XMConvertToRadians(-45.f), XMConvertToRadians(-45.f) });
	}

	// Platforms
	for (int i = 0; i < 5; i++)
	{
		Entity* cube = addEntity("cube-test" + std::to_string(i));
		if (cube)
		{
			addComponent(cube, "mesh", new MeshComponent("testCube_pCube1.lrm", ShaderProgramsEnum::TEMP_TEST));
			cube->scale({ 3,0.2,5 });
			cube->move({ 10.f + (float)i * 3.f, .2f + (float)i, 15.f });
		}
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
	ApplicationLayer::getInstance().m_input.Attach(m_player);
	if (addEntity("meshPlayer"))
	{
		addComponent(m_entities["meshPlayer"], "mesh", new MeshComponent("testTania_tania_geo.lrm", ShaderProgramsEnum::TEMP_TEST));
		m_entities["meshPlayer"]->move({ 1, -0.5, 0 });
		m_entities["meshPlayer"]->scaleUniform(0.02f);
		m_player->setPlayerEntity(m_entities["meshPlayer"]);

		addComponent(m_entities["meshPlayer"], "audio", new AudioComponent(L"Explosion.wav", false, 0.5f));
		addComponent(m_entities["meshPlayer"], "audioLoop", new AudioComponent(L"PickupTunnels.wav", true, 0.5f));
	}
	else
	{
		ErrorLogger::get().logError("No player model added or already exists when adding");
	}

	// Audio test
	Entity* audioTest = addEntity("audioTest");
	addComponent(audioTest, "testSound", new AudioComponent(L"NightAmbienceSimple_02.wav", true, 0.5f));
	nightSlide = 0.1f;
	nightVolume = 0.5f;

	// Temp entity init
	addEntity("first");
	addComponent(m_entities["first"], "test", new TestComponent());

	//Build the hardcoded test stage
	buildTestStage();

}
