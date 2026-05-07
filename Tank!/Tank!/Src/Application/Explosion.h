#pragma once
#include "GameObject.h"

class Explosion : public GameObject
{
    KdTexture* m_pTex = nullptr;
    int m_animTimer = 0;
    int m_animFrame = 0;
    int m_maxFrame = 1;

public:
    Explosion(float x, float y, KdTexture* tex);
    void Update() override;
    void Draw() override;
};