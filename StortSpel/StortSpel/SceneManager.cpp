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

	//join button
	btnStyle.position = Vector2(140, 550);
	btnStyle.scale = Vector2(1, 1);
	m_joinGameIndex = GUIHandler::get().addGUIButton(L"joinBtn.png", btnStyle);

	GUIButton* joinButton = dynamic_cast<GUIButton*>(GUIHandler::get().getElementMap()->at(m_joinGameIndex));
	joinButton->Attach(this);
	joinButton->setPrevMenuButton(startButton);
	startButton->setNextMenuButton(joinButton);

	//Host Button
	btnStyle.position = Vector2(320, 550);
	btnStyle.scale = Vector2(1, 1);
	m_hostGameIndex = GUIHandler::get().addGUIButton(L"hostBtn.png", btnStyle);

	GUIButton* hostButton = dynamic_cast<GUIButton*>(GUIHandler::get().getElementMap()->at(m_hostGameIndex));
	hostButton->Attach(this);
	hostButton->setPrevMenuButton(joinButton);
	joinButton->setNextMenuButton(hostButton);

	//Exit button
	btnStyle.position = Vector2(1700, 850);
	btnStyle.scale = Vector2(1, 1);
	m_exitIndex = GUIHandler::get().addGUIButton(L"exitBtn.png", btnStyle);

	GUIButton* exitButton = dynamic_cast<GUIButton*>(GUIHandler::get().getElementMap()->at(m_exitIndex));

	//Back to lobby button
	btnStyle.position = Vector2(140, 800);
	btnStyle.scale = Vector2(1.5, 1.5);
	m_backToLobbyIndex = GUIHandler::get().addGUIButton(L"backToLobby.png", btnStyle);

	GUIButton* backToLobbyButton = dynamic_cast<GUIButton*>(GUIHandler::get().getElementMap()->at(m_backToLobbyIndex));
	backToLobbyButton->Attach(this);
	
	//Settings button
	btnStyle.position = Vector2(140, 700);
	btnStyle.scale = Vector2(1, 1);
	m_settingsIndex = GUIHandler::get().addGUIButton(L"settingsButton.png", btnStyle);

	GUIButton* settingsButton = dynamic_cast<GUIButton*>(GUIHandler::get().getElementMap()->at(m_settingsIndex));
	settingsButton->Attach(this);
	
	//fov button
	btnStyle.position = Vector2(940, 500);
	btnStyle.scale = Vector2(1, 1);
	m_setFovIncreaseIndex = GUIHandler::get().addGUIButton(L"fovIncrease.png", btnStyle);

	GUIButton* fovIncreaseBtn = dynamic_cast<GUIButton*>(GUIHandler::get().getElementMap()->at(m_setFovIncreaseIndex));
	fovIncreaseBtn->Attach(this);

	//fov button
	btnStyle.position = Vector2(840, 500);
	btnStyle.scale = Vector2(1, 1);
	m_setFovDecreaseIndex = GUIHandler::get().addGUIButton(L"fovDecrease.png", btnStyle);

	GUIButton* fovDecreaseBtn = dynamic_cast<GUIButton*>(GUIHandler::get().getElementMap()->at(m_setFovDecreaseIndex));
	fovDecreaseBtn->Attach(this);
	
	//sense Increase button
	btnStyle.position = Vector2(940, 400);
	btnStyle.scale = Vector2(1, 1);
	m_volumeIncreaseIndex = GUIHandler::get().addGUIButton(L"volumeRight.png", btnStyle);

	GUIButton* volumeIncrease = dynamic_cast<GUIButton*>(GUIHandler::get().getElementMap()->at(m_volumeIncreaseIndex));
	volumeIncrease->Attach(this);

	//sense Increase button
	btnStyle.position = Vector2(840, 400);
	btnStyle.scale = Vector2(1, 1);
	m_volumeDecreaseIndex = GUIHandler::get().addGUIButton(L"volumeLeft.png", btnStyle);

	GUIButton* volumeDecrease = dynamic_cast<GUIButton*>(GUIHandler::get().getElementMap()->at(m_volumeDecreaseIndex));
	volumeDecrease->Attach(this);

	//sense Increase button
	btnStyle.position = Vector2(940, 300);
	btnStyle.scale = Vector2(1, 1);
	m_senseIncreaseIndex = GUIHandler::get().addGUIButton(L"senseIncrease.png", btnStyle);

	GUIButton* senseIncrease = dynamic_cast<GUIButton*>(GUIHandler::get().getElementMap()->at(m_senseIncreaseIndex));
	senseIncrease->Attach(this);

	//sense decrease button
	btnStyle.position = Vector2(840, 300);
	btnStyle.scale = Vector2(1, 1);
	m_senseDecreaseIndex = GUIHandler::get().addGUIButton(L"senseDecrease.png", btnStyle);

	GUIButton* senseDecrease = dynamic_cast<GUIButton*>(GUIHandler::get().getElementMap()->at(m_senseDecreaseIndex));
	senseDecrease->Attach(this);

	//resume button
	btnStyle.position = Vector2(740, 800);
	btnStyle.scale = Vector2(1, 1);
	m_resumeBtnIndex = GUIHandler::get().addGUIButton(L"resumeBtn.png", btnStyle);

	GUIButton* resumeBtn = dynamic_cast<GUIButton*>(GUIHandler::get().getElementMap()->at(m_resumeBtnIndex));
	resumeBtn->Attach(this);
	//assdasdasdfsdfsjdfjsdfjsdfjsdfjsdfj
	btnStyle.position = Vector2(930, 600);
	btnStyle.scale = Vector2(0.2, 0.2);
	m_fullscreenIndex = GUIHandler::get().addGUIButton(L"uncheckedBox.png", btnStyle);

	GUIButton* fullscreenBtn = dynamic_cast<GUIButton*>(GUIHandler::get().getElementMap()->at(m_fullscreenIndex));
	fullscreenBtn->Attach(this);




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

	m_camera = Engine::get().getCameraPtr();
	setScorePtr(m_currentScene->getScores());
	
	style.position.y = 120.f;
	style.scale = { 0.5f };
	m_highScoreLabelIndex = GUIHandler::get().addGUIText("High Score", L"squirk.spritefont", style);
	style.position.x = 160.f;
	style.position.y = 300.f;

	style.color = Colors::Yellow;
	m_playerOneScoreIndex = GUIHandler::get().addGUIText(std::to_string(m_scores->at(0).first), L"squirk.spritefont", style);
	style.position.y -= 50.0f;
	m_playerTwoScoreIndex = GUIHandler::get().addGUIText(std::to_string(m_scores->at(1).first), L"squirk.spritefont", style);
	style.position.y -= 50.0f;
	m_playerThreeScoreIndex = GUIHandler::get().addGUIText(std::to_string(m_scores->at(2).first), L"squirk.spritefont", style);

	style.position.x = 50.0f;
	style.position.y = 300.f;
	int rankings = 3;
	m_rankingScoreIndecOne = GUIHandler::get().addGUIText("#" + std::to_string(rankings--), L"squirk.spritefont", style);
	style.position.y -= 50.0f;
	m_rankingScoreIndecTwo = GUIHandler::get().addGUIText("#" + std::to_string(rankings--), L"squirk.spritefont", style);
	style.position.y -= 50.0f;
	m_rankingScoreIndecThree = GUIHandler::get().addGUIText("#" + std::to_string(rankings--), L"squirk.spritefont", style);

	style.position.x = 1100.0f;
	style.position.y = 300.0f;
	m_sensitivityIndex = GUIHandler::get().addGUIText(std::to_string(m_cameraSense), L"squirk.spritefont", style);
	
	style.position.x = 1100.0f;
	style.position.y = 400.0f;
	m_volumeAmountIndex = GUIHandler::get().addGUIText(std::to_string(m_volumeAmount), L"squirk.spritefont", style);

	//240, 400
	style.position.x = 1100.0f;
	style.position.y = 500.0f;
	m_fovIndex = GUIHandler::get().addGUIText(std::to_string(m_camera->fovAmount), L"squirk.spritefont", style);
	
	//
	style.position.x = 590.0f;
	style.position.y = 300.0f;
	m_senseTextIndex = GUIHandler::get().addGUIText("Sensitivity", L"squirk.spritefont", style);

	//400
	style.position.x = 650.0f;
	style.position.y = 400.0f;
	m_volumeTextIndex = GUIHandler::get().addGUIText("Volume", L"squirk.spritefont", style);

	//240
	style.position.x = 700.0f;
	style.position.y = 500.0f;
	m_fovText = GUIHandler::get().addGUIText("FOV", L"squirk.spritefont", style);

	//240
	style.position.x = 800.0f;
	style.position.y = 200.0f;
	m_settingsText = GUIHandler::get().addGUIText("Settings", L"squirk.spritefont", style);

	//24000
	style.position.x = 670.0f;
	style.position.y = 600.0f;
	m_fullscreenText = GUIHandler::get().addGUIText("FullScreen", L"squirk.spritefont", style);

	
	hideScore();
	GUIHandler::get().setVisible(m_volumeDecreaseIndex, false);
	GUIHandler::get().setVisible(m_volumeIncreaseIndex, false);
	GUIHandler::get().setVisible(m_setFovDecreaseIndex, false);
	GUIHandler::get().setVisible(m_setFovIncreaseIndex, false);
	GUIHandler::get().setVisible(m_sensitivityIndex, false);
	GUIHandler::get().setVisible(m_volumeAmountIndex, false);
	GUIHandler::get().setVisible(m_backToLobbyIndex, false);
	GUIHandler::get().setVisible(m_fovIndex, false);
	GUIHandler::get().setVisible(m_fovText, false);
	GUIHandler::get().setVisible(m_senseTextIndex, false);
	GUIHandler::get().setVisible(m_volumeTextIndex, false);
	GUIHandler::get().setVisible(m_senseIncreaseIndex, false);
	GUIHandler::get().setVisible(m_senseDecreaseIndex, false);
	GUIHandler::get().setVisible(m_settingsText, false); 
	GUIHandler::get().setVisible(m_resumeBtnIndex, false);
	GUIHandler::get().setVisible(m_fullscreenIndex, false);
	GUIHandler::get().setVisible(m_fullscreenText, false);



}

