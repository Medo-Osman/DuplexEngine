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

void SceneManager::initalize(Input* input)
{
	//ImGui::NewFrame();
	//ImGui::Begin("My First Tool", &my_test_active, ImGuiWindowFlags_MenuBar);
	//if (ImGui::BeginMenuBar())
	//{
	//	if (ImGui::BeginMenu("File"))
	//	{
	//		if (ImGui::MenuItem("Open..", "Ctrl+O")) { /* Do stuff */ }
	//		if (ImGui::MenuItem("Save", "Ctrl+S")) { /* Do stuff */ }
	//		if (ImGui::MenuItem("Close", "Ctrl+W")) { my_test_active = false; }
	//		ImGui::EndMenu();
	//	}
	//	ImGui::EndMenuBar();
	//}
	m_inputPtr = input;
	m_camera = Engine::get().getCameraPtr(); // Neeeds to be before uiMenuInitialize()
	uiMenuInitialize();

	// Start Scene
	m_currentScene = new Scene();
	Scene::loadMainMenu(m_currentScene, m_nextSceneReady);
	m_currectSceneEnum = ScenesEnum::MAINMENU;
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
	Engine::get().setQuadTreePtr(m_currentScene->getQuadTreePtr());

	setScorePtr(m_currentScene->getScores());

	GUITextStyle textStyle;

	textStyle.position.x = 140.f;
	textStyle.position.y = 100.f;
	m_highScoreLabelIndex = GUIHandler::get().addGUIText("High Score", L"concert_one_60.spritefont", textStyle);

	textStyle.color = Colors::Yellow;
	textStyle.position.y = 300.f;
	m_playerOneScoreIndex = GUIHandler::get().addGUIText(std::to_string(m_scores->at(0).first), L"concert_one_32.spritefont", textStyle);
	
	textStyle.position.y -= 50.0f;
	m_playerTwoScoreIndex = GUIHandler::get().addGUIText(std::to_string(m_scores->at(1).first), L"concert_one_32.spritefont", textStyle);
	
	textStyle.position.y -= 50.0f;
	m_playerThreeScoreIndex = GUIHandler::get().addGUIText(std::to_string(m_scores->at(2).first), L"concert_one_32.spritefont", textStyle);

	textStyle.position.x = 50.0f;
	textStyle.position.y = 300.f;
	int rankings = 3;
	m_rankingScoreIndecOne = GUIHandler::get().addGUIText("#" + std::to_string(rankings--), L"concert_one_32.spritefont", textStyle);
	textStyle.position.y -= 50.0f;
	m_rankingScoreIndecTwo = GUIHandler::get().addGUIText("#" + std::to_string(rankings--), L"concert_one_32.spritefont", textStyle);
	textStyle.position.y -= 50.0f;
	m_rankingScoreIndecThree = GUIHandler::get().addGUIText("#" + std::to_string(rankings--), L"concert_one_32.spritefont", textStyle);

	hideScore();
	hideSettingsMenu();
	hidePauseMenu();
	showMainMenu();
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
			hideSettingsMenu();
			Engine::get().getPlayerPtr()->setScore(0);
			hideScore();
			showMainMenu();
			m_gameStarted = false;
			m_loadNextSceneWhenReady = true; //Tell scene manager to switch to the next scene as soon as the next scene finished loading.
			m_camera->endSceneCamera = false;
			break;
		case ScenesEnum::START:
			//sceneLoaderThread = std::thread(Scene::loadTestLevel, m_nextScene,m_nextSceneReady);
			sceneLoaderThread = std::thread(Scene::loadScene, m_nextScene, "Skyway_1", m_nextSceneReady);
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
			sceneLoaderThread = std::thread(Scene::loadBossTest, m_nextScene, m_nextSceneReady);
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
			m_camera->setRotation(Vector3(0.f)); // Reset camera rotation
			Engine::get().getPlayerPtr()->getPlayerEntity()->setRotationQuat(Quaternion());
			hideScore();
			GUIHandler::get().setVisible(m_backToLobbyIndex, false);
			
		
			GUIHandler::get().setInMenu(true, m_singleplayerIndex);
			showMainMenu();
			break;
		case ScenesEnum::ENDSCENE:
			sceneLoaderThread = std::thread(Scene::loadEndScene, m_nextScene, m_nextSceneReady);
			sceneLoaderThread.detach();
			m_loadNextSceneWhenReady = true; //Tell scene manager to switch to the next scene as soon as the next scene finished loading.
			disableMovement();
			
			hideSettingsMenu();
			hideMainMenu();

			GUIHandler::get().setVisible(m_backToLobbyIndex, true);
			GUIHandler::get().setInMenu(true, m_backToLobbyIndex);
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
	if (DEBUGMODE)
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
				std::thread sceneLoaderThread = std::thread(Scene::loadScene, m_nextScene, "Skyway_1", m_nextSceneReady);
				//std::thread sceneLoaderThread = std::thread(Scene::loadMaterialTest, m_nextScene, m_nextSceneReady);
				sceneLoaderThread.detach();
				//Scene::loadScene(m_nextScene, "levelMeshTest1", m_nextSceneReady);

				//Scene::loadLobby(m_nextScene, m_nextSceneReady);

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
			else if (inputData.actionData[i] == LOAD_ARENA)
			{
				m_nextScene = new Scene();
				//std::thread sceneLoaderThread = std::thread(Scene::loadScene, m_nextScene, "levelMeshTest", m_nextSceneReady);
				std::thread sceneLoaderThread = std::thread(Scene::loadBossTestPhaseTwo, m_nextScene, m_nextSceneReady);
				sceneLoaderThread.detach();

				m_gameStarted = true;
				m_loadNextSceneWhenReady = true; //Tell scene manager to switch to the next scene as soon as the next scene finished loading.
			}
			else if (inputData.actionData[i] == LOAD_EMPTY)
			{
				m_nextScene = new Scene();
				//std::thread sceneLoaderThread = std::thread(Scene::loadScene, m_nextScene, "levelMeshTest", m_nextSceneReady);
				std::thread sceneLoaderThread = std::thread(Scene::loadEndScene, m_nextScene, m_nextSceneReady);
				sceneLoaderThread.detach();

				m_loadNextSceneWhenReady = true; //Tell scene manager to switch to the next scene as soon as the next scene finished loading.
				disableMovement();

				hideSettingsMenu();
				hideMainMenu();

				GUIHandler::get().setVisible(m_backToLobbyIndex, true);
				GUIHandler::get().setInMenu(true, m_backToLobbyIndex);

				showScore();
				m_camera->endSceneCamera = true; // If this is true the camera no longer updates and have a fixed position in this scene

				m_gameStarted = true;
				m_loadNextSceneWhenReady = true; //Tell scene manager to switch to the next scene as soon as the next scene finished loading.
			}
			else if (inputData.actionData[i] == LOAD_ALMOST_EMPTY)
			{
				m_nextScene = new Scene();
				//std::thread sceneLoaderThread = std::thread(Scene::loadScene, m_nextScene, "levelMeshTest", m_nextSceneReady);
				std::thread sceneLoaderThread = std::thread(Scene::loadLobby, m_nextScene, m_nextSceneReady);
				sceneLoaderThread.detach();

				m_gameStarted = false;
				m_loadNextSceneWhenReady = true; //Tell scene manager to switch to the next scene as soon as the next scene finished loading.
			}

			else if (inputData.actionData[i] == MENU)
			{
				if (m_currectSceneEnum != ScenesEnum::MAINMENU && m_currectSceneEnum != ScenesEnum::ENDSCENE)
				{
					if (!m_inPause || m_inPauseSettings)
					{

						//showPauseMenu();

						// close Pause Settings
						if (m_inPauseSettings) // if back from pause settings, hide settings
						{
							hideSettingsMenu();

							m_inPauseSettings = false;
						}
						else
						{
							m_inputPtr->setCursor(true);
						}

						showPauseMenu();

						m_inPause = true;
					}
					else
					{
						enableMovement();
						m_inputPtr->setCursor(false);
						hidePauseMenu();
						m_inPause = false;
					}
				}
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

void SceneManager::setScorePtr(std::vector<std::pair<int, std::string>>* scores)
{
	m_scores = scores;
}

std::vector<std::pair<int, std::string>>* SceneManager::getScorePtr()
{
	return m_scores;
}

void SceneManager::inputIP()
{

	ImGui::Begin("Input IP", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar);
	ImGui::InputText("IP", charPtr,IM_ARRAYSIZE(charPtr));
	
	ImGui::End();
	std::string IP = std::string(charPtr, charPtr + sizeof(char)*256);
	
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

		m_currentScene->deactivateScene();

		// Swap
		delete m_currentScene;
		m_currentScene = m_nextScene;
		m_nextScene = nullptr;
		m_currectSceneEnum = m_nextSceneEnum;

		// Update currentScene in engine
		Engine::get().setEntitiesMapPtr(m_currentScene->getEntityMap());
		Engine::get().setLightComponentMapPtr(m_currentScene->getLightMap());
		Engine::get().setMeshComponentMapPtr(m_currentScene->getMeshComponentMap());
		Engine::get().getPlayerPtr()->reset3DMarker();

		m_currentScene->activateScene();
		Engine::get().setQuadTreePtr(m_currentScene->getQuadTreePtr());

		// Set as PhysicsObserver
		Physics::get().Attach(m_currentScene, false, true);
		Physics::get().changeScene(m_currentScene->getSceneID());
		CharacterControllerComponent* ccc = static_cast<CharacterControllerComponent*>(Engine::get().getPlayerPtr()->getPlayerEntity()->getComponent("CCC"));
		ccc->initController(Engine::get().getPlayerPtr()->getPlayerEntity(), PLAYER_CAPSULE_HEIGHT, PLAYER_CAPSULE_RADIUS, "human");
		ccc->setPosition(m_currentScene->getEntryPosition());

		m_currentScene->onSceneLoaded();
		ResourceHandler::get().checkResources();
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
		if (guiElement->m_index == m_tutorialIndex)
		{
			button->setTexture(L"tutorialBtnHover.png");
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
			button->setTexture(L"mainmenuBtnHover.png");
		}
		if (guiElement->m_index == m_settingsIndex)
		{
			button->setTexture(L"optionsBtnHover.png");
		}
		if (guiElement->m_index == m_senseDecreaseIndex)
		{
			button->setTexture(L"leftBtnHover.png");
		}
		if (guiElement->m_index == m_senseIncreaseIndex)
		{
			button->setTexture(L"rightBtnHover.png");
		}
		if (guiElement->m_index == m_volumeIncreaseIndex)
		{
			button->setTexture(L"rightBtnHover.png");
		}
		if (guiElement->m_index == m_volumeDecreaseIndex)
		{
			button->setTexture(L"leftBtnHover.png");
		}
		if (guiElement->m_index == m_setFovDecreaseIndex)
		{
			button->setTexture(L"leftBtnHover.png");
		}
		if (guiElement->m_index == m_setFovIncreaseIndex)
		{
			button->setTexture(L"rightBtnHover.png");
		}
		if (guiElement->m_index == m_resumeBtnIndex)
		{
			button->setTexture(L"resumeBtnHover.png");
		}
		if (guiElement->m_index == m_fullscreenIndex)
		{
			if (checked)
			{
				button->setTexture(L"checkedBoxHover.png");
			}
			else
			{
				button->setTexture(L"uncheckedBoxHover.png");
			}
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
		if (guiElement->m_index == m_tutorialIndex)
		{
			button->setTexture(L"tutorialBtn.png");
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
			button->setTexture(L"mainmenuBtn.png");
		}
		if (guiElement->m_index == m_settingsIndex)
		{
			button->setTexture(L"optionsBtn.png");
		}
		if (guiElement->m_index == m_senseDecreaseIndex)
		{
			button->setTexture(L"leftBtn.png");
		}
		if (guiElement->m_index == m_senseIncreaseIndex)
		{
			button->setTexture(L"rightBtn.png");
		}
		if (guiElement->m_index == m_volumeIncreaseIndex)
		{
			button->setTexture(L"rightBtn.png");
		}
		if (guiElement->m_index == m_volumeDecreaseIndex)
		{
			button->setTexture(L"leftBtn.png");
		}
		if (guiElement->m_index == m_setFovIncreaseIndex)
		{
			button->setTexture(L"rightBtn.png");
		}
		if (guiElement->m_index == m_setFovDecreaseIndex)
		{
			button->setTexture(L"leftBtn.png");
		}
		if (guiElement->m_index == m_resumeBtnIndex)
		{
			button->setTexture(L"resumeBtn.png");
		}
		if (guiElement->m_index == m_fullscreenIndex)
		{
			if (checked)
			{
				button->setTexture(L"checkedBox.png");
			}
			else
			{
				button->setTexture(L"uncheckedBox.png");
			}
		}
	}

	//Checks which button is being clicked
	if (type == GUIUpdateType::CLICKED)
	{

		if (guiElement->m_index == m_singleplayerIndex)
		{
			m_gameStarted = true;
			hideMainMenu();
			m_nextSceneEnum = ScenesEnum::START;
			m_swapScene = true;
		}
		if (guiElement->m_index == m_tutorialIndex)
		{
			// LADDA IN TUTORIAL NIVÅ HÄR!!!!!!
			//-----------------------------------------------------
			/*GUIHandler::get().setVisible(m_singleplayerIndex, false);
			GUIHandler::get().setVisible(m_tutorialIndex, false);
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
			GUIHandler::get().setVisible(m_fullscreenText, false);*/

			//m_nextSceneEnum = ScenesEnum::START; // GLÖM INTE ATT SKAPA OCH SÄTTA TUTORIAL ENUM!!!!!!
			//m_swapScene = true;
		}
		if (guiElement->m_index == m_joinGameIndex)
		{
			//do stuff
		}
		if (guiElement->m_index == m_hostGameIndex)
		{
			//do stuff
			if (showInputBar == false)
			{
				
			}
			else
			{
				

			}
			showInputBar = !showInputBar;
		}
		if (guiElement->m_index == m_exitIndex)
		{
			endGame = true;
		}
		if (guiElement->m_index == m_backToLobbyIndex)
		{
	
			hideSettingsMenu();
			hidePauseMenu();

			m_inPause = false;
			m_inPauseSettings = false;
			showMainMenu();
			m_nextSceneEnum = ScenesEnum::MAINMENU;
			m_swapScene = true;
		}
		if (guiElement->m_index == m_settingsIndex)
		{
			hideMainMenu();
			hidePauseMenu();
			showSettingsMenu();
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
			hideSettingsMenu();
			hidePauseMenu();

			enableMovement();
			m_inputPtr->setCursor(false);
			GUIHandler::get().setInMenu(false);
			m_inPause = false;
		}
		if (guiElement->m_index == m_fullscreenIndex)
		{
			// do stuff

			
			if (checked == false)
			{
				button->setTexture(L"checkedbox.png");
				Renderer::get().setFullScreen(true);
			}
			else
			{
				button->setTexture(L"uncheckedBox.png");
				Renderer::get().setFullScreen(false);
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

void SceneManager::hideMainMenu()
{
	GUIHandler::get().setVisible(m_singleplayerIndex, false);
	GUIHandler::get().setVisible(m_tutorialIndex, false);
	GUIHandler::get().setVisible(m_exitIndex, false);
	GUIHandler::get().setVisible(m_settingsIndex, false);
	GUIHandler::get().setVisible(m_senseDecreaseIndex, false);
	GUIHandler::get().setVisible(m_senseIncreaseIndex, false);
	GUIHandler::get().setVisible(m_volumeDecreaseIndex, false);
	GUIHandler::get().setVisible(m_volumeIncreaseIndex, false);
	GUIHandler::get().setVisible(m_setFovDecreaseIndex, false);
	GUIHandler::get().setVisible(m_setFovIncreaseIndex, false);
	GUIHandler::get().setVisible(m_fovIndex, false);
	GUIHandler::get().setVisible(m_fullscreenText, false);

	GUIHandler::get().setInMenu(false);
}

void SceneManager::showPauseMenu()
{
	GUIHandler::get().setInMenu(true, m_resumeBtnIndex);

	GUIButton* resumeBtn = dynamic_cast<GUIButton*>(GUIHandler::get().getElementMap()->at(m_resumeBtnIndex));
	GUIButton* backToLobbyBtn = dynamic_cast<GUIButton*>(GUIHandler::get().getElementMap()->at(m_backToLobbyIndex));
	GUIButton* settingsBtn = dynamic_cast<GUIButton*>(GUIHandler::get().getElementMap()->at(m_settingsIndex));

	disableMovement();
	GUIHandler::get().setVisible(m_pauseText, true);
	GUIHandler::get().setVisible(m_resumeBtnIndex, true);

	resumeBtn->setNextMenuButton(settingsBtn);
	resumeBtn->setPrevMenuButton(backToLobbyBtn);

	GUIHandler::get().setVisible(m_settingsIndex, true);
	settingsBtn->setNextMenuButton(backToLobbyBtn);
	settingsBtn->setPrevMenuButton(resumeBtn);

	GUIHandler::get().setVisible(m_backToLobbyIndex, true);
	backToLobbyBtn->setNextMenuButton(resumeBtn);
	backToLobbyBtn->setPrevMenuButton(settingsBtn);
}

void SceneManager::hidePauseMenu()
{
	GUIHandler::get().setVisible(m_singleplayerIndex, false);
	GUIHandler::get().setVisible(m_tutorialIndex, false);
	GUIHandler::get().setVisible(m_settingsIndex, false);
	GUIHandler::get().setVisible(m_exitIndex, false);
	GUIHandler::get().setVisible(m_pauseText, false);
	GUIHandler::get().setVisible(m_backToLobbyIndex, false);

	GUIHandler::get().setInMenu(false);
}

void SceneManager::showMainMenu()
{

	GUIHandler::get().setVisible(m_singleplayerIndex, true);
	GUIHandler::get().setVisible(m_tutorialIndex, true);
	GUIHandler::get().setVisible(m_settingsIndex, true);
	GUIHandler::get().setVisible(m_exitIndex, true);
	
	GUIButton* startButton = dynamic_cast<GUIButton*>(GUIHandler::get().getElementMap()->at(m_singleplayerIndex));
	GUIButton* tutorialButton = dynamic_cast<GUIButton*>(GUIHandler::get().getElementMap()->at(m_tutorialIndex));
	GUIButton* settingsButton = dynamic_cast<GUIButton*>(GUIHandler::get().getElementMap()->at(m_settingsIndex));
	GUIButton* exitButton = dynamic_cast<GUIButton*>(GUIHandler::get().getElementMap()->at(m_exitIndex));


	startButton->setPrevMenuButton(exitButton);
	startButton->setNextMenuButton(tutorialButton);

	tutorialButton->setPrevMenuButton(startButton);
	tutorialButton->setNextMenuButton(settingsButton);

	settingsButton->setPrevMenuButton(tutorialButton);
	settingsButton->setNextMenuButton(exitButton);

	exitButton->setPrevMenuButton(settingsButton);
	exitButton->setNextMenuButton(startButton);

	GUIHandler::get().setInMenu(true, m_singleplayerIndex);
}

void SceneManager::showSettingsMenu()
{
	

	GUIButton* senseIncrease = dynamic_cast<GUIButton*>(GUIHandler::get().getElementMap()->at(m_senseIncreaseIndex));
	GUIButton* senseDecrease = dynamic_cast<GUIButton*>(GUIHandler::get().getElementMap()->at(m_senseDecreaseIndex));

	GUIButton* volumeIncrease = dynamic_cast<GUIButton*>(GUIHandler::get().getElementMap()->at(m_volumeIncreaseIndex));
	GUIButton* volumeDecrease = dynamic_cast<GUIButton*>(GUIHandler::get().getElementMap()->at(m_volumeDecreaseIndex));

	GUIButton* fovIncrease = dynamic_cast<GUIButton*>(GUIHandler::get().getElementMap()->at(m_setFovIncreaseIndex));
	GUIButton* fovDecrease = dynamic_cast<GUIButton*>(GUIHandler::get().getElementMap()->at(m_setFovDecreaseIndex));

	GUIButton* fullscreen = dynamic_cast<GUIButton*>(GUIHandler::get().getElementMap()->at(m_fullscreenIndex));

	GUIButton* resumeButton = dynamic_cast<GUIButton*>(GUIHandler::get().getElementMap()->at(m_resumeBtnIndex));

	GUIButton* mainMenuButton = dynamic_cast<GUIButton*>(GUIHandler::get().getElementMap()->at(m_backToLobbyIndex));


	

	//GUIHandler::get().setInMenu(true, m_resumeBtnIndex);
	if (m_inPause) // came from pause menu
	{
		GUIHandler::get().setVisible(m_resumeBtnIndex, true);
		GUIHandler::get().setVisible(m_backToLobbyIndex, false);
	}
	else
	{
		GUIHandler::get().setVisible(m_backToLobbyIndex, true);
	}



	GUIHandler::get().setVisible(m_senseDecreaseIndex, true);
	senseDecrease->setNextMenuButton(senseIncrease);
	senseDecrease->setPrevMenuButton(fullscreen);

	GUIHandler::get().setVisible(m_senseIncreaseIndex, true);
	senseIncrease->setNextMenuButton(volumeDecrease);
	senseIncrease->setPrevMenuButton(senseDecrease);

	GUIHandler::get().setVisible(m_volumeDecreaseIndex, true);
	volumeDecrease->setPrevMenuButton(senseIncrease);
	volumeDecrease->setNextMenuButton(volumeIncrease);

	GUIHandler::get().setVisible(m_volumeIncreaseIndex, true);
	volumeIncrease->setPrevMenuButton(volumeDecrease);
	volumeIncrease->setNextMenuButton(fovDecrease);

	GUIHandler::get().setVisible(m_setFovDecreaseIndex, true);
	fovDecrease->setNextMenuButton(fovIncrease);
	fovDecrease->setPrevMenuButton(volumeIncrease);

	GUIHandler::get().setVisible(m_setFovIncreaseIndex, true);
	fovIncrease->setNextMenuButton(fullscreen);
	fovIncrease->setPrevMenuButton(fovDecrease);

	GUIHandler::get().setVisible(m_fullscreenIndex, true);
	fullscreen->setPrevMenuButton(fovIncrease);
	fullscreen->setNextMenuButton(senseDecrease);

	if(m_inPause)
	{
		fullscreen->setNextMenuButton(resumeButton);
		resumeButton->setPrevMenuButton(fullscreen);
		resumeButton->setNextMenuButton(senseDecrease);
		senseDecrease->setPrevMenuButton(resumeButton);
	}
	else
	{
		fullscreen->setNextMenuButton(mainMenuButton);

		senseDecrease->setPrevMenuButton(mainMenuButton);
		mainMenuButton->setNextMenuButton(senseDecrease);
		mainMenuButton->setPrevMenuButton(fullscreen);
	}


	GUIHandler::get().setVisible(m_sensitivityIndex, true);
	GUIHandler::get().setVisible(m_settingsText, true);
	GUIHandler::get().setVisible(m_fullscreenText, true);
	GUIHandler::get().setVisible(m_fovIndex, true);
	GUIHandler::get().setVisible(m_fovText, true);
	GUIHandler::get().setVisible(m_volumeTextIndex, true);
	GUIHandler::get().setVisible(m_senseTextIndex, true);
	GUIHandler::get().setVisible(m_volumeAmountIndex, true);
	m_inPauseSettings = true;

	GUIHandler::get().setInMenu(true, m_senseDecreaseIndex);
}

void SceneManager::hideSettingsMenu()
{
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
	GUIHandler::get().setVisible(m_fullscreenIndex, false);
	GUIHandler::get().setVisible(m_pauseText, false);
	m_inPause = false;

	GUIHandler::get().setInMenu(false);
}

void SceneManager::uiMenuInitialize()
{
	//define gui button
	GUIButtonStyle btnStyle;
	GUITextStyle textStyle;

	//------- Main Menu ----------
	//start button
	//btnStyle.position = Vector2(140, 150);
	btnStyle.position = Vector2(140, 450);
	btnStyle.scale = Vector2(1, 1);
	m_singleplayerIndex = GUIHandler::get().addGUIButton(L"singleplayerBtn.png", btnStyle);

	GUIButton* startButton = dynamic_cast<GUIButton*>(GUIHandler::get().getElementMap()->at(m_singleplayerIndex));
	startButton->Attach(this);

	//Tutorial button
	//btnStyle.position = Vector2(140, 300);
	btnStyle.position = Vector2(140, 600);
	btnStyle.scale = Vector2(1, 1);
	m_tutorialIndex = GUIHandler::get().addGUIButton(L"tutorialBtn.png", btnStyle);

	GUIButton* tutorialButton = dynamic_cast<GUIButton*>(GUIHandler::get().getElementMap()->at(m_tutorialIndex));
	tutorialButton->Attach(this);


	//join button
	//btnStyle.position = Vector2(140, 450);
	//btnStyle.scale = Vector2(1, 1);
	//m_joinGameIndex = GUIHandler::get().addGUIButton(L"joinBtn.png", btnStyle);

	//GUIButton* joinButton = dynamic_cast<GUIButton*>(GUIHandler::get().getElementMap()->at(m_joinGameIndex));
	//joinButton->Attach(this);
	//joinButton->setPrevMenuButton(tutorialButton);
	//tutorialButton->setNextMenuButton(joinButton);

	////Host Button
	//btnStyle.position = Vector2(140, 600);
	//btnStyle.scale = Vector2(1, 1);
	//m_hostGameIndex = GUIHandler::get().addGUIButton(L"hostBtn.png", btnStyle);

	//GUIButton* hostButton = dynamic_cast<GUIButton*>(GUIHandler::get().getElementMap()->at(m_hostGameIndex));
	//hostButton->Attach(this);
	//hostButton->setPrevMenuButton(joinButton);
	//joinButton->setNextMenuButton(hostButton);

	//Settings button
	btnStyle.position = Vector2(140, 750);
	btnStyle.scale = Vector2(1, 1);
	m_settingsIndex = GUIHandler::get().addGUIButton(L"optionsBtn.png", btnStyle);

	GUIButton* settingsButton = dynamic_cast<GUIButton*>(GUIHandler::get().getElementMap()->at(m_settingsIndex));
	settingsButton->Attach(this);

	//Exit button
	btnStyle.position = Vector2(140, 900);
	btnStyle.scale = Vector2(1, 1);
	m_exitIndex = GUIHandler::get().addGUIButton(L"exitBtn.png", btnStyle);

	GUIButton* exitButton = dynamic_cast<GUIButton*>(GUIHandler::get().getElementMap()->at(m_exitIndex));
	exitButton->Attach(this);
	

	//------- Options/Settings ---------
	
	//Options/Settings Label
	textStyle.position.x = 140.0f;
	textStyle.position.y = 100.0f;
	m_settingsText = GUIHandler::get().addGUIText("Options", L"concert_one_60.spritefont", textStyle);

	// --Sensetivity--
	// Sensetivity Label
	textStyle.position.x = 140.0f;
	textStyle.position.y = 200.0f;
	m_senseTextIndex = GUIHandler::get().addGUIText("Sensitivity", L"concert_one_32.spritefont", textStyle);

	//sense Increase button
	btnStyle.position = Vector2(500, 200);
	btnStyle.scale = Vector2(0.9f);
	m_senseIncreaseIndex = GUIHandler::get().addGUIButton(L"rightBtn.png", btnStyle);

	GUIButton* senseIncrease = dynamic_cast<GUIButton*>(GUIHandler::get().getElementMap()->at(m_senseIncreaseIndex));
	senseIncrease->Attach(this);

	//sense decrease button
	btnStyle.position = Vector2(370, 200);
	m_senseDecreaseIndex = GUIHandler::get().addGUIButton(L"leftBtn.png", btnStyle);

	GUIButton* senseDecrease = dynamic_cast<GUIButton*>(GUIHandler::get().getElementMap()->at(m_senseDecreaseIndex));
	senseDecrease->Attach(this);

	//sense amount label
	textStyle.position.x = 660.0f;
	textStyle.position.y = 200.0f;
	m_sensitivityIndex = GUIHandler::get().addGUIText(std::to_string(m_cameraSense), L"concert_one_32.spritefont", textStyle);

	//--Volume--
	//Volume Label
	textStyle.position.x = 140.0f;
	textStyle.position.y = 300.0f;
	m_volumeTextIndex = GUIHandler::get().addGUIText("Volume", L"concert_one_32.spritefont", textStyle);

	//Volume Increase button
	btnStyle.position = Vector2(500, 300);
	m_volumeIncreaseIndex = GUIHandler::get().addGUIButton(L"rightBtn.png", btnStyle);

	GUIButton* volumeIncrease = dynamic_cast<GUIButton*>(GUIHandler::get().getElementMap()->at(m_volumeIncreaseIndex));
	volumeIncrease->Attach(this);

	//Voume Decrease button
	btnStyle.position = Vector2(370, 300);
	m_volumeDecreaseIndex = GUIHandler::get().addGUIButton(L"leftBtn.png", btnStyle);

	GUIButton* volumeDecrease = dynamic_cast<GUIButton*>(GUIHandler::get().getElementMap()->at(m_volumeDecreaseIndex));
	volumeDecrease->Attach(this);

	//volume amount label
	textStyle.position.x = 660.0f;
	textStyle.position.y = 300.0f;
	m_volumeAmountIndex = GUIHandler::get().addGUIText(std::to_string(m_volumeAmount), L"concert_one_32.spritefont", textStyle);

	//--FOV--
	//fov Label
	textStyle.position.x = 140.0f;
	textStyle.position.y = 400.0f;
	m_fovText = GUIHandler::get().addGUIText("FOV", L"concert_one_32.spritefont", textStyle);

	//fov button
	btnStyle.position = Vector2(500, 400);
	m_setFovIncreaseIndex = GUIHandler::get().addGUIButton(L"rightBtn.png", btnStyle);

	GUIButton* fovIncreaseBtn = dynamic_cast<GUIButton*>(GUIHandler::get().getElementMap()->at(m_setFovIncreaseIndex));
	fovIncreaseBtn->Attach(this);

	//fov button
	btnStyle.position = Vector2(370, 400);
	m_setFovDecreaseIndex = GUIHandler::get().addGUIButton(L"leftBtn.png", btnStyle);

	GUIButton* fovDecreaseBtn = dynamic_cast<GUIButton*>(GUIHandler::get().getElementMap()->at(m_setFovDecreaseIndex));
	fovDecreaseBtn->Attach(this);

	//fov amount label
	textStyle.position.x = 660.0f;
	textStyle.position.y = 400.0f;
	m_fovIndex = GUIHandler::get().addGUIText(std::to_string(m_camera->fovAmount), L"concert_one_32.spritefont", textStyle);

	//--Fullscreen--
	//Fullscreen Label
	textStyle.position.x = 140.0f;
	textStyle.position.y = 500.0f;
	m_fullscreenText = GUIHandler::get().addGUIText("FullScreen", L"concert_one_32.spritefont", textStyle);

	//Fullscreen checkbox
	btnStyle.position = Vector2(370, 500);
	btnStyle.scale = Vector2(0.5f);
	m_fullscreenIndex = GUIHandler::get().addGUIButton(L"checkedBox.png", btnStyle);

	GUIButton* fullscreenBtn = dynamic_cast<GUIButton*>(GUIHandler::get().getElementMap()->at(m_fullscreenIndex));
	fullscreenBtn->Attach(this);

	//Main Menu Buttons
	btnStyle.position = Vector2(140, 900);
	btnStyle.scale = Vector2(1, 1);
	m_backToLobbyIndex = GUIHandler::get().addGUIButton(L"mainmenuBtn.png", btnStyle);

	GUIButton* backToLobbyButton = dynamic_cast<GUIButton*>(GUIHandler::get().getElementMap()->at(m_backToLobbyIndex));
	backToLobbyButton->Attach(this);

	//------- Pause ---------
	//Pause Label
	textStyle.position.x = 140.0f;
	textStyle.position.y = 100.0f;
	m_pauseText = GUIHandler::get().addGUIText("Pause", L"concert_one_60.spritefont", textStyle);

	//resume button
	btnStyle.position = Vector2(140, 600);
	btnStyle.scale = Vector2(1, 1);
	m_resumeBtnIndex = GUIHandler::get().addGUIButton(L"resumeBtn.png", btnStyle);

	GUIButton* resumeBtn = dynamic_cast<GUIButton*>(GUIHandler::get().getElementMap()->at(m_resumeBtnIndex));
	resumeBtn->Attach(this);

	showMainMenu();

	// Used for Menu Selection
	GUIHandler::get().setInMenu(true, m_singleplayerIndex);
}
