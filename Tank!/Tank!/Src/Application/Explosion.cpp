#include "Explosion.h"

Explosion::Explosion(float x, float y, KdTexture* tex)
{
    pos = { x, y };
    m_pTex = tex;


    // テクスチャが読み込まれていれば、横幅を64で割って全コマ数を自動計算する
    if (m_pTex)
    {
        m_maxFrame = m_pTex->GetInfo().Width / 64;
    }
}

void Explosion::Update()
{
    m_animTimer++;

    // 60フレームごとに次のコマへ進む（数字を小さくするとアニメーションが速くなります）
    if (m_animTimer % 60 == 0)
    {
        m_animFrame++;

        // 全てのコマを表示しきったらオブジェクトを消滅させる
        if (m_animFrame >= m_maxFrame)
        {
            isDead = true;
        }
    }
}

void Explosion::Draw()
{
    if (!m_pTex || isDead) return;

    Math::Matrix mat = Math::Matrix::CreateTranslation(pos.x, pos.y, 0.0f);
    SHADER.m_spriteShader.SetMatrix(mat);

    Math::Rectangle srcRect = {m_animFrame*64, 0, 64, 64};

    SHADER.m_spriteShader.DrawTex(m_pTex, 0, 0, 64, 64, &srcRect);
}