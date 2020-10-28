#include "3DPCH.h"
#include "SceneManager.h"

SceneManager::SceneManager()
{
	m_currentScene = nullptr;
	m_nextScene = nullptr;
	m_gameStarted = false;
	m_swapScene = false;
	Physics::get().Attach(this, true, false);
}

SceneManager::~SceneManager()
{
	delete m_currentScene;
	delete m_nextScene;
}

void SceneManager::initalize()
{
	// Start Scene
	m_currentScene = new Scene();
	m_currentScene->loadLobby();
	// Set as PhysicsObserver
	Physics::get().Attach(m_currentScene, false, true);

	// Next Scene
	m_nextScene = nullptr;

	// Update currentScene in engine
	Engine::get().setEntitiesMapPtr(m_currentScene->getEntityMap());
	Engine::get().setLightComponentMapPtr(m_currentScene->getLightMap());
	Engine::get().setMeshComponentMapPtr(m_currentScene->getMeshComponentMap());
}

void SceneManager::updateScene(const float &dt)
{
	if (m_swapScene)
		swapScenes();

	m_currentScene->updateScene(dt);
}

void SceneManager::inputUpdate(InputData& inputData)
{
	for (size_t i = 0; i < inputData.actionData.size(); i++)
	{
		if (inputData.actionData[i] == SWAP_SCENES)
		{
			if (!m_gameStarted)
			{
				m_nextScene = new Scene();
				m_nextScene->loadScene("test");
				swapScenes();
				m_gameStarted = true;
			}
		}
	}
}


//Can't swap scenes when using onTrigger function due to it being triggered in physx simulations, ie it could result in a crash and is not best practice. 
void SceneManager::sendPhysicsMessage(PhysicsData& physicsData, bool& destroyEntity)
{
	if (physicsData.triggerType == TriggerType::EVENT)
	{
		if ((EventType)physicsData.assosiatedTriggerEnum == EventType::SWAPSCENE)
		{
			m_nextScene = new Scene();
			switch ((ScenesEnum)physicsData.intData)
			{
			case ScenesEnum::LOBBY:
				m_nextScene->loadLobby();
				break;
			case ScenesEnum::START:
				m_nextScene->loadScene("Ogorki");
				break;
			case ScenesEnum::ARENA:
				m_nextScene->loadArena();
				break;
			default:
				break;
			}
			m_swapScene = true;
		}
	}
}

void SceneManager::swapScenes()
{
	m_swapScene = false;
	Physics::get().Detach(m_currentScene, false, true);

	// Swap
	delete m_currentScene;
	m_currentScene = m_nextScene;
	m_nextScene = nullptr;

	// Update currentScene in engine
	Engine::get().setEntitiesMapPtr(m_currentScene->getEntityMap());
	Engine::get().setLightComponentMapPtr(m_currentScene->getLightMap());
	Engine::get().setMeshComponentMapPtr(m_currentScene->getMeshComponentMap());

	// Set as PhysicsObserver
	Physics::get().Attach(m_currentScene, false, true);

	static_cast<CharacterControllerComponent*>(Engine::get().getPlayerPtr()->getPlayerEntity()->getComponent("CCC"))->setPosition(m_currentScene->getEntryPosition());
}
