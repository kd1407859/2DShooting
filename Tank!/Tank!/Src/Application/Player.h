#pragma once
#include "GameObject.h"
#include"main.h"

class Player : public GameObject {

    KdTexture m_tex;
    Math::Matrix m_mat;

    bool success = false;

    float m_angle = 0.0f;

public:

    Player();
    

    float GetAngle() const { return m_angle; }

    void Update() override;

    void Draw() override;
};
