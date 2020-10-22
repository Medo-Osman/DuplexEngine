#pragma once
#include "Scene.h"


class SceneManager : public InputObserver
{
private:
	Scene* m_currentScene;
	Scene* m_nextScene;
public:
	SceneManager();
	~SceneManager();
	
	void initalize();
	void updateScene(const float &dt);

	void inputUpdate(InputData& inputData);

};