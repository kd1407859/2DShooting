#include "SceneManager.h"
#include "GameScene.h"
#include "TitleScene.h"

void SceneManager::Init()
{
	ChangeScene(new TitleScene());
}

void SceneManager::Update()
{
	if (m_currentScene) {
		m_currentScene->Update();
	}
}

void SceneManager::Release()
{
	if (m_currentScene) {
		m_currentScene->Release();
		delete m_currentScene;
		m_currentScene = nullptr;
	}
}

void SceneManager::Draw()
{
	if (m_currentScene) {
		m_currentScene->Draw();
	}
}