#include "3DPCH.h"
#include "SceneManager.h"

SceneManager::SceneManager()
{
}

SceneManager::~SceneManager()
{
	delete m_currentScene;
	delete m_nextScene;
}

void SceneManager::initalize()
{
	m_currentScene = new Scene();
	m_currentScene->loadScene("playTest");
}

void SceneManager::updateScene(const float &dt)
{
	m_currentScene->updateScene(dt);
}
