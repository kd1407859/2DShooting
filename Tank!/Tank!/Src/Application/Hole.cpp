#include "Hole.h"

Hole::Hole(float x, float y, KdTexture* tex)
{
    pos = { x, y };
    m_pTex = tex;
}

void Hole::Draw()
{
    if (!m_pTex) return;

    Math::Rectangle rect = { 0, 0, 64, 64 };
    // 穴専用のテクスチャ（少し暗い色など）を指定
    Math::Matrix worldMat = Math::Matrix::CreateTranslation(pos.x, pos.y, 0);
    SHADER.m_spriteShader.SetMatrix(worldMat);
    SHADER.m_spriteShader.DrawTex(m_pTex, 0, 0, &rect);
}