void SceneManager::updateScene(const float &dt)
{
	m_volumeAmount = AudioHandler::get().getVolumeAmount();
	m_cameraSense = m_camera->getSensitivity();
	GUIHandler::get().changeGUIText(m_sensitivityIndex, std::to_string(m_cameraSense));
	GUIHandler::get().changeGUIText(m_volumeAmountIndex, std::to_string(m_volumeAmount));
	GUIHandler::get().changeGUIText(m_fovIndex, std::to_string(m_camera->fovAmount));
	
	if (m_swapScene && !m_loadNextSceneWhenReady)
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
			GUIHandler::get().setVisible(m_senseDecreaseIndex, false);
			GUIHandler::get().setVisible(m_senseIncreaseIndex, false);
			GUIHandler::get().setVisible(m_settingsIndex, true);
			GUIHandler::get().setVisible(m_sensitivityIndex, false);
			GUIHandler::get().setVisible(m_volumeDecreaseIndex, false);
			GUIHandler::get().setVisible(m_volumeIncreaseIndex, false);
			GUIHandler::get().setVisible(m_setFovDecreaseIndex, false);
			GUIHandler::get().setVisible(m_setFovIncreaseIndex, false);
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
			GUIHandler::get().setVisible(m_settingsIndex, false);
			GUIHandler::get().setVisible(m_exitIndex, true);
			GUIHandler::get().setVisible(m_backToLobbyIndex, true);
			GUIHandler::get().setVisible(m_senseDecreaseIndex, false);
			GUIHandler::get().setVisible(m_senseIncreaseIndex, false);
			GUIHandler::get().setVisible(m_sensitivityIndex, false);
			GUIHandler::get().setVisible(m_volumeAmountIndex, false);
			GUIHandler::get().setVisible(m_volumeDecreaseIndex, false);
			GUIHandler::get().setVisible(m_volumeIncreaseIndex, false);
			GUIHandler::get().setVisible(m_setFovDecreaseIndex, false);
			GUIHandler::get().setVisible(m_setFovIncreaseIndex, false);
			GUIHandler::get().setVisible(m_fovIndex, false);
			GUIHandler::get().setInMenu(true, m_singleplayerIndex);
			showScore();
			m_camera->endSceneCamera = true; // If this is true the camera no longer updates and have a fixed position in this scene
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
			std::thread sceneLoaderThread = std::thread(Scene::loadScene, m_nextScene, "levelMeshTest", m_nextSceneReady);
			sceneLoaderThread.detach();

			m_gameStarted = false;
			m_loadNextSceneWhenReady = true; //Tell scene manager to switch to the next scene as soon as the next scene finished loading.
		}
		else if (inputData.actionData[i] == LOAD_TEST_SCENE)
		{
			m_nextScene = new Scene();
			//std::thread sceneLoaderThread = std::thread(Scene::loadScene, m_nextScene, "levelMeshTest", m_nextSceneReady);
			std::thread sceneLoaderThread = std::thread(Scene::loadBossTest, m_nextScene, m_nextSceneReady);
			sceneLoaderThread.detach();

			m_gameStarted = true;
			m_loadNextSceneWhenReady = true; //Tell scene manager to switch to the next scene as soon as the next scene finished loading.
		}
		else if (inputData.actionData[i] == MENU)
		{
			disableMovement();
			GUIHandler::get().setVisible(m_settingsIndex, true);
			GUIHandler::get().setVisible(m_exitIndex, true);
			GUIHandler::get().setVisible(m_resumeBtnIndex, true);
			GUIHandler::get().setVisible(m_backToLobbyIndex, true);


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

void SceneManager::setScorePtr(std::vector<std::pair<int, std::string>>* scores)
{
	m_scores = scores;
}

std::vector<std::pair<int, std::string>>* SceneManager::getScorePtr()
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
		if (guiElement->m_index == m_settingsIndex)
		{
			button->setTexture(L"settingsButtonHover.png");
		}
		if (guiElement->m_index == m_senseDecreaseIndex)
		{
			button->setTexture(L"senseDecreaseHover.png");
		}
		if (guiElement->m_index == m_senseIncreaseIndex)
		{
			button->setTexture(L"senseIncreaseHover.png");
		}
		if (guiElement->m_index == m_volumeIncreaseIndex)
		{
			button->setTexture(L"volumeRightHover.png");
		}
		if (guiElement->m_index == m_volumeDecreaseIndex)
		{
			button->setTexture(L"volumeLeftHover.png");
		}
		if (guiElement->m_index == m_setFovDecreaseIndex)
		{
			button->setTexture(L"fovDecreaseHover.png");
		}
		if (guiElement->m_index == m_setFovIncreaseIndex)
		{
			button->setTexture(L"fovIncreaseHover.png");
		}
		if (guiElement->m_index == m_resumeBtnIndex)
		{
			button->setTexture(L"resumeBtnHover.png");
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
		if (guiElement->m_index == m_settingsIndex)
		{
			button->setTexture(L"settingsButton.png");
		}
		if (guiElement->m_index == m_senseDecreaseIndex)
		{
			button->setTexture(L"senseDecrease.png"); 
		}
		if (guiElement->m_index == m_senseIncreaseIndex)
		{
			button->setTexture(L"senseIncrease.png");
		}
		if (guiElement->m_index == m_volumeIncreaseIndex)
		{
			button->setTexture(L"volumeRight.png");
		}
		if (guiElement->m_index == m_volumeDecreaseIndex)
		{
			button->setTexture(L"volumeLeft.png");
		}
		if (guiElement->m_index == m_setFovIncreaseIndex)
		{
			button->setTexture(L"fovIncrease.png");
		}
		if (guiElement->m_index == m_setFovDecreaseIndex)
		{
			button->setTexture(L"fovDecrease.png");
		}
		if (guiElement->m_index == m_resumeBtnIndex)
		{
			button->setTexture(L"resumeBtn.png");
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
			GUIHandler::get().setVisible(m_settingsIndex, false);
			GUIHandler::get().setVisible(m_senseDecreaseIndex, false);
			GUIHandler::get().setVisible(m_senseIncreaseIndex, false);
			GUIHandler::get().setVisible(m_volumeDecreaseIndex, false);
			GUIHandler::get().setVisible(m_volumeIncreaseIndex, false);
			GUIHandler::get().setVisible(m_setFovDecreaseIndex, false);
			GUIHandler::get().setVisible(m_setFovIncreaseIndex, false);
			GUIHandler::get().setVisible(m_fovIndex, false);


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
		}
		if (guiElement->m_index == m_backToLobbyIndex)
		{
			GUIHandler::get().setVisible(m_singleplayerIndex, true);
			GUIHandler::get().setVisible(m_hostGameIndex, true);
			GUIHandler::get().setVisible(m_joinGameIndex, true);
			GUIHandler::get().setVisible(m_exitIndex, true);
			GUIHandler::get().setVisible(m_settingsIndex, true);
			GUIHandler::get().setVisible(m_sensitivityIndex, false);
			GUIHandler::get().setVisible(m_volumeAmountIndex, false);
			GUIHandler::get().setVisible(m_volumeDecreaseIndex, false);
			GUIHandler::get().setVisible(m_volumeIncreaseIndex, false);
			GUIHandler::get().setVisible(m_setFovDecreaseIndex, false);
			GUIHandler::get().setVisible(m_setFovIncreaseIndex, false);
			GUIHandler::get().setVisible(m_fovIndex, false);
			GUIHandler::get().setVisible(m_fovText, false);
			GUIHandler::get().setVisible(m_senseTextIndex, false);
			GUIHandler::get().setVisible(m_volumeTextIndex, false);
			GUIHandler::get().setVisible(m_senseIncreaseIndex, false);
			GUIHandler::get().setVisible(m_senseDecreaseIndex, false);
			GUIHandler::get().setVisible(m_settingsText, false);
			GUIHandler::get().setVisible(m_resumeBtnIndex, false);
			GUIHandler::get().setVisible(m_fullscreenText, false);


			m_nextSceneEnum = ScenesEnum::MAINMENU;
			m_swapScene = true;
		}
		if (guiElement->m_index == m_settingsIndex)
		{
			GUIHandler::get().setVisible(m_singleplayerIndex, false);
			GUIHandler::get().setVisible(m_hostGameIndex, false);
			GUIHandler::get().setVisible(m_joinGameIndex, false);
			GUIHandler::get().setVisible(m_exitIndex, true);
			GUIHandler::get().setVisible(m_settingsIndex, false);
			GUIHandler::get().setVisible(m_backToLobbyIndex, true);
			GUIHandler::get().setVisible(m_senseDecreaseIndex, true);
			GUIHandler::get().setVisible(m_volumeDecreaseIndex, true);
			GUIHandler::get().setVisible(m_volumeIncreaseIndex, true);
			GUIHandler::get().setVisible(m_senseIncreaseIndex, true);
			GUIHandler::get().setVisible(m_sensitivityIndex, true);
			GUIHandler::get().setVisible(m_volumeAmountIndex, true);
			GUIHandler::get().setVisible(m_setFovDecreaseIndex, true);
			GUIHandler::get().setVisible(m_setFovIncreaseIndex, true);
			GUIHandler::get().setVisible(m_fovIndex, true);
			GUIHandler::get().setVisible(m_fovText, true);
			GUIHandler::get().setVisible(m_senseTextIndex, true);
			GUIHandler::get().setVisible(m_volumeTextIndex, true);
			GUIHandler::get().setVisible(m_settingsText, true);
			GUIHandler::get().setVisible(m_fullscreenIndex, true);
			GUIHandler::get().setVisible(m_fullscreenText, true);

		}
		if (guiElement->m_index == m_senseIncreaseIndex)
		{
			m_camera->increaseSensitivity();
		}
		if (guiElement->m_index == m_senseDecreaseIndex)
		{
			m_camera->decreaseSensitivity();
		}
		if (guiElement->m_index == m_volumeDecreaseIndex)
		{
			AudioHandler::get().decreaseVolume();
		}
		if (guiElement->m_index == m_volumeIncreaseIndex)
		{
			AudioHandler::get().increaseVolume();
		}
		if (guiElement->m_index == m_setFovIncreaseIndex)
		{
			m_camera->increaseFov();
		}
		if (guiElement->m_index == m_setFovDecreaseIndex)
		{
			m_camera->decreaseFov();
		}
		if (guiElement->m_index == m_resumeBtnIndex)
		{
			enableMovement();
			GUIHandler::get().setVisible(m_settingsText, false);
			GUIHandler::get().setVisible(m_exitIndex, false);
			GUIHandler::get().setVisible(m_senseDecreaseIndex, false);
			GUIHandler::get().setVisible(m_volumeDecreaseIndex, false);
			GUIHandler::get().setVisible(m_volumeIncreaseIndex, false);
			GUIHandler::get().setVisible(m_senseIncreaseIndex, false);
			GUIHandler::get().setVisible(m_sensitivityIndex, false);
			GUIHandler::get().setVisible(m_volumeAmountIndex, false);
			GUIHandler::get().setVisible(m_setFovDecreaseIndex, false);
			GUIHandler::get().setVisible(m_setFovIncreaseIndex, false);
			GUIHandler::get().setVisible(m_fovIndex, false);
			GUIHandler::get().setVisible(m_fovText, false);
			GUIHandler::get().setVisible(m_senseTextIndex, false);
			GUIHandler::get().setVisible(m_volumeTextIndex, false);
			GUIHandler::get().setVisible(m_resumeBtnIndex, false);
			GUIHandler::get().setVisible(m_settingsIndex, false);
			GUIHandler::get().setVisible(m_backToLobbyIndex, false);
		}
		if (guiElement->m_index == m_fullscreenIndex)
		{
			// do stuff
			
			
			if (checked == false)
			{
				button->setTexture(L"uncheckedbox.png");
				
			}
			else
			{
				button->setTexture(L"checkedbox.png");
			}
			checked = !checked;
			
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

