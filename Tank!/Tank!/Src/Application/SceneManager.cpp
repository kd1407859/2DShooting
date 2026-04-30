#include "SceneManager.h"
#include "TitleScene.h"
#include "InputManager.h"

void SceneManager::Init()
{
	ChangeScene(new TitleScene());
}

void SceneManager::Update()
{
	InputManager::GetInstance().Update();

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

void SceneManager::ChangeScene(Scene* newScene)
{
	if (m_currentScene) {
		m_currentScene->Release();
		delete m_currentScene;
	}
	m_currentScene = newScene;
	m_currentScene->Init();
	InputManager::GetInstance().OnSceneTransition();
}

