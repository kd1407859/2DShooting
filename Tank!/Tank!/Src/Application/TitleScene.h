#pragma once
#include "Scene.h"
#include "Enemy.h"
#include "Wall.h"

class TitleScene : public Scene
{
    KdTexture m_titleTex;
    KdTexture m_pressEnterTex;
    KdTexture m_dirtTex;
    KdTexture m_wallTex;
    std::vector<std::shared_ptr<Wall>> m_walls;

    // 戦車リスト
    std::vector<std::shared_ptr<Enemy>> m_tanks;
    KdTexture m_enemyTexBrown;
    KdTexture m_enemyTexAsh;

    //マップデータ
    static const int MAP_W = 20;
    static const int MAP_H = 12;
    int m_titleMap[MAP_H][MAP_W] = { 0 };

    // タイトルロゴのスライドイン用
    float m_logoY = 400.0f;
    float m_logoTargetY = 100.0f;

    // PRESS ENTERの点滅用
    int   m_blinkTimer = 0;
    bool  m_blinkVisible = true;

public:
    void Init()    override;
    void Update()  override;
    void Draw()    override;
    void Release() override;
};
