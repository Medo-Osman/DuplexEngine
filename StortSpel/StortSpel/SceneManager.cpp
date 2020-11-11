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


	//define gui button
	GUIButtonStyle btnStyle;
	//start button
	btnStyle.position = Vector2(140, 100);
	btnStyle.scale = Vector2(0.5, 0.5);
	m_startGameIndex = GUIHandler::get().addGUIButton(L"ButtonStart.png", btnStyle);


	dynamic_cast<GUIButton*>(GUIHandler::get().getElementMap()->at(m_startGameIndex))->Attach(this);

	//customization button
	btnStyle.position = Vector2(840, 200);
	btnStyle.scale = Vector2(0.5, 0.5);
	m_customization = GUIHandler::get().addGUIButton(L"customizationBtn.png", btnStyle);


	dynamic_cast<GUIButton*>(GUIHandler::get().getElementMap()->at(m_customization))->Attach(this);

	//join button
	btnStyle.position = Vector2(140, 600);
	btnStyle.scale = Vector2(0.5, 0.5);
	m_joinGameIndex = GUIHandler::get().addGUIButton(L"JoinBtn.png", btnStyle);

	
	dynamic_cast<GUIButton*>(GUIHandler::get().getElementMap()->at(m_joinGameIndex))->Attach(this);
	//Host Button
	btnStyle.position = Vector2(140, 700);
	btnStyle.scale = Vector2(0.5, 0.5);
	m_hostGameIndex = GUIHandler::get().addGUIButton(L"HostBtn.png", btnStyle);

	
	dynamic_cast<GUIButton*>(GUIHandler::get().getElementMap()->at(m_hostGameIndex))->Attach(this);

	
	// Start Scene
	m_currentScene = new Scene();
	m_currentScene->loadLobby();
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
			GUIHandler::get().setVisible(m_startGameIndex, true);
			GUIHandler::get().setVisible(m_hostGameIndex, true);
			GUIHandler::get().setVisible(m_joinGameIndex, true);
			GUIHandler::get().setVisible(m_customization, true);

			Engine::get().getPlayerPtr()->setScore(0);
			
			break;
		case ScenesEnum::START:
			m_nextScene->loadScene("Ogorki");
			break;
		case ScenesEnum::ARENA:
			m_nextScene->loadArena();
			break;
		case ScenesEnum::MAINMENU:
			m_nextScene->loadMainMenu();
			break;
		case ScenesEnum::ENDSCENE:
			//
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

void SceneManager::update(GUIUpdateType type, GUIElement* guiElement)
{
	if (type == GUIUpdateType::CLICKED)
	{
		
		if (guiElement->m_index = m_startGameIndex)
		{
			GUIHandler::get().setVisible(m_startGameIndex, false);
			GUIHandler::get().setVisible(m_hostGameIndex, false);
			GUIHandler::get().setVisible(m_joinGameIndex, false);
			GUIHandler::get().setVisible(m_customization, false);

			m_nextSceneEnum = ScenesEnum::START;
			m_swapScene = true;
		}
		if (guiElement->m_index = m_joinGameIndex)
		{
			//do stuff
		}
		if (guiElement->m_index = m_hostGameIndex)
		{
			//do stuff
		}
		if (guiElement->m_index = m_customization)
		{
			//do stuff
		}
		
		
		//m_gameStarted = true;
		//m_gameRestarted = false;
	
	}
}
