#pragma once
#include "GameObject.h"


class Wall : public GameObject
{

	KdTexture* m_pTex = nullptr;

public:

	Wall(float x, float y, KdTexture* tex)
	{
		pos.x = x;
		pos.y = y;
		m_pTex = tex;
	}

	~Wall() {}

	void Draw() override
	{
		if (!m_pTex) return;

		Math::Rectangle rect = { 0, 0, 64, 64 };

		// 壁の位置に描画
		Math::Matrix worldMat = Math::Matrix::CreateTranslation(pos.x, pos.y, 0);
		SHADER.m_spriteShader.SetMatrix(worldMat);
		SHADER.m_spriteShader.DrawTex(m_pTex, 0, 0,&rect);
	}

};