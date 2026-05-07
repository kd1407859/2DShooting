#include "GameOverScene.h"
#include "SceneManager.h"
#include "TitleScene.h"
#include "InputManager.h"
#include "main.h"

GameOverScene::GameOverScene(int failedStage)
{
    m_failedStage = failedStage;
}

GameOverScene::~GameOverScene()
{
}

void GameOverScene::Init()
{
    // ゲームオーバー画面の画像を読み込み
    m_gameOverTex.Load("Texture/Font/gameover.png");
    m_pressEnterTex.Load("Texture/Font/press_click.png");
    m_dirtTex.Load("Texture/Map/dirt.png");
    m_wallTex.Load("Texture/Map/crateMetal.png");

    // 背景マップを読み込み（タイトル画面と同じ）
    std::ifstream file("Texture/Map/title.txt");
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

void GameOverScene::Update()
{
    // Enterキーが押されたらタイトルへ戻る
    if (InputManager::GetInstance().IsKeyPressed(VK_RETURN))
    {
        SceneManager::GetInstance().ChangeScene(new TitleScene());
        return;
    }

    // マウス左クリックでタイトルに戻る
    if (InputManager::GetInstance().IsMouseLeftPressed())
    {
        SceneManager::GetInstance().ChangeScene(new TitleScene());
        return;
    }

    // PRESS ENTERの点滅
    m_blinkTimer++;
    if (m_blinkTimer >= 40)
    {
        m_blinkVisible = !m_blinkVisible;
        m_blinkTimer = 0;
    }
}

void GameOverScene::Draw()
{
    // --- 背景タイル描画 ---
    const float CHIP_SIZE = 64.0f;
    Math::Rectangle srcRect = { 0, 0, 64, 64 };

    for (int y = 0; y < MAP_H; y++)
    {
        for (int x = 0; x < MAP_W; x++)
        {
            float posX = (x * CHIP_SIZE) - (SCREEN_WIDTH / 2.0f) + (CHIP_SIZE / 2.0f);
            float posY = (SCREEN_HEIGHT / 2.0f) - (y * CHIP_SIZE) - (CHIP_SIZE / 2.0f);

            Math::Matrix mat = Math::Matrix::CreateTranslation(posX, posY, 0.0f);
            SHADER.m_spriteShader.SetMatrix(mat);

            if (m_backgroundMap[y][x] == 1)
                SHADER.m_spriteShader.DrawTex(&m_wallTex, 0, 0, &srcRect);
            else
                SHADER.m_spriteShader.DrawTex(&m_dirtTex, 0, 0, &srcRect);
        }
    }

    // ゲームオーバーロゴ
    {
        Math::Rectangle srcRect = { 0, 0, 842, 150 };
        Math::Matrix worldMat = Math::Matrix::CreateTranslation(0.0f, 100.0f, 0);
        SHADER.m_spriteShader.SetMatrix(worldMat);
        SHADER.m_spriteShader.DrawTex(&m_gameOverTex, 0, 0, &srcRect);
    }

    // PRESS ENTER（点滅）
    if (m_blinkVisible)
    {
        Math::Rectangle srcRect = { 0, 0, 512, 128 };
        Math::Matrix worldMat = Math::Matrix::CreateTranslation(0.0f, -150.0f, 0);
        SHADER.m_spriteShader.SetMatrix(worldMat);
        SHADER.m_spriteShader.DrawTex(&m_pressEnterTex, 0, 0, &srcRect);
    }

    // スコア表示
 /* std::string scoreStr = "FINAL SCORE: " + std::to_string(m_finalScore);
  SHADER.m_spriteShader.DrawString(-150, -50, scoreStr.c_str(), Math::Vector4(1, 0, 0, 1));*/
}
