#include "3DPCH.h"
#include "Engine.h"

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



void Engine::update(Mouse* mousePtr, Keyboard* keyboardPtr, const float& dt)
{
	m_player->updatePlayer(keyboardPtr, dt);
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

	Entity* floor = addEntity("floor");
	if (floor)
	{
		addComponent(floor, "mesh", new MeshComponent("../res/models/testCube_pCube1.lrm"));
		floor->scale({ 500,0.02,500 });
		floor->move({ 0,-0.6,0 });
	}
	

	Entity* cube = addEntity("cube-test");
	if (cube)
	{
		addComponent(cube, "mesh", new MeshComponent("../res/models/testCube_pCube1.lrm"));
		cube->scaleUniform({ 1.4 });
		cube->move({ 0.2,0.2,3 });
	}
	
	for (int i = 0; i < 5; i++)
	{
		Entity* cube = addEntity("cube-test"+std::to_string(i));
		if (cube)
		{
			addComponent(cube, "mesh", new MeshComponent("../res/models/testCube_pCube1.lrm"));
			cube->scale({ 3,0.2,5 });
			cube->move({ (float)30+i*3,(float)0.2+i,15 });
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
	if (addEntity("meshPlayer"))
	{
		
		addComponent(m_entities["meshPlayer"], "mesh", new MeshComponent("../res/models/testTania_tania_geo.lrm"));
		m_entities["meshPlayer"]->move({ 1, -0.5, 0 });
		m_entities["meshPlayer"]->scaleUniform(0.02f);
		m_player->setPlayerEntity(m_entities["meshPlayer"]);
	}
	else
	{
		ErrorLogger::get().logError("No player model added or already exists when adding");
	}


	//Build the hardcoded test stage
	buildTestStage();
	

	// Temp entity init
	addEntity("first");
	//m_entities["first"]->addComponent("test", new TestComponent());
	addComponent(m_entities["first"], "test", new TestComponent());

	if(addEntity("meshTest"))
		addComponent(m_entities["meshTest"], "mesh", new MeshComponent("../res/models/testCube_pCube1.lrm"));
		//m_entities["meshTest"]->addComponent("mesh", new MeshComponent("../res/models/testCube_pCube1.lrm"));


	/*if (addEntity("meshTest1"))
	{
		m_entities["meshTest1"]->addComponent("mesh", new MeshComponent("../res/models/testCube_pCube1.lrm"));

		m_entities["meshTest1"]->move({ 2, 1, 1});

		MeshComponent* meshPtr = dynamic_cast<MeshComponent*>(m_entities["meshTest1"]->getComponent("mesh"));
		if (meshPtr)
		{
			meshPtr->move({ 2, 10, 1 });
		}
	}

	if (addEntity("meshTest2"))
	{
		m_entities["meshTest2"]->addComponent("mesh", new MeshComponent("../res/models/testTania_tania_geo.lrm"));
		m_entities["meshTest2"]->move({ -5, -1, 0 });
		m_entities["meshTest2"]->scaleUniform(0.02f);

	}

	}*/

}
