#include "Hole.h"
#include "main.h"

Hole::Hole(float x, float y, KdTexture* tex)
    : m_pTex(tex)
{
    pos = { x, y };
}

void Hole::Draw()
{
    if (isDead) return;

    Math::Matrix mat = Math::Matrix::CreateTranslation(pos.x, pos.y, 0.0f);
    SHADER.m_spriteShader.SetMatrix(mat);

    Math::Rectangle srcRect = { 0, 0, 64, 64 };
    SHADER.m_spriteShader.DrawTex(m_pTex, 0, 0, &srcRect);
}
