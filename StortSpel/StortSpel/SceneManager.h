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
	bool loadNextSceneWhenReady = false;
	ScenesEnum m_nextSceneEnum;

	//std::shared_future<void> result;
	//std::vector<std::future<void>> futures;
public:
	SceneManager();
	~SceneManager();
	
	void initalize();
	void updateScene(const float &dt);

	void inputUpdate(InputData& inputData);

	void sendPhysicsMessage(PhysicsData& physicsData, bool& destroyEntity);

	void swapScenes();
};