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
	delete m_nextSceneReady;
}

void SceneManager::initalize()
{
	//define gui button
	GUIButtonStyle btnStyle;
	//start button
	btnStyle.position = Vector2(140, 300);
	btnStyle.scale = Vector2(1, 1);
	m_singleplayerIndex = GUIHandler::get().addGUIButton(L"singleplayerBtn.png", btnStyle);

	GUIButton* startButton = dynamic_cast<GUIButton*>(GUIHandler::get().getElementMap()->at(m_singleplayerIndex));
	startButton->Attach(this);


	//start button
	btnStyle.position = Vector2(140, 100);
	btnStyle.scale = Vector2(1, 1);
	m_multiPlayerIndexTwo = GUIHandler::get().addGUIButton(L"2.png", btnStyle);

	GUIButton* peopleButton = dynamic_cast<GUIButton*>(GUIHandler::get().getElementMap()->at(m_multiPlayerIndexTwo));
	peopleButton->Attach(this);

	btnStyle.position = Vector2(140, 300);
	btnStyle.scale = Vector2(1, 1);
	m_multiPlayerIndexThree = GUIHandler::get().addGUIButton(L"3.png", btnStyle);

	peopleButton = dynamic_cast<GUIButton*>(GUIHandler::get().getElementMap()->at(m_multiPlayerIndexThree));
	peopleButton->Attach(this);

	btnStyle.position = Vector2(140, 500);
	btnStyle.scale = Vector2(1, 1);
	m_multiPlayerIndexFour = GUIHandler::get().addGUIButton(L"4.png", btnStyle);

	peopleButton = dynamic_cast<GUIButton*>(GUIHandler::get().getElementMap()->at(m_multiPlayerIndexFour));
	peopleButton->Attach(this);

	GUIHandler::get().setVisible(m_multiPlayerIndexTwo, false);
	GUIHandler::get().setVisible(m_multiPlayerIndexThree, false);
	GUIHandler::get().setVisible(m_multiPlayerIndexFour, false);


	//join button
	btnStyle.position = Vector2(140, 550);
	btnStyle.scale = Vector2(1, 1);
	m_joinGameIndex = GUIHandler::get().addGUIButton(L"joinBtn.png", btnStyle);

	GUIButton* joinButton = dynamic_cast<GUIButton*>(GUIHandler::get().getElementMap()->at(m_joinGameIndex));
	joinButton->Attach(this);
	joinButton->setPrevMenuButton(startButton);
	startButton->setNextMenuButton(joinButton);

	//Host Button
	btnStyle.position = Vector2(140, 700);
	btnStyle.scale = Vector2(1, 1);
	m_hostGameIndex = GUIHandler::get().addGUIButton(L"hostBtn.png", btnStyle);

	GUIButton* hostButton = dynamic_cast<GUIButton*>(GUIHandler::get().getElementMap()->at(m_hostGameIndex));
	hostButton->Attach(this);
	hostButton->setPrevMenuButton(joinButton);
	joinButton->setNextMenuButton(hostButton);

	//Exit button
	btnStyle.position = Vector2(140, 850);
	btnStyle.scale = Vector2(1, 1);
	m_exitIndex = GUIHandler::get().addGUIButton(L"exitBtn.png", btnStyle);

	GUIButton* exitButton = dynamic_cast<GUIButton*>(GUIHandler::get().getElementMap()->at(m_exitIndex));

	btnStyle.position = Vector2(700, 800);
	btnStyle.scale = Vector2(1.5, 1.5);
	m_backToLobbyIndex = GUIHandler::get().addGUIButton(L"backToLobby.png", btnStyle);

	GUIButton* backToLobbyButton = dynamic_cast<GUIButton*>(GUIHandler::get().getElementMap()->at(m_backToLobbyIndex));
	backToLobbyButton->Attach(this);

	exitButton->Attach(this);
	exitButton->setPrevMenuButton(hostButton);
	exitButton->setNextMenuButton(startButton);
	startButton->setPrevMenuButton(exitButton);
	hostButton->setNextMenuButton(exitButton);

	// Used for Menu Selection
	GUIHandler::get().setInMenu(true, m_singleplayerIndex);

	// Start Scene
	m_currentScene = new Scene();
	 //Single thread first load-in.
	Scene::loadMainMenu(m_currentScene, m_nextSceneReady);
	disableMovement();
	*m_nextSceneReady = false;
	//m_currentScene->loadArena();

	m_currentScene->onSceneLoaded();

	// Set as PhysicsObserver
	Physics::get().Attach(m_currentScene, false, true);
	static_cast<CharacterControllerComponent*>(Engine::get().getPlayerPtr()->getPlayerEntity()->getComponent("CCC"))->setPosition(m_currentScene->getEntryPosition());

	// Next Scene
	m_nextScene = nullptr;

	// Update currentScene in engine
	Engine::get().setEntitiesMapPtr(m_currentScene->getEntityMap());
	Engine::get().setLightComponentMapPtr(m_currentScene->getLightMap());
	Engine::get().setMeshComponentMapPtr(m_currentScene->getMeshComponentMap());

	m_bossScene = new Scene();
	std::thread sceneLoaderThread = std::thread(Scene::loadBossTest, m_bossScene, new bool);
	sceneLoaderThread.detach();

	Physics::get().blockRemovalOfScene(m_bossScene->getSceneID());
	Engine::get().setBossEntityMap(m_bossScene->getEntityMap());

	m_camera = Engine::get().getCameraPtr();
	setScorePtr(m_currentScene->getScores());
	
	GUITextStyle style;

	style.position.y = 120.f;
	style.scale = { 0.5f };
	m_highScoreLabelIndex = GUIHandler::get().addGUIText("High Score", L"squirk.spritefont", style);
	style.position.x = 160.f;
	style.position.y = 300.f;

	style.color = Colors::Yellow;
	m_playerOneScoreIndex = GUIHandler::get().addGUIText(std::to_string(m_scores->at(0).second), L"squirk.spritefont", style);
	style.position.y -= 50.0f;
	m_playerTwoScoreIndex = GUIHandler::get().addGUIText(std::to_string(m_scores->at(1).second), L"squirk.spritefont", style);
	style.position.y -= 50.0f;
	m_playerThreeScoreIndex = GUIHandler::get().addGUIText(std::to_string(m_scores->at(2).second), L"squirk.spritefont", style);


	style.position.x = 50.0f;
	style.position.y = 300.f;
	int rankings = 3;
	m_rankingScoreIndecOne = GUIHandler::get().addGUIText("#" + std::to_string(rankings--), L"squirk.spritefont", style);
	style.position.y -= 50.0f;
	m_rankingScoreIndecTwo = GUIHandler::get().addGUIText("#" + std::to_string(rankings--), L"squirk.spritefont", style);
	style.position.y -= 50.0f;
	m_rankingScoreIndecThree = GUIHandler::get().addGUIText("#" + std::to_string(rankings--), L"squirk.spritefont", style);
	hideScore();

	GUIHandler::get().setVisible(m_backToLobbyIndex, false);
}

