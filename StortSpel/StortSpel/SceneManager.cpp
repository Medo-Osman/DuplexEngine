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
	*m_nextSceneReady = false;
	m_currentScene = new Scene();
	Scene::loadLobby(m_currentScene, m_nextSceneReady); //Single thread first load-in.
	*m_nextSceneReady = false; //Because init is required

	// Set as PhysicsObserver
	Physics::get().Attach(m_currentScene, false, true);
	static_cast<CharacterControllerComponent*>(Engine::get().getPlayerPtr()->getPlayerEntity()->getComponent("CCC"))->setPosition(m_currentScene->getEntryPosition());

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
	{
		std::cout << "new scene" << std::endl;
		m_nextScene = new Scene();
		std::thread sceneLoaderThread;
		switch (m_nextSceneEnum)
		{
		case ScenesEnum::LOBBY:
			sceneLoaderThread = std::thread(Scene::loadLobby, m_nextScene, m_nextSceneReady);
			sceneLoaderThread.detach();

			//Reset game variables that are needed here
			Engine::get().getPlayerPtr()->setScore(0);
			m_gameStarted = false;
			m_loadNextSceneWhenReady = true; //Tell scene manager to switch to the next scene as soon as the next scene finished loading.
			break;
		case ScenesEnum::START:
			sceneLoaderThread = std::thread(Scene::loadScene, m_nextScene, "Ogorki", m_nextSceneReady);
			sceneLoaderThread.detach();

			m_loadNextSceneWhenReady = true; //Tell scene manager to switch to the next scene as soon as the next scene finished loading.
			break;
		case ScenesEnum::ARENA:
			sceneLoaderThread = std::thread(Scene::loadArena, m_nextScene, m_nextSceneReady);
			sceneLoaderThread.detach();

			m_gameStarted = true;
			m_loadNextSceneWhenReady = true; //Tell scene manager to switch to the next scene as soon as the next scene finished loading.
			break;
		default:
			break;
		}
	}

		swapScenes();
	m_currentScene->updateScene(dt);
}

void SceneManager::inputUpdate(InputData& inputData)
{
	for (size_t i = 0; i < inputData.actionData.size(); i++)
	{
		if (inputData.actionData[i] == TEST_SCENE)
		{
			if (!m_gameStarted)
			{
				m_nextScene = new Scene();

				std::thread sceneLoaderThread(Scene::loadTestLevel, m_nextScene, m_nextSceneReady);
				sceneLoaderThread.detach();

				m_gameStarted = true;
				m_loadNextSceneWhenReady = true; //Tell scene manager to switch to the next scene as soon as the next scene finished loading.
			}
		}
		else if (inputData.actionData[i] == LOAD_SCENE)
		{
			m_nextScene = new Scene();
			std::thread sceneLoaderThread = std::thread(Scene::loadLobby, m_nextScene, m_nextSceneReady);
			sceneLoaderThread.detach();

			m_gameStarted = false;
			m_loadNextSceneWhenReady = true; //Tell scene manager to switch to the next scene as soon as the next scene finished loading.
		}
		else if (inputData.actionData[i] == LOAD_TEST_SCENE)
		{
			m_nextScene = new Scene();
			std::thread sceneLoaderThread = std::thread(Scene::loadArena, m_nextScene, m_nextSceneReady);
			sceneLoaderThread.detach();

			m_gameStarted = true;
			m_loadNextSceneWhenReady = true; //Tell scene manager to switch to the next scene as soon as the next scene finished loading.
		}
	}
}


//Can't swap scenes when using onTrigger function due to it being triggered in physx simulations, ie it could result in a crash and is not best practice. 
void SceneManager::sendPhysicsMessage(PhysicsData& physicsData, bool& destroyEntity)
{
	if (physicsData.triggerType == TriggerType::EVENT)
	{
		if ((EventType)physicsData.associatedTriggerEnum == EventType::SWAPSCENE)
		{
			m_nextSceneEnum = (ScenesEnum)physicsData.intData;
			m_swapScene = true;
		}
	}

	if (physicsData.triggerType == TriggerType::TRAP)
	{
		if ((TrapType)physicsData.associatedTriggerEnum == TrapType::PUSH)
		{
			static_cast<PushTrapComponent*>(physicsData.pointer)->push();
		}
	}

	if (physicsData.triggerType == TriggerType::TRAP)
	{
		if ((TrapType)physicsData.associatedTriggerEnum == TrapType::BARRELTRIGGER)
		{
			BarrelTriggerComponent* barrelTriggerPtr = static_cast<BarrelTriggerComponent*>(physicsData.pointer);
		
			if (barrelTriggerPtr->m_triggerTimer.timeElapsed() >= 3)
			{
				m_currentScene->addBarrelDrop(Vector3(-30, 50, 130));
				barrelTriggerPtr->m_triggerTimer.restart();
				m_currentScene->addedBarrel = true;
			}
			
			
		}
	}
}

void SceneManager::swapScenes()
{
	m_swapScene = false;
	if (*m_nextSceneReady == true && m_loadNextSceneWhenReady)
	{
		*m_nextSceneReady = false;
		m_loadNextSceneWhenReady = false;
		Physics::get().Detach(m_currentScene, false, true);
	
		//Reset boss
		if (m_currentScene->m_boss)
			m_currentScene->m_boss->Detach(m_currentScene);

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
		Physics::get().changeScene(m_currentScene->getSceneID());
		CharacterControllerComponent* ccc = static_cast<CharacterControllerComponent*>(Engine::get().getPlayerPtr()->getPlayerEntity()->getComponent("CCC"));
		ccc->initController(Engine::get().getPlayerPtr()->getPlayerEntity(), 1.75f, 0.5, "human");
		ccc->setPosition(m_currentScene->getEntryPosition());
	}

	//std::cout << "Ready: " << *m_nextSceneReady << std::endl;
}
