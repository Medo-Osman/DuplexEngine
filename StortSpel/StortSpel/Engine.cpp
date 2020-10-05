#include "3DPCH.h"
#include "Engine.h"
#include"ApplicationLayer.h"

Engine::Engine()
{
	m_settings.width = m_startWidth;
	m_settings.height = m_startHeight;
}

void Engine::updateRenderPointLights()
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
		

		/*lightInfo.lightPosArray[nr] = finalPos;
		lightInfo.lightColorArray[nr++] = light.second->getColor();
		lightInfo.nrOfLights = m_lightCount;*/
	}

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
	updateRenderPointLights();
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

		addPointLightComponent(lightComponent);
	}

	return true;
}

void Engine::addMeshComponent(MeshComponent* component)
{
	component->setRenderId(++m_MeshCount);
	m_meshComponentMap[m_MeshCount] = component;
}

void Engine::addPointLightComponent(LightComponent* component)
{
	if (m_lightCount < 8)
	{
		component->setLightID(m_lightCount);
		m_lightComponentMap[m_lightCount++] = component;

		//updateRenderPointLights();
	}
	else
		ErrorLogger::get().logError("Maximum lights achieved, failed to add one.");
}

void Engine::removePointLightComponent(UINT32 id)
{
	int nrOfErased = m_lightComponentMap.erase(id);
	if (nrOfErased > 0) //if it deleted more than 0 elements
	{
		m_lightCount =- nrOfErased;
		//updateRenderPointLights();
	}
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
		addComponent(m_entities["tent"], "mesh", new MeshComponent("BigTopTent_Cylinder.lrm", Material({ L"T_CircusTent_D.png" })));
		m_entities["tent"]->rotate({ XMConvertToRadians(-90.f), 0.f, 0.f });
		m_entities["tent"]->move({ -10.f, 0.f, 0.f });
	}

	Material gridTest = Material({ L"T_GridTestTex.bmp" });

	// Floor
	Entity* floor = addEntity("floor");
	if (floor)
	{
		addComponent(floor, "mesh", new MeshComponent("testCube_pCube1.lrm"));
		floor->scale({ 300,0.1,300 });
		floor->move({ 0,-0.6,0 });
	}

	//Cube 2
	Entity* cube = addEntity("cube-test2");
	if (cube)
	{
		addComponent(cube, "mesh", new MeshComponent("testCube_pCube1.lrm", gridTest));
		cube->scaleUniform({ 3.f });
		cube->move({ 0.f, 5.f, 5.f });
		cube->rotate({ 0.f, XMConvertToRadians(-45.f), XMConvertToRadians(-45.f) });
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
	m_camera.setProjectionMatrix(80.f, (float)m_settings.height / (float)m_settings.width, 0.01f, 1000.0f);
	ApplicationLayer::getInstance().m_input.Attach(m_player);
	if (addEntity("meshPlayer"))
	{
		addComponent(m_entities["meshPlayer"], "mesh", new MeshComponent("testTania_tania_geo.lrm", ShaderProgramsEnum::TEMP_TEST));
		//addComponent(m_entities["meshPlayer"], "lightTest", new PointLightComponent());
		//dynamic_cast<PointLightComponent*>(m_entities["meshPlayer"]->getComponent("lightTest"))->translation({ 0,1.f,0 });

		/*addComponent(m_entities["meshPlayer"], "lightTest2", new LightComponent());
		dynamic_cast<LightComponent*>(m_entities["meshPlayer"]->getComponent("lightTest2"))->translation({ 0,1.f,0 });
		dynamic_cast<LightComponent*>(m_entities["meshPlayer"]->getComponent("lightTest2"))->setColor(XMFLOAT3(1,0,0));
		dynamic_cast<LightComponent*>(m_entities["meshPlayer"]->getComponent("lightTest2"))->setIntensity(1.0f);

		addComponent(m_entities["meshPlayer"], "lightTest", new LightComponent());
		dynamic_cast<LightComponent*>(m_entities["meshPlayer"]->getComponent("lightTest"))->translation({ 3,1.f,3 });
		dynamic_cast<LightComponent*>(m_entities["meshPlayer"]->getComponent("lightTest"))->setColor(XMFLOAT3(1, 0, 0));
		dynamic_cast<LightComponent*>(m_entities["meshPlayer"]->getComponent("lightTest"))->setIntensity(1.0f);

		addComponent(m_entities["meshPlayer"], "lightTest1", new LightComponent());
		dynamic_cast<LightComponent*>(m_entities["meshPlayer"]->getComponent("lightTest1"))->translation({ -3,1.f,3 });
		dynamic_cast<LightComponent*>(m_entities["meshPlayer"]->getComponent("lightTest1"))->setColor(XMFLOAT3(1, 1, 1));
		dynamic_cast<LightComponent*>(m_entities["meshPlayer"]->getComponent("lightTest1"))->setIntensity(1.0f);*/

		addComponent(m_entities["meshPlayer"], "spotlightTest2", new SpotLightComponent());
		dynamic_cast<SpotLightComponent*>(m_entities["meshPlayer"]->getComponent("spotlightTest2"))->translation({ 0,1.f,0 });
		dynamic_cast<SpotLightComponent*>(m_entities["meshPlayer"]->getComponent("spotlightTest2"))->setColor(XMFLOAT3(1, 1, 1));
		//dynamic_cast<SpotLightComponent*>(m_entities["meshPlayer"]->getComponent("lightTest2"))->setColor(XMFLOAT3(0, 1, 0));
		//dynamic_cast<SpotLightComponent*>(m_entities["meshPlayer"]->getComponent("lightTest2"))->setColor(XMFLOAT3(0, 1, 0));


		m_entities["meshPlayer"]->move({ 1, -0.5, 0 });
		m_entities["meshPlayer"]->scaleUniform(0.02f);
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
