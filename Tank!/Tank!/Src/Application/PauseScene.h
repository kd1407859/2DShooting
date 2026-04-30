#pragma once
#include "Scene.h"

class PauseScene : public Scene
{
    KdTexture m_pauseTex;
    KdTexture m_resumeTexture;
    KdTexture m_dirtTex;
    KdTexture m_wallTex;
    KdTexture m_breakableWallTex;
    KdTexture m_holeTex;

    int m_blinkTimer = 0;
    bool m_blinkVisible = true;

    // マップデータ
    static const int MAP_W = 20;
    static const int MAP_H = 12;
    int m_backgroundMap[MAP_H][MAP_W] = { 0 };
    int m_currentStage = 1;

public:

    PauseScene(int stageNum = 1);
    ~PauseScene();

    void Init()    override;
    void Update()  override;
    void Draw()    override;
};