void SceneManager::updateScene(const float &dt)
{
	//checking = PacketHandler::get().getStarted();
	//std::cout << checking << std::endl;
	if (m_swapScene && !m_loadNextSceneWhenReady)
	{

		m_nextScene = new Scene();
		std::thread sceneLoaderThread;
		switch (m_nextSceneEnum)
		{
		case ScenesEnum::LOBBY:

			sceneLoaderThread = std::thread(Scene::loadLobby, m_nextScene, m_nextSceneReady);
			sceneLoaderThread.detach();
			enableMovement();
			//disableMovement();
			//Reset game variables that are needed here
			//GUIHandler::get().setVisible(m_singleplayerIndex, true);
			//GUIHandler::get().setVisible(m_hostGameIndex, true);
			//GUIHandler::get().setVisible(m_joinGameIndex, true);
			//GUIHandler::get().setVisible(m_exitIndex, true);
			GUIHandler::get().setVisible(m_multiPlayerIndexTwo, true);
			GUIHandler::get().setVisible(m_multiPlayerIndexThree, true);
			GUIHandler::get().setVisible(m_multiPlayerIndexFour, true);
			Engine::get().getPlayerPtr()->setScore(0);
			hideScore();
			m_gameStarted = false;
			m_loadNextSceneWhenReady = true; //Tell scene manager to switch to the next scene as soon as the next scene finished loading.
			m_camera->endSceneCamera = false;
			GUIHandler::get().setInMenu(false);
			break;
		case ScenesEnum::START:
			sceneLoaderThread = std::thread(Scene::loadTestLevel, m_nextScene,m_nextSceneReady);
			sceneLoaderThread.detach();
			m_nextScene->hidescore = true;
			m_gameStarted = true;
			hideScore();
			enableMovement();
			m_loadNextSceneWhenReady = true; //Tell scene manager to switch to the next scene as soon as the next scene finished loading.
			m_camera->endSceneCamera = false;
			GUIHandler::get().setInMenu(false);
			break;
		case ScenesEnum::ARENA:
			sceneLoaderThread = std::thread(Scene::loadArena, m_nextScene, m_nextSceneReady);
			sceneLoaderThread.detach();
			m_gameStarted = true;
			m_loadNextSceneWhenReady = true; //Tell scene manager to switch to the next scene as soon as the next scene finished loading.
			m_camera->endSceneCamera = false;
			hideScore();
			GUIHandler::get().setInMenu(false);
			
			break;
		case ScenesEnum::MAINMENU:
			disableMovement();
			sceneLoaderThread = std::thread(Scene::loadMainMenu, m_nextScene, m_nextSceneReady);
			sceneLoaderThread.detach();
			m_loadNextSceneWhenReady = true; //Tell scene manager to switch to the next scene as soon as the next scene finished loading.
			m_gameStarted = false;
			m_camera->endSceneCamera = false; // if this is false the camera follows the player as usual
			hideScore();
			GUIHandler::get().setVisible(m_backToLobbyIndex, false);
			GUIHandler::get().setInMenu(true, m_singleplayerIndex);

			break;
		case ScenesEnum::ENDSCENE:
			sceneLoaderThread = std::thread(Scene::loadEndScene, m_nextScene, m_nextSceneReady);
			sceneLoaderThread.detach();
			m_loadNextSceneWhenReady = true; //Tell scene manager to switch to the next scene as soon as the next scene finished loading.
			disableMovement();
			GUIHandler::get().setVisible(m_singleplayerIndex, false);
			GUIHandler::get().setVisible(m_hostGameIndex, false);
			GUIHandler::get().setVisible(m_joinGameIndex, false);
			GUIHandler::get().setVisible(m_exitIndex, true);
			GUIHandler::get().setVisible(m_backToLobbyIndex, true);
			GUIHandler::get().setInMenu(true, m_singleplayerIndex);
			showScore();
			m_camera->endSceneCamera = true; // If this is true the camera no longer updates and have a fixed position in this scene
			break;
		default:
			break;
		}
		
	}
	swapScenes();
	if (PacketHandler::get().getServerReady() && !PacketHandler::get().gameStarted)
	{
		m_nextSceneEnum = ScenesEnum::START;
		m_swapScene = true;
		PacketHandler::get().gameStarted = true;
	}
	
	m_currentScene->updateScene(dt);
}

