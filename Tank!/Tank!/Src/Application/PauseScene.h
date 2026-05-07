#pragma once
#include "Scene.h"

class PauseScene : public Scene
{
    // --- テクスチャ ---
    KdTexture m_pauseTex;       // PAUSEロゴ (512x128)
    KdTexture m_stageTex;       // "Stage" ラベル (512x128)
    KdTexture m_numberTex;      // 数字スプライト (279x34, 各桁28px)
    KdTexture m_resumeTex;      // 再開ボタン
    KdTexture m_toTitleTex;     // タイトルに戻るボタン
    KdTexture m_dirtTex;
    KdTexture m_wallTex;
    KdTexture m_breakableWallTex;
    KdTexture m_holeTex;
    KdTexture m_enemyTexBrown;  // 茶色戦車アイコン用
    KdTexture m_enemyTexAsh;    // 灰色戦車アイコン用

    // --- 点滅 ---
    int  m_blinkTimer = 0;
    bool m_blinkVisible = true;

    // --- 選択（0:再開  1:タイトル）---
    int  m_selectIndex = 0;
    bool m_prevUpKey = false;
    bool m_prevDownKey = false;
    bool m_prevEnterKey = false;
    bool m_prevPKey = false;

    // --- マップデータ ---
    static const int MAP_W = 20;
    static const int MAP_H = 12;
    int m_backgroundMap[MAP_H][MAP_W] = { 0 };

    // --- 引き継ぎ情報 ---
    int m_currentStage = 1;
    int m_brownCount = 0;
    int m_ashCount = 0;

    // --- 数字描画ヘルパー ---
    void DrawNumber(int value, float cx, float cy, float scale = 1.0f);

public:
    PauseScene(int stageNum, int brownCount, int ashCount);
    ~PauseScene();

    void Init()    override;
    void Update()  override;
    void Draw()    override;
    void Release() override {}
};