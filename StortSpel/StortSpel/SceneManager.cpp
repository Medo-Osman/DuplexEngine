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

void printStuff()
{

	for(int i = 0; i < 100000000000; i++)
		std::cout << "thread: " << i++ << std::endl;
}

void SceneManager::initalize()
{

	//for (int i = 0; i < 100000000000; i++)
		//std::cout << "thread: " << i++ << std::endl;

	// Start Scene
	m_currentScene = new Scene();
	m_currentScene->loadLobby();

	Timer timer;
	timer.start();
	m_nextScene = new Scene();
	std::thread myThread(Scene::loadScene, m_nextScene); //Create a popupthread
	myThread.detach(); //Otherwise it will try to terminate once the function exits
	std::cout << "CONTINUED ON IN MAIN - seconds since starting thread: " << timer.timeElapsed() << std::endl;

	//m_currentScene->loadScene(m_currentScene);
	//result = std::future<void>(std::async(std::launch::async, printStuff)); //Works
	//result = std::future<void>(std::async(std::launch::async, Scene::loadScene, m_currentScene)); //Works 2
	/*result = std::async(std::launch::async, [](Scene* nextScene) {
		nextScene->loadScene();
		}, m_nextScene);*/


	

	//m_currentScene->loadArena();
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
		m_nextScene = new Scene();
		switch (m_nextSceneEnum)
		{
		case ScenesEnum::LOBBY:
			m_nextScene->loadLobby();
			//Reset game variables that are needed here
			Engine::get().getPlayerPtr()->setScore(0);
			m_gameStarted = false;
			break;
		case ScenesEnum::START:
			//m_nextScene->loadScene("Ogorki");

			

			
			break;
		case ScenesEnum::ARENA:
			m_nextScene->loadArena();
			break;
		default:
			break;
		}

		swapScenes();
	}

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
				//result = std::async(std::launch::async, Scene::loadScene, m_currentScene).share();
				
				//m_nextScene->loadScene("test");
				//std::thread thread;
				/*thread = std::thread([](Scene* nextScene) {
					nextScene->loadScene("testLevel");
					}, m_nextScene);*/
				
				Timer timer;
				timer.start();
				std::thread myThread(Scene::loadScene, m_nextScene); //Create a popupthread
				myThread.detach(); //Otherwise it will try to terminate once the function exits
				std::cout << "CONTINUED ON IN MAIN - seconds since starting thread: " << timer.timeElapsed() << std::endl;


				swapScenes();
				m_gameStarted = true;
				//thread.join();
			}
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

	static_cast<CharacterControllerComponent*>(Engine::get().getPlayerPtr()->getPlayerEntity()->getComponent("CCC"))->setPosition(m_currentScene->getEntryPosition());
}
