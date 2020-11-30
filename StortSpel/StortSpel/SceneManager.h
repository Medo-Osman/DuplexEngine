#pragma once
#include "Scene.h"
#include"iContext.h"
#include "GUIContext.h"
#include "Boss.h"
#include <thread>
#include <future>
#include "Camera.h"
#include "Server.h"

class SceneManager : public InputObserver, public PhysicsObserver, public GUIObserver
{
private:
	Scene* m_nextScene;
	Input* inputPtr;
	ScenesEnum m_nextSceneEnum;
	Camera* m_camera;

	bool m_swapScene;

	//GUI score
	int m_highScoreLabelIndex;
	int m_playerOneScoreIndex;
	int m_playerTwoScoreIndex;
	int m_playerThreeScoreIndex;

	int m_rankingScoreIndecOne;
	int m_rankingScoreIndecTwo;
	int m_rankingScoreIndecThree;
	//
	std::vector<iContext*>* m_contexts;

	int m_singleplayerIndex = 0;
	int m_exitIndex = 0;
	int m_joinGameIndex = 0;
	int m_hostGameIndex = 0;
	int m_backToLobbyIndex = 0;

	//
	int m_multiPlayerIndexTwo;
	int m_multiPlayerIndexThree;
	int m_multiPlayerIndexFour;


	bool* m_nextSceneReady = new bool;
	bool m_loadNextSceneWhenReady = false;
	std::vector<std::pair<int, std::string>>* m_scores;
	void hideScore();
	void showScore(); 

	void startBarrelDrop(std::string entity);

public:
	Scene* m_currentScene;
	SceneManager();
	~SceneManager();

	bool m_gameStarted = false;
	bool endGame = false;
	void initalize();
	void updateScene(const float &dt);

	void inputUpdate(InputData& inputData);

	void sendPhysicsMessage(PhysicsData& physicsData, bool& destroyEntity);

	void disableMovement();
	void enableMovement();

	void setContextPtr(std::vector<iContext*>* contexts);
	std::vector<iContext*>* getContextPtr();

	void setScorePtr(std::vector<std::pair<int, std::string>>* m_scores);
	std::vector<std::pair<int, std::string>>* getScorePtr();

	void swapScenes();

	// Inherited via GUIObserver
	virtual void update(GUIUpdateType type, GUIElement* guiElement) override;
};
