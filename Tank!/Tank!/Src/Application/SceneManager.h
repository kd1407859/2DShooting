#pragma once
#include "Scene.h"

class SceneManager
{
private:
	Scene* m_currentScene = nullptr; // 現在実行中のシーン

public:
	// シングルトンパターン（ここに移設）
	static SceneManager& GetInstance() {
		static SceneManager instance;
		return instance;
	}

	void Init();   // 最初のシーンを作る
	void Release();// メモリ解放
	void Update(); // 現在のシーンのUpdateを呼ぶ
	void Draw();   // 現在のシーンのDrawを呼ぶ

	// シーン切り替え関数
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
