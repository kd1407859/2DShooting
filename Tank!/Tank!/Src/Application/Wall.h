#pragma once
#include "GameObject.h"


class Wall : public GameObject {
    KdTexture* m_pTex = nullptr;
public:
    Wall(float x, float y, KdTexture* tex);
    void Draw() override;
};