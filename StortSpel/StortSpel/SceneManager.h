#pragma once
#include "Scene.h"
#include"iContext.h"
#include "GUIContext.h"
#include "Boss.h"


class SceneManager : public InputObserver, public PhysicsObserver, public GUIObserver
{
private:
	Scene* m_currentScene;
	Scene* m_nextScene;
	Input* inputPtr;
	ScenesEnum m_nextSceneEnum;


	bool m_swapScene;

	std::vector<iContext*>* m_contexts;
	int m_singleplayerIndex = 4;
	int m_exitIndex = 5;
	int m_joinGameIndex = 6;
	int m_hostGameIndex = 7;
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

	void setContextPtr(std::vector<iContext*>* m_contexts);
	//std::unordered_map<std::string, LightComponent*>* getLightComponentMap();
	std::vector<iContext*>* getContextPtr();

	void swapScenes();

	// Inherited via GUIObserver
	virtual void update(GUIUpdateType type, GUIElement* guiElement) override;
};
