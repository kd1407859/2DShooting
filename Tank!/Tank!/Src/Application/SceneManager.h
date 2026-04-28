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
	void ChangeScene(Scene* newScene);
};

#define SCENE_MGR SceneManager::GetInstance()
