#include "PauseScene.h"
#include "SceneManager.h"
#include "GameScene.h"
#include "InputManager.h"
#include "main.h"

PauseScene::PauseScene(int stageNum)
{
    m_currentStage = stageNum;
}

PauseScene::~PauseScene()
{
}

void PauseScene::Init()
{
    // ポーズ画面の画像を読み込み
    m_pauseTex.Load("Texture/Font/pause.png");
    m_resumeTexture.Load("Texture/Font/press_click.png");
    m_dirtTex.Load("Texture/Map/dirt.png");
    m_wallTex.Load("Texture/Map/createMetal.png");
    m_breakableWallTex.Load("Texture/Map/crateWood.png");
    m_holeTex.Load("Texture/Map/hole.png");

    // マップデータを初期化してからファイル読み込み
    memset(m_backgroundMap, 0, sizeof(m_backgroundMap));

    std::string fileName = "Texture/Map/stage" + std::to_string(m_currentStage) + ".txt";
    std::ifstream file(fileName);
    if (file.is_open())
    {
        std::string line;
        int y = 0;
        while (std::getline(file, line) && y < MAP_H)
        {
            for (int x = 0; x < MAP_W && x < (int)line.length(); x++)
            {
                if (line[x] == '1')
                    m_backgroundMap[y][x] = 1;
                else if (line[x] == '2')
                    m_backgroundMap[y][x] = 2;
                else if (line[x] == '3')
                    m_backgroundMap[y][x] = 3;
                else if (line[x] == '4')
                    m_backgroundMap[y][x] = 4;
                else if (line[x] == '5')
                    m_backgroundMap[y][x] = 5;
                else if (line[x] == '6')
                    m_backgroundMap[y][x] = 6;
                else
                    m_backgroundMap[y][x] = 0;
            }
            y++;
        }
        file.close();
    }

    m_blinkTimer = 0;
    m_blinkVisible = true;
}

void PauseScene::Update()
{
    // Pキーが押されたらゲームに戻る
    if (InputManager::GetInstance().IsKeyPressed('P'))
    {
        SceneManager::GetInstance().ChangeScene(new GameScene(m_currentStage));
        return;
    }

    // Enterキーが押されたらゲームに戻る
    if (InputManager::GetInstance().IsKeyPressed(VK_RETURN))
    {
        SceneManager::GetInstance().ChangeScene(new GameScene(m_currentStage));
        return;
    }

    // マウス左クリックでゲームに戻る
    if (InputManager::GetInstance().IsMouseLeftPressed())
    {
        SceneManager::GetInstance().ChangeScene(new GameScene(m_currentStage));
        return;
    }

    // PRESS CLICKの点滅
    m_blinkTimer++;
    if (m_blinkTimer >= 40)
    {
        m_blinkVisible = !m_blinkVisible;
        m_blinkTimer = 0;
    }
}

void PauseScene::Draw()
{
    // --- 背景タイル描画 ---
    const float CHIP_SIZE = 64.0f;
    const float chipshiftdown = 24.0f;
    Math::Rectangle srcRect = { 0, 0, 64, 64 };

    for (int y = 0; y < MAP_H; y++)
    {
        for (int x = 0; x < MAP_W; x++)
        {
            float posX = SCREEN_LEFT + (x * CHIP_SIZE) + (CHIP_SIZE / 2.0f);
            float posY = SCREEN_TOP - (y * CHIP_SIZE) - (CHIP_SIZE / 2.0f) + chipshiftdown;

            Math::Matrix mat = Math::Matrix::CreateTranslation(posX, posY, 0.0f);
            SHADER.m_spriteShader.SetMatrix(mat);

            if (m_backgroundMap[y][x] == 1)
                SHADER.m_spriteShader.DrawTex(&m_wallTex, 0, 0, &srcRect);
            else if (m_backgroundMap[y][x] == 5)
                SHADER.m_spriteShader.DrawTex(&m_breakableWallTex, 0, 0, &srcRect);
            else if (m_backgroundMap[y][x] == 6)
                SHADER.m_spriteShader.DrawTex(&m_holeTex, 0, 0, &srcRect);
            else
                SHADER.m_spriteShader.DrawTex(&m_dirtTex, 0, 0, &srcRect);
        }
    }

    // ポーズロゴ
    {
        Math::Rectangle logoRect = { 0, 0, 842, 150 };
        Math::Matrix worldMat = Math::Matrix::CreateTranslation(0.0f, 100.0f, 0);
        SHADER.m_spriteShader.SetMatrix(worldMat);
        SHADER.m_spriteShader.DrawTex(&m_pauseTex, 0, 0, &logoRect);
    }

    // PRESS CLICK（点滅）
    if (m_blinkVisible)
    {
        Math::Rectangle btnRect = { 0, 0, 512, 128 };
        Math::Matrix worldMat = Math::Matrix::CreateTranslation(0.0f, -150.0f, 0);
        SHADER.m_spriteShader.SetMatrix(worldMat);
        SHADER.m_spriteShader.DrawTex(&m_resumeTexture, 0, 0, &btnRect);
    }
}
