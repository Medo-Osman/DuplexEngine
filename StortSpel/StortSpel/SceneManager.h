#pragma once
#include "Scene.h"
#include "Boss.h"



class SceneManager : public InputObserver, public PhysicsObserver
{
private:
	Scene* m_currentScene;
	Scene* m_nextScene;
	bool m_gameStarted;
	bool m_swapScene;
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