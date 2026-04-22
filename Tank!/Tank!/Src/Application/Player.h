#pragma once
#include "GameObject.h"
#include"main.h"

class Player : public GameObject {

    KdTexture m_tex;
    Math::Matrix m_mat;

    bool success;

    float m_angle = 0.0f;

public:

    Player()
    {
        m_tex.Load("Texture/Player/player.png");

        pos = { 0,0 };

        m_angle = 0.0f;
    }

    float GetAngle() const
    {
        return m_angle;
    }

    void Update() override {
        
        // --- 直感的なWASD移動処理 ---
        float speed = 4.0f; // 戦車の移動スピード

        if (GetAsyncKeyState('W') & 0x8000) {
            pos.y += speed; // 上へ
        }
        if (GetAsyncKeyState('S') & 0x8000) {
            pos.y -= speed; // 下へ
        }
        if (GetAsyncKeyState('A') & 0x8000) {
            pos.x -= speed; // 左へ
        }
        if (GetAsyncKeyState('D') & 0x8000) {
            pos.x += speed; // 右へ
        }
       
        const float BODY_SIZE = 32.0f;

        // 左端
        if (pos.x < SCREEN_LEFT + BODY_SIZE)  pos.x = SCREEN_LEFT + BODY_SIZE;

        // 右端
        if (pos.x > SCREEN_RIGHT - BODY_SIZE) pos.x = SCREEN_RIGHT - BODY_SIZE;

        // 下端
        if (pos.y < SCREEN_BOTTOM + BODY_SIZE) pos.y = SCREEN_BOTTOM + BODY_SIZE;

        // 上端
        if (pos.y > SCREEN_TOP - BODY_SIZE)    pos.y = SCREEN_TOP - BODY_SIZE;

        

        Math::Matrix transMat = Math::Matrix::CreateTranslation(pos.x, pos.y, 0);

        m_mat = transMat;
        
    }

    void Draw() override {
        // 自分の描画
        Math::Rectangle srcRect = { 0, 0, 64, 64 };

        SHADER.m_spriteShader.SetMatrix(m_mat);
        SHADER.m_spriteShader.DrawTex(&m_tex, 0,0,64,64,&srcRect);
    }
};
