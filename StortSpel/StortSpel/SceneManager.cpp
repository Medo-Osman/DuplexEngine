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
	btnStyle.position = Vector2(140, 200);
	btnStyle.scale = Vector2(0.5, 0.5);
	m_singleplayerIndex = GUIHandler::get().addGUIButton(L"closeButton.png", btnStyle);

	dynamic_cast<GUIButton*>(GUIHandler::get().getElementMap()->at(m_singleplayerIndex))->Attach(this);


	////Exit button
	//btnStyle.position = Vector2(1600, 100);
	//btnStyle.scale = Vector2(1, 1);
	//m_exitIndex = GUIHandler::get().addGUIButton(L"exitBtn.png", btnStyle);


	//dynamic_cast<GUIButton*>(GUIHandler::get().getElementMap()->at(m_exitIndex))->Attach(this);

	////join button
	//btnStyle.position = Vector2(140, 550);
	//btnStyle.scale = Vector2(1, 1);
	//m_joinGameIndex = GUIHandler::get().addGUIButton(L"joinBtn.png", btnStyle);

	//dynamic_cast<GUIButton*>(GUIHandler::get().getElementMap()->at(m_joinGameIndex))->Attach(this);

	////Host Button
	//btnStyle.position = Vector2(140, 700);
	//btnStyle.scale = Vector2(1, 1);
	//m_hostGameIndex = GUIHandler::get().addGUIButton(L"hostBtn.png", btnStyle);
	//
	//dynamic_cast<GUIButton*>(GUIHandler::get().getElementMap()->at(m_hostGameIndex))->Attach(this);

	
	// Start Scene
	m_currentScene = new Scene();
	m_currentScene->loadMainMenu();
	disableMovement();
	
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
	std::cout << m_currentScene->disMovment << std::endl;
	/*if (m_currentScene->disMovment)
	{
		disableMovement();
	}*/
	if (m_swapScene)
	{
		
		m_nextScene = new Scene();
		switch (m_nextSceneEnum)
		{
		case ScenesEnum::LOBBY:
			m_nextScene->loadLobby();
			//Reset game variables that are needed here
			GUIHandler::get().setVisible(m_singleplayerIndex, true);
			GUIHandler::get().setVisible(m_hostGameIndex, true);
			GUIHandler::get().setVisible(m_joinGameIndex, true);
			GUIHandler::get().setVisible(m_exitIndex, true);
			Engine::get().getPlayerPtr()->setScore(0);
			disableMovement();

			break;
		case ScenesEnum::START:
			m_nextScene->loadScene("Ogorki");
			m_gameStarted = true;
			enableMovement();
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

	//if (type == GUIUpdateType::HOVER_ENTER)
	//{
	//	GUIButton* button = dynamic_cast<GUIButton*>(guiElement);
	//	button->setTexture(L"exitbtn.png");
	//}
	//

	//if (type == GUIUpdateType::HOVER_EXIT)
	//{
	//	GUIButton* button = dynamic_cast<GUIButton*>(guiElement);
	//	button->setTexture(L"exitbtnhover.png");
	//}

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
			
			//do stuff
		}
		
	
		//m_gameStarted = true;
		//m_gameRestarted = false;
	
	}
}
