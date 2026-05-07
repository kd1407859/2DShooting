#pragma once
#include "Scene.h"

class GameOverScene : public Scene
{
	KdTexture m_gameOverTex;
	KdTexture m_pressEnterTex;
	KdTexture m_dirtTex;
	KdTexture m_wallTex;

	int m_failedStage = 0;
	int m_blinkTimer = 0;
	bool m_blinkVisible = true;

	// マップデータ
	static const int MAP_W = 20;
	static const int MAP_H = 12;
	int m_backgroundMap[MAP_H][MAP_W] = { 0 }; // 未初期化バグ修正

public:

	GameOverScene(int failedStage);
	~GameOverScene();

	void Init()    override;
	void Update()  override;
	void Draw()    override;
};