#pragma once
#include "Scene.h"

class SceneManager
{
private:
	Scene* m_currentScene = nullptr;

public:

	static SceneManager& GetInstance() {
		static SceneManager instance;
		return instance;
	}

	void Init();
	void Release();
	void Update();
	void Draw();

	// ƒV[ƒ“Ø‚è‘Ö‚¦ŠÖ”
	void ChangeScene(Scene* newScene) {
		if (m_currentScene) {
			m_currentScene->Release();
			delete m_currentScene;
		}
		m_currentScene = newScene;
		m_currentScene->Init();
	}
};

#define SCENE_MGR SceneManager::GetInstance()