void SceneManager::inputUpdate(InputData& inputData)
{
	Engine::get().setUseBossEntityMap(true);
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
			std::thread sceneLoaderThread = std::thread(Scene::loadScene, m_nextScene, "levelMeshTest", m_nextSceneReady);
			sceneLoaderThread.detach();

			m_gameStarted = false;
			m_loadNextSceneWhenReady = true; //Tell scene manager to switch to the next scene as soon as the next scene finished loading.
		}
		else if (inputData.actionData[i] == LOAD_TEST_SCENE)
		{
			m_nextScene = m_bossScene;//new Scene();
			//std::thread sceneLoaderThread = std::thread(Scene::loadScene, m_nextScene, "levelMeshTest", m_nextSceneReady);
			
			*m_nextSceneReady = true;
			m_gameStarted = true;
			Engine::get().setUseBossEntityMap(false);
			m_loadNextSceneWhenReady = true; //Tell scene manager to switch to the next scene as soon as the next scene finished loading.
		}

		else if (inputData.actionData[i] == READY_UP)
		{
			
				std::cout << "sending rdy package" << std::endl;
				PacketHandler::get().sendReady();


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
			//Send activation message to server
			PacketHandler::get().sendTrapData(static_cast<PushTrapComponent*>(physicsData.pointer)->getParentEntityIdentifier());


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
				//Send activation message to server
				PacketHandler::get().sendTrapData(static_cast<BarrelTriggerComponent*>(physicsData.pointer)->getParentEntityIdentifier());

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

void SceneManager::setScorePtr(std::vector<std::pair<int, int>>* scores)
{
	m_scores = scores;
}

std::vector<std::pair<int, int>>* SceneManager::getScorePtr()
{
	return m_scores;
}

void SceneManager::swapScenes()
{
	m_swapScene = false;
	if (*m_nextSceneReady == true && m_loadNextSceneWhenReady)
	{
		*m_nextSceneReady = false;
		m_loadNextSceneWhenReady = false;
		

		// Swap
		if (m_currentScene != m_bossScene) //Not moving from boss scene
		{
			Physics::get().Detach(m_currentScene, false, true);

			//Reset boss
			if (m_currentScene->m_boss)
				m_currentScene->m_boss->Detach(m_currentScene);
			delete m_currentScene;

		}
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
		ccc->initController(Engine::get().getPlayerPtr()->getPlayerEntity(), 1.75f, 0.5f, "human");
		ccc->setPosition(m_currentScene->getEntryPosition());

		m_currentScene->onSceneLoaded();
	}
}

void SceneManager::update(GUIUpdateType type, GUIElement* guiElement)
{
	GUIButton* button = dynamic_cast<GUIButton*>(guiElement);
	if (type == GUIUpdateType::HOVER_ENTER)
	{
		if (guiElement->m_index == m_exitIndex)
		{
			button->setTexture(L"exitBtnHover.png");
		}
		if (guiElement->m_index == m_singleplayerIndex)
		{
			button->setTexture(L"singleplayerBtnHover.png");
		}
		if (guiElement->m_index == m_joinGameIndex)
		{
			button->setTexture(L"joinBtnHover.png");
		}
		if (guiElement->m_index == m_hostGameIndex)
		{
			button->setTexture(L"hostBtnHover.png");
		}
		if (guiElement->m_index == m_backToLobbyIndex)
		{
			button->setTexture(L"backToLobbyHover.png");
		}
	}

	if (type == GUIUpdateType::HOVER_EXIT)
	{
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
		if (guiElement->m_index == m_backToLobbyIndex)
		{
			button->setTexture(L"backToLobby.png");
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
			Engine::get().setClient(true);
			//m_gameStarted = true;
			GUIHandler::get().setVisible(m_singleplayerIndex, false);
			GUIHandler::get().setVisible(m_hostGameIndex, false);
			GUIHandler::get().setVisible(m_joinGameIndex, false);
			GUIHandler::get().setVisible(m_exitIndex, false);
			GUIHandler::get().setVisible(m_multiPlayerIndexTwo, false);
			GUIHandler::get().setVisible(m_multiPlayerIndexThree, false);
			GUIHandler::get().setVisible(m_multiPlayerIndexFour, false);
			m_nextSceneEnum = ScenesEnum::LOBBY;
			m_swapScene = true;
		}
		if (guiElement->m_index == m_hostGameIndex)
		{
			//do stuff
			Engine::get().setHost(true);
			//m_gameStarted = true;
			GUIHandler::get().setVisible(m_singleplayerIndex, false);
			GUIHandler::get().setVisible(m_hostGameIndex, false);
			GUIHandler::get().setVisible(m_joinGameIndex, false);
			GUIHandler::get().setVisible(m_exitIndex, false);
			m_nextSceneEnum = ScenesEnum::LOBBY;
			m_swapScene = true;

		}
		if (guiElement->m_index == m_exitIndex)
		{
			endGame = true;
		}
		if (guiElement->m_index == m_backToLobbyIndex)
		{
			GUIHandler::get().setVisible(m_singleplayerIndex, true);
			GUIHandler::get().setVisible(m_hostGameIndex, true);
			GUIHandler::get().setVisible(m_joinGameIndex, true);
			GUIHandler::get().setVisible(m_exitIndex, true);
			m_nextSceneEnum = ScenesEnum::MAINMENU;
			m_swapScene = true;
		}
		if (guiElement->m_index == m_multiPlayerIndexTwo)
		{
			Server::get().setNrOfPlayers(2);
			GUIHandler::get().setVisible(m_multiPlayerIndexTwo, false);
			GUIHandler::get().setVisible(m_multiPlayerIndexThree, false);
			GUIHandler::get().setVisible(m_multiPlayerIndexFour, false);
			std::cout << "Nr of players set to 2" << std::endl;
		}
		if (guiElement->m_index == m_multiPlayerIndexThree)
		{
			GUIHandler::get().setVisible(m_multiPlayerIndexTwo, false);
			GUIHandler::get().setVisible(m_multiPlayerIndexThree, false);
			GUIHandler::get().setVisible(m_multiPlayerIndexFour, false);
			Server::get().setNrOfPlayers(3);
		}
		if (guiElement->m_index == m_multiPlayerIndexFour)
		{
			GUIHandler::get().setVisible(m_multiPlayerIndexTwo, false);
			GUIHandler::get().setVisible(m_multiPlayerIndexThree, false);
			GUIHandler::get().setVisible(m_multiPlayerIndexFour, false);
			std::cout << "Nr of players set to 1" << std::endl;
			Server::get().setNrOfPlayers(1);
		}


		static_cast<CharacterControllerComponent*>(Engine::get().getPlayerPtr()->getPlayerEntity()->getComponent("CCC"))->setPosition(m_currentScene->getEntryPosition());
		for (int i = 0; i < 3; i++)
		{
			Engine::get().getServerPlayers()->at(i)->getPlayerEntity()->setPosition(m_currentScene->getEntryPosition());


		}
	}



}
void SceneManager::hideScore()
{
	GUIHandler::get().setVisible(m_highScoreLabelIndex, false);
	GUIHandler::get().setVisible(m_playerOneScoreIndex, false);
	GUIHandler::get().setVisible(m_playerTwoScoreIndex, false);
	GUIHandler::get().setVisible(m_playerThreeScoreIndex, false);

	GUIHandler::get().setVisible(m_rankingScoreIndecOne, false);
	GUIHandler::get().setVisible(m_rankingScoreIndecTwo, false);
	GUIHandler::get().setVisible(m_rankingScoreIndecThree, false);
}

void SceneManager::showScore()
{

	GUIHandler::get().setVisible(m_highScoreLabelIndex, true);
	GUIHandler::get().setVisible(m_playerOneScoreIndex, true);
	GUIHandler::get().setVisible(m_playerTwoScoreIndex, true);
	GUIHandler::get().setVisible(m_playerThreeScoreIndex, true);

	GUIHandler::get().setVisible(m_rankingScoreIndecOne, true);
	GUIHandler::get().setVisible(m_rankingScoreIndecTwo, true);
	GUIHandler::get().setVisible(m_rankingScoreIndecThree, true);
}

void SceneManager::startBarrelDrop(std::string entity)
{
	Entity *temp = Engine::get().getEntityMap()->at(entity);
	std::vector<Component*> tempVec;
	temp->getComponentsOfType(tempVec, ComponentType::TRIGGER);

	for (int i = 0; i < tempVec.size(); i++)
	{
		BarrelTriggerComponent* test = dynamic_cast<BarrelTriggerComponent*>(tempVec[i]);
		if (test != nullptr)
		{
			m_currentScene->addBarrelDrop(Vector3(-30, 50, 130));
			test->m_triggerTimer.restart();
			m_currentScene->addedBarrel = true;
		}
	}
	
}



