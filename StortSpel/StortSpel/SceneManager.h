#pragma once
#include "Scene.h"


class SceneManager : public InputObserver, public PhysicsObserver, public GUIObserver
{
private:
	Scene* m_currentScene;
	Scene* m_nextScene;
	bool m_gameStarted;
	bool m_swapScene;
	ScenesEnum m_nextSceneEnum;
	int m_startGameIndex = 0;
	int m_joinGameIndex = 0;
	int m_customization = 0;
	int m_hostGameIndex = 0;
	bool m_gameRestarted = true;
public:
	SceneManager();
	~SceneManager();
	
	void initalize();
	void updateScene(const float &dt);

	void inputUpdate(InputData& inputData);

	void sendPhysicsMessage(PhysicsData& physicsData, bool& destroyEntity);

	void swapScenes();

	// Inherited via GUIObserver
	virtual void update(GUIUpdateType type, GUIElement* guiElement) override;
};