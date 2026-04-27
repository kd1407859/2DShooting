#include "TitleScene.h"
#include "GameScene.h"
#include "SceneManager.h"

void TitleScene::Update()
{
	if (GetAsyncKeyState(VK_RETURN) & 0x8000)
	{
		SceneManager::GetInstance().ChangeScene(new GameScene());
	}
}

void TitleScene::Draw()
{
	SHADER.m_spriteShader.DrawString(100, 100, "TITLE SCREEN", Math::Vector4(1, 1, 1, 1));

	SHADER.m_spriteShader.DrawString(100, 0, "PRESS ENTER TO START", Math::Vector4(1, 1, 1, 1));
}

