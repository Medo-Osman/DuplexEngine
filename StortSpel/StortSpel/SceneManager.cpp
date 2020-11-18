#include "3DPCH.h"
#include "SceneManager.h"

SceneManager::SceneManager()
{
	m_currentScene = nullptr;
	m_nextScene = nullptr;
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


	//define gui button
	GUIButtonStyle btnStyle;
	//start button
	btnStyle.position = Vector2(140, 300);
	btnStyle.scale = Vector2(1, 1);
	m_singleplayerIndex = GUIHandler::get().addGUIButton(L"singleplayerBtn.png", btnStyle);

	dynamic_cast<GUIButton*>(GUIHandler::get().getElementMap()->at(m_singleplayerIndex))->Attach(this);


	//Exit button
	btnStyle.position = Vector2(1600, 100);
	btnStyle.scale = Vector2(1, 1);
	m_exitIndex = GUIHandler::get().addGUIButton(L"exitBtn.png", btnStyle);


	dynamic_cast<GUIButton*>(GUIHandler::get().getElementMap()->at(m_exitIndex))->Attach(this);

	//join button
	btnStyle.position = Vector2(140, 550);
	btnStyle.scale = Vector2(1, 1);
	m_joinGameIndex = GUIHandler::get().addGUIButton(L"joinBtn.png", btnStyle);

	dynamic_cast<GUIButton*>(GUIHandler::get().getElementMap()->at(m_joinGameIndex))->Attach(this);

	//Host Button
	btnStyle.position = Vector2(140, 700);
	btnStyle.scale = Vector2(1, 1);
	m_hostGameIndex = GUIHandler::get().addGUIButton(L"hostBtn.png", btnStyle);

	dynamic_cast<GUIButton*>(GUIHandler::get().getElementMap()->at(m_hostGameIndex))->Attach(this);


	// Start Scene
	m_currentScene = new Scene();

	Scene::loadMainMenu(m_currentScene, m_nextSceneReady);
	disableMovement();
	*m_nextSceneReady = false;
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
		std::thread sceneLoaderThread;
		switch (m_nextSceneEnum)
		{
		case ScenesEnum::LOBBY:
			sceneLoaderThread = std::thread(Scene::loadLobby, m_nextScene, m_nextSceneReady);
			sceneLoaderThread.detach();
			disableMovement();
			//Reset game variables that are needed here
			GUIHandler::get().setVisible(m_singleplayerIndex, true);
			GUIHandler::get().setVisible(m_hostGameIndex, true);
			GUIHandler::get().setVisible(m_joinGameIndex, true);
			GUIHandler::get().setVisible(m_exitIndex, true);
			Engine::get().getPlayerPtr()->setScore(0);
			m_gameStarted = false;
			m_loadNextSceneWhenReady = true; //Tell scene manager to switch to the next scene as soon as the next scene finished loading.
			break;
		case ScenesEnum::START:
			sceneLoaderThread = std::thread(Scene::loadScene, m_nextScene, "Ogorki", m_nextSceneReady);
			sceneLoaderThread.detach();
			m_gameStarted = true;
			enableMovement();
			m_loadNextSceneWhenReady = true; //Tell scene manager to switch to the next scene as soon as the next scene finished loading.
			break;
		case ScenesEnum::ARENA:
			sceneLoaderThread = std::thread(Scene::loadArena, m_nextScene, m_nextSceneReady);
			sceneLoaderThread.detach();

			m_gameStarted = true;
			m_loadNextSceneWhenReady = true; //Tell scene manager to switch to the next scene as soon as the next scene finished loading.
			break;
		case ScenesEnum::MAINMENU:
			disableMovement();
			m_nextScene->loadMainMenu();
			break;
		case ScenesEnum::ENDSCENE:
			//
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
			std::thread sceneLoaderThread = std::thread(Scene::loadScene, m_nextScene, "levelMeshTest", m_nextSceneReady);
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

void SceneManager::disableMovement()
{
	m_contexts->at(0)->setMute(true);
}

void SceneManager::enableMovement()
{
	m_contexts->at(0)->setMute(false);
}

void SceneManager::setContextPtr(std::vector<iContext*>* contexts)
{
	m_contexts = contexts;
}

std::vector<iContext*>* SceneManager::getContextPtr()
{
	return m_contexts;
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
		//hej detta �r big changes
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
}

void SceneManager::update(GUIUpdateType type, GUIElement* guiElement)
{
	if (type == GUIUpdateType::HOVER_ENTER)
	{
		GUIButton* buttonEnter = dynamic_cast<GUIButton*>(guiElement);
		if (guiElement->m_index == m_exitIndex)
		{
			buttonEnter->setTexture(L"exitBtnHover.png");
		}
		if (guiElement->m_index == m_singleplayerIndex)
		{
			buttonEnter->setTexture(L"singleplayerBtnHover.png");
		}
		if (guiElement->m_index == m_joinGameIndex)
		{
			buttonEnter->setTexture(L"joinBtnHover.png");
		}
		if (guiElement->m_index == m_hostGameIndex)
		{
			buttonEnter->setTexture(L"hostBtnHover.png");
		}
	}
	if (type == GUIUpdateType::HOVER_EXIT)
	{
		GUIButton* button = dynamic_cast<GUIButton*>(guiElement);
		if (guiElement->m_index == m_exitIndex)
		{
			button->setTexture(L"exitBtn.png");
		}
		if (guiElement->m_index == m_singleplayerIndex)
		{
			button->setTexture(L"singleplayerBtn.png");
		}
		if (guiElement->m_index == m_joinGameIndex)
		{
			button->setTexture(L"joinBtn.png");
		}
		if (guiElement->m_index == m_hostGameIndex)
		{
			button->setTexture(L"hostBtn.png");
		}
	}


	//Checks which button is being clicked
	if (type == GUIUpdateType::CLICKED)
	{

		if (guiElement->m_index == m_singleplayerIndex)
		{
			m_gameStarted = true;
			GUIHandler::get().setVisible(m_singleplayerIndex, false);
			GUIHandler::get().setVisible(m_hostGameIndex, false);
			GUIHandler::get().setVisible(m_joinGameIndex, false);
			GUIHandler::get().setVisible(m_exitIndex, false);
			m_nextSceneEnum = ScenesEnum::START;
			m_swapScene = true;
		}
		if (guiElement->m_index == m_joinGameIndex)
		{
			//do stuff
		}
		if (guiElement->m_index == m_hostGameIndex)
		{
			//do stuff
		}
		if (guiElement->m_index == m_exitIndex)
		{
			endGame = true;
			//do stuff
		}


		//m_gameStarted = true;
		//m_gameRestarted = false;

	}
}
