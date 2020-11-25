#pragma once
#include "Scene.h"
#include"iContext.h"
#include "GUIContext.h"
#include "Boss.h"
#include <thread>
#include <future>
#include "Camera.h"


class SceneManager : public InputObserver, public PhysicsObserver, public GUIObserver
{
private:
	Scene* m_currentScene;
	Scene* m_nextScene;
	bool m_swapScene;
	bool* m_nextSceneReady = new bool;
	bool m_loadNextSceneWhenReady = false;
	ScenesEnum m_nextSceneEnum;
	Input* inputPtr;
	Camera* m_camera;

	std::vector<iContext*>* m_contexts;
	int m_singleplayerIndex = 0;
	int m_exitIndex = 0;
	int m_joinGameIndex = 0;
	int m_hostGameIndex = 0;
public:
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

	void swapScenes();

	// Inherited via GUIObserver
	virtual void update(GUIUpdateType type, GUIElement* guiElement) override;
};
