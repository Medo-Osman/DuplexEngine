#pragma once
#include "Scene.h"


class SceneManager
{
private:
	Scene* m_currentScene;
	Scene* m_nextScene;
public:
	SceneManager();
	~SceneManager();
	
	void initalize();
	void updateScene(const float &dt);

};