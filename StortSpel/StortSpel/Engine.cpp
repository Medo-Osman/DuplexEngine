#include "3DPCH.h"
#include "Engine.h"
#include"ApplicationLayer.h"
#include"CharacterControllerComponent.h"
#include"TriggerComponent.h"
#include"RotateComponent.h"
#include"PickupComponent.h"
#include"Particles\PlayerLineParticle.h"

Engine::Engine()
{
	//m_settings.width = m_startWidth;
	//m_settings.height = m_startHeight;
			// OR!
	//static const int m_startWidth = ApplicationLayer::getInstance().m_width;
	//static const int m_startHeight = ApplicationLayer::m_height;

	int x = ApplicationLayer::getInstance().m_width;
	int y = ApplicationLayer::getInstance().m_height;

	m_settings.width = x;
	m_settings.height = y;
}

Engine& Engine::get()
{
	static Engine instance;
	return instance;
}

Engine::~Engine()
{
	if (m_player)
		delete m_player;

	//static const int m_startWidth = ApplicationLayer::getInstance().m_width;
	//static const int m_startHeight = ApplicationLayer::m_height;
	//for (std::pair<std::string, Entity*> entity : m_entities)
	//	delete entity.second;
	
	//m_entities->clear();
	//m_entities->clear();
}

void Engine::release()
{
}

void Engine::update(const float& dt)
{


	//Example for updating light direction
	/*Vector4 dir = m_skyLightDir;
	dir = XMVector3TransformCoord(dir, XMMatrixRotationY(XMConvertToRadians(2.f)));
	m_skyLightDir = dir;*/

	for (auto& entity : *m_entities)
	{
		entity.second->update(dt);
	}

	if (AudioHandler::get().getAudioChanged())
	{
		AudioHandler::get().update(dt);
	}

	m_camera.update(dt);
	m_player->updatePlayer(dt);
	m_camera.setProjectionMatrix(m_camera.fovAmount, (float)m_settings.width / (float)m_settings.height, 0.01f, 1000.0f);
	updateLightData();

}

void Engine::setEntitiesMapPtr(std::unordered_map<std::string, Entity*>* entities)
{
	m_entities = entities;
}
void Engine::setMeshComponentMapPtr(std::unordered_map<unsigned int long, MeshComponent*>* meshComponents)
{
	m_meshComponentMap = meshComponents;
}

void Engine::setLightComponentMapPtr(std::unordered_map<std::string, LightComponent*>* lightComponents)
{
	m_lightComponentMap = lightComponents;
}

bool Engine::addComponentToPlayer(std::string componentIdentifier, Component* component)
{
	Entity* playerEntity = m_player->getPlayerEntity();
	playerEntity->addComponent(componentIdentifier, component);

	if (component->getType() == ComponentType::MESH)
	{
		//M eshComponent* meshComponent = dynamic_cast<MeshComponent*>(component);
		//m_currentScene->addMeshComponent(meshComponent);
		//meshComponent->setRenderId(++m_meshCount);
		//m_meshComponentMap->insert({m_meshCount, meshComponent});
	}

	//if (component->getType() == ComponentType::LIGHT)
	//{
	//	//LightComponent* lightComponent = dynamic_cast<LightComponent*>(component);

	//	//if (m_lightCount < 8)
	//	//{
	//	//	lightComponent->setLightID(component->getIdentifier());
	//	//	m_lightComponentMap->insert({component->getIdentifier(), lightComponent});
	//	//	m_lightCount++;
	//	//}
	//	//else
	//	//	ErrorLogger::get().logError("Maximum lights achieved, failed to add one.");

	//	//m_currentScene->addLightComponent(lightComponent);
	//}

	return true;
}


std::unordered_map<unsigned int long, MeshComponent*>* Engine::getMeshComponentMap()
{
	return m_meshComponentMap;
}

std::unordered_map<std::string, LightComponent*>* Engine::getLightComponentMap()
{
	return m_lightComponentMap;
}

std::unordered_map<std::string, Entity*>* Engine::getEntityMap()
{
	return m_entities;
}

Input* Engine::getInput()
{
	return m_input;
}

Vector4& Engine::getSkyLightDir()
{
	return m_skyLightDir;
}

void Engine::setSkyLightDir(Vector4 dir)
{
	m_skyLightDir = dir;
}

void Engine::setSkyLightColor(Vector4 color)
{
	m_skyLightColor = color;
}

void Engine::setSkyLightIntensity(float intensity)
{
	m_skyLightBrightness = intensity;
}

Settings Engine::getSettings() const
{
	return m_settings;
}
Camera* Engine::getCameraPtr()
{
	return &m_camera;
}

float Engine::getGameTime() const
{
	return ApplicationLayer::getInstance().getGameTime();
}
Player* Engine::getPlayerPtr()
{
	return m_player;
}

void Engine::setDeviceAndContextPtrs(ID3D11Device* devicePtr, ID3D11DeviceContext* dContextPtr)
{
	m_devicePtr = devicePtr;
	m_dContextPtr = dContextPtr;

	DeviceAndContextPtrsAreSet = true;
}

void Engine::initialize(Input* input)
{
	m_input = input;

	if (!DeviceAndContextPtrsAreSet)
	{
		// Renderer::initialize needs to be called and it needs to call setDeviceAndContextPtrs()
		// before this function call be called.
		assert(false);
	}
	Material::readMaterials();
	m_camera.initialize(m_camera.fovAmount,(float)m_settings.width / (float)m_settings.height, 0.01f, 1000.0f);
	// Audio Handler Listener setup
	AudioHandler::get().setListenerTransformPtr(m_camera.getTransform());
	

	// Player
	m_player = new Player();

	ApplicationLayer::getInstance().m_input.Attach(m_player);

	// - Entity
	Entity* playerEntity = new Entity(PLAYER_ENTITY_NAME);
	playerEntity->setPosition({ 0, 0, 0 });

	//playerEntity->scaleUniform(0.02f);

	// - Mesh Componenet
	//AnimatedMeshComponent* animMeshComp = new AnimatedMeshComponent("Lucy1.lrsm", { ShaderProgramsEnum::SKEL_PBR, ShaderProgramsEnum::SKEL_PBR, ShaderProgramsEnum::SKEL_PBR, ShaderProgramsEnum::LUCY_FACE }, { Material(L"Cloth", true), Material(L"Skin", true), Material(L"Hair", true), Material(L"LucyEyes") });
	//AnimatedMeshComponent* animMeshComp = new AnimatedMeshComponent("Lucy1.lrsm", { ShaderProgramsEnum::LUCY_FACE }, { Material(L"Cloth" /*+ std::to_wstring(1)*/), Material(L"Skin"), Material(L"Hair"), Material(L"LucyEyes") });
	AnimatedMeshComponent* animMeshComp = new AnimatedMeshComponent("Lucy_ReRigged.lrsm", { ShaderProgramsEnum::LUCY_FACE }, { Material(L"Cloth" /*+ std::to_wstring(1)*/), Material(L"Skin"), Material(L"Hair"), Material(L"LucyEyes") });
	playerEntity->addComponent("mesh", animMeshComp);
	playerEntity->setScaleUniform(0.5f);

	//animMeshComp->playAnimation("Running4.1", true);
	//animMeshComp->playSingleAnimation("Running4.1", 0.0f);

	//animMeshComp->addAndPlayBlendState({ {"Idle", 0.f}, {"RunLoop", 1.f} }, "runOrIdle", 0.f, true, true);
	animMeshComp->addAndPlayBlendState({ {"Idle", 0.f}, {"runReRigged", 1.f} }, "runOrIdle", 0.f, true, true);
	animMeshComp->setAnimationSpeed(1, 1.7f);

	m_player->setAnimMeshPtr(animMeshComp);

	//a4->setAnimationSpeed(0.05f);
	// - Physics Componenet
	playerEntity->addComponent("CCC", new CharacterControllerComponent());
	CharacterControllerComponent* pc = static_cast<CharacterControllerComponent*>(playerEntity->getComponent("CCC"));
	pc->initController(playerEntity, PLAYER_CAPSULE_HEIGHT, PLAYER_CAPSULE_RADIUS, "human");

	// - Camera Follow Transform ptr
	m_player->setCameraTranformPtr(m_camera.getTransform());

	// - set player Entity
	m_player->setPlayerEntity(playerEntity);
	//GUIHandler::get().initialize(m_devicePtr.Get(), m_dContextPtr.Get());

	// Audio Handler needs Camera Transform ptr for 3D positional audio
	AudioHandler::get().setListenerTransformPtr(m_camera.getTransform());
	

	Renderer::get().setFullScreen(false);
}

void Engine::updateLightData()
{
	lightBufferStruct lightInfo;
	int nrPointLights = 0;
	int nrSpotLights = 0;
	Entity* tempEntity;
	for (auto light : *m_lightComponentMap)
	{
		if ((light.second->getParentEntityIdentifier()) == PLAYER_ENTITY_NAME)
			tempEntity = m_player->getPlayerEntity();
		else
			tempEntity = m_entities->at(light.second->getParentEntityIdentifier());

		Matrix parentTransform = XMMatrixTranslationFromVector(tempEntity->getTranslation());

		Vector3 offsetFromParent = light.second->getTranslation();

		Matrix parentRotation = tempEntity->getRotationMatrix();

		Vector3 finalPos = XMVector3TransformCoord(offsetFromParent, parentRotation * parentTransform);

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
			spotLight.direction = Vector3(XMVector3TransformCoord(XMVectorSet(spotLightComponent->getDirection().x, spotLightComponent->getDirection().y, spotLightComponent->getDirection().z, 0), parentRotation));

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
