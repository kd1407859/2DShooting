#pragma once
#include "Scene.h"
#include "SceneManager.h"
#include "TitleScene.h" // タイトルに戻るために必要
#include "main.h"

class GameOverScene : public Scene
{
	KdTexture m_tex;

	int m_finalScore = 0;

public:

	GameOverScene(int score)
	{
		m_finalScore = score;
	}

	void Init() override
	{
		// いただいた画像のパスを指定
		m_tex.Load("Texture/Font/gameover.png");
	}

	void Update() override
	{
		// Enterキーが押されたらタイトルへ戻る
		if (GetAsyncKeyState(VK_RETURN) & 0x8000)
		{
			SceneManager::GetInstance().ChangeScene(new TitleScene());
		}
	}

	void Draw() override
	{
		// 画像サイズに合わせて表示
		Math::Rectangle srcRect = { 0, 0, 842, 150 };

		Math::Matrix worldMat = Math::Matrix::Identity;
		SHADER.m_spriteShader.SetMatrix(worldMat);

		// 画面中央(0,0)に描画
		SHADER.m_spriteShader.DrawTex(&m_tex, 0, 0, &srcRect);

		// ★追加：最終スコアを表示する
		std::string scoreStr = "FINAL SCORE: " + std::to_string(m_finalScore);
		// 画面中央より少し下（Y:-100）に表示
		SHADER.m_spriteShader.DrawString(-100, -100, scoreStr.c_str(), Math::Vector4(1, 1, 0, 1)); // 黄色

	}
};