#pragma once
#include "GameObject.h"

class Hole : public GameObject
{
    KdTexture* m_pTex = nullptr;
public:
    Hole(float x, float y, KdTexture* tex);

    void Draw() override;
};