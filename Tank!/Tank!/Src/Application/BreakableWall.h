#pragma once
#include "GameObject.h"

class BreakableWall : public GameObject
{
    KdTexture* m_pTex = nullptr;
public:
    BreakableWall(float x, float y, KdTexture* tex);

    void Draw() override;
};