#pragma once
#include "Scene.h"
#include"iContext.h"
#include "GUIContext.h"
#include "Boss.h"
#include <thread>
#include <future>
#include "Camera.h"
#include <Windows.h>
#include "Renderer.h"



class SceneManager : public InputObserver, public PhysicsObserver, public GUIObserver
{
private:
	Scene* m_currentScene;
	Scene* m_nextScene;
	Input* m_inputPtr;
	ScenesEnum m_currectSceneEnum;
	ScenesEnum m_nextSceneEnum;
	Camera* m_camera;
	std::string str;
	bool m_swapScene;

	//GUI score
	int m_highScoreLabelIndex;
	int m_playerOneScoreIndex;
	int m_playerTwoScoreIndex;
	int m_playerThreeScoreIndex;

	int m_rankingScoreIndecOne;
	int m_rankingScoreIndecTwo;
	int m_rankingScoreIndecThree;

	bool checked = true;

	int m_menuOverlayIndex = 0;
	bool m_inPause = false;
	bool m_inPauseSettings = false;
	int m_cameraSense;
	int m_volumeAmount;
	std::vector<iContext*>* m_contexts;
	bool showInputBar = true;

	char charPtr[256] = { 0 };
	int m_singleplayerIndex = 0;
	int m_tutorialIndex = 0;
	int m_exitIndex = 0;
	int m_joinGameIndex = 0;
	int m_hostGameIndex = 0;
	int m_backToLobbyIndex = 0;
	int m_settingsIndex = 0;
	int m_senseIncreaseIndex = 0;
	int m_senseDecreaseIndex = 0;
	int m_volumeIncreaseIndex = 0;
	int m_volumeDecreaseIndex = 0;
	int m_volumeAmountIndex = 0;
	int m_setFovIncreaseIndex = 0;
	int m_setFovDecreaseIndex = 0;
	int m_controllsUIIndex = 0;
	int m_resumeBtnIndex = 0;
	int m_fullscreenIndex = 0;
	int m_exitFullscreenindex = 0;


	//text index
	int m_sensitivityIndex;
	int m_fovIndex;
	int m_fovText;
	int m_senseTextIndex;
	int m_volumeTextIndex;
	int m_settingsText;
	int m_pauseText;
	int m_fullscreenText;

	bool* m_nextSceneReady = new bool;
	bool m_loadNextSceneWhenReady = false;
	std::vector<std::pair<int, std::string>>* m_scores;
	void hideScore();
	void showScore(); 

	void hideMainMenu();
	void hidePauseMenu();
	void hideSettingsMenu();
	void showPauseMenu();
	void showMainMenu();
	void showSettingsMenu();

	void uiMenuInitialize();

public:
	SceneManager();
	~SceneManager();

	bool m_gameStarted = false;
	bool endGame = false;
	void initalize(Input* input);
	void updateScene(const float &dt);

	void inputUpdate(InputData& inputData);

	void sendPhysicsMessage(PhysicsData& physicsData, bool& destroyEntity);

	void disableMovement();
	void enableMovement();

	void setContextPtr(std::vector<iContext*>* contexts);
	std::vector<iContext*>* getContextPtr();

	void setScorePtr(std::vector<std::pair<int, std::string>>* m_scores);
	std::vector<std::pair<int, std::string>>* getScorePtr();

	void inputIP();

	void swapScenes();

	// Inherited via GUIObserver
	virtual void update(GUIUpdateType type, GUIElement* guiElement) override;
};
