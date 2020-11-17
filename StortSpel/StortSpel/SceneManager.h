#pragma once
#include "Scene.h"
#include "Boss.h"
#include <thread>
#include <future>



class SceneManager : public InputObserver, public PhysicsObserver
{
private:
	Scene* m_currentScene;
	Scene* m_nextScene;
	bool m_gameStarted;
	bool m_swapScene;
	bool* m_nextSceneReady = new bool;
	bool m_loadNextSceneWhenReady = false;
	ScenesEnum m_nextSceneEnum;

public:
	SceneManager();
	~SceneManager();
	
	void initalize();
	void updateScene(const float &dt);

	void inputUpdate(InputData& inputData);

	void sendPhysicsMessage(PhysicsData& physicsData, bool& destroyEntity);

	void swapScenes();
};