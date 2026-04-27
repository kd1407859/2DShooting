#pragma once
#include "GameObject.h"

class BreakableWall : public GameObject
{
    KdTexture* m_pTex = nullptr;
public:
    BreakableWall(float x, float y, KdTexture* tex) {
        pos = { x, y };
        m_pTex = tex;
    }

    void Draw() override {
        Math::Rectangle rect = { 0, 0, 64, 64 };
        Math::Matrix worldMat = Math::Matrix::CreateTranslation(pos.x, pos.y, 0);
        SHADER.m_spriteShader.SetMatrix(worldMat);
        SHADER.m_spriteShader.DrawTex(m_pTex, 0, 0, &rect);
    }
};