#pragma once
#include "Scene.h"

class ClearScene : public Scene
{
	KdTexture m_clearTex;
	KdTexture m_pressEnterTex;
	KdTexture m_dirtTex;
	KdTexture m_wallTex;

	int m_clearedStages = 0;
	int m_blinkTimer = 0;
	bool m_blinkVisible = true;

	// マップデータ
	static const int MAP_W = 20;
	static const int MAP_H = 12;
	int m_backgroundMap[MAP_H][MAP_W] = { 0 }; // 未初期化バグ修正

public:

	ClearScene(int stageNum);
	~ClearScene();

	void Init()    override;
	void Update()  override;
	void Draw()    override;
};