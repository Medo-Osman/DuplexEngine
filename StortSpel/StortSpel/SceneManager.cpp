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
	m_currentScene->loadLobby();
	//Update currentScene in engine
	Engine::get().setEntitiesMapPtr(m_currentScene->getEntityMap());
	Engine::get().setLightComponentMapPtr(m_currentScene->getLightMap());
	Engine::get().setMeshComponentMapPtr(m_currentScene->getMeshComponentMap());

	//m_nextScene = new Scene();
	//m_nextScene->loadScene("playTest");
	
}

void SceneManager::updateScene(const float &dt)
{
	m_currentScene->updateScene(dt);
}

void SceneManager::inputUpdate(InputData& inputData)
{
	for (size_t i = 0; i < inputData.actionData.size(); i++)
	{
		if (inputData.actionData[i] == SWAP_SCENES)
		{
			Scene* temp;
			temp = m_currentScene;
			m_currentScene = m_nextScene;
			m_nextScene = temp;
			Engine::get().setEntitiesMapPtr(m_currentScene->getEntityMap());
			Engine::get().setLightComponentMapPtr(m_currentScene->getLightMap());
			Engine::get().setMeshComponentMapPtr(m_currentScene->getMeshComponentMap());
		}
	}
}
