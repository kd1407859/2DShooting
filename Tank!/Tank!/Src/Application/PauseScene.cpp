#include "PauseScene.h"
#include "SceneManager.h"
#include "GameScene.h"
#include "TitleScene.h"
#include "InputManager.h"
#include "main.h"

// number.bmp の1桁あたりのサイズ（画像サイズ279x34, 10桁）
static const int DIGIT_W = 28; // 279 / 10 ≈ 28
static const int DIGIT_H = 34;

PauseScene::PauseScene(int stageNum, int brownCount, int ashCount)
{
    m_currentStage = stageNum;
    m_brownCount = brownCount;
    m_ashCount = ashCount;
}

PauseScene::~PauseScene()
{
}

void PauseScene::Init()
{
    m_pauseTex.Load("Texture/Font/pause.png");
    m_stageTex.Load("Texture/Font/stage.png");
    m_numberTex.Load("Texture/Font/number.bmp");
    m_resumeTex.Load("Texture/Font/resume.png");
    m_toTitleTex.Load("Texture/Font/to_title.png");
    m_dirtTex.Load("Texture/Map/dirt.png");
    m_wallTex.Load("Texture/Map/crateMetal.png");
    m_breakableWallTex.Load("Texture/Map/crateWood.png");
    m_holeTex.Load("Texture/Map/hole.png");
    m_enemyTexBrown.Load("Texture/Enemy/enemy_brown.png");
    m_enemyTexAsh.Load("Texture/Enemy/enemy_ash.png");

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
                if (line[x] == '1') m_backgroundMap[y][x] = 1;
                else if (line[x] == '5') m_backgroundMap[y][x] = 5;
                else if (line[x] == '6') m_backgroundMap[y][x] = 6;
                else                     m_backgroundMap[y][x] = 0;
            }
            y++;
        }
        file.close();
    }

    m_blinkTimer = 0;
    m_blinkVisible = true;
    m_selectIndex = 0;
}

// ========================================
// 数字描画ヘルパー
// value の各桁を number.bmp から描画する
// cx: 数字全体の中心X  cy: 中心Y  scale: 拡大率
// ========================================
void PauseScene::DrawNumber(int value, float cx, float cy, float scale)
{
    std::string str = std::to_string(value);
    int  digits = (int)str.size();
    float drawW = DIGIT_W * scale;
    float drawH = DIGIT_H * scale;
    float startX = cx - (digits * drawW) / 2.0f + drawW / 2.0f;

    for (int i = 0; i < digits; i++)
    {
        int d = str[i] - '0';
        Math::Rectangle srcRect = { d * DIGIT_W, 0, DIGIT_W, DIGIT_H };
        Math::Matrix mat = Math::Matrix::CreateTranslation(
            startX + i * drawW, cy, 0.0f);
        SHADER.m_spriteShader.SetMatrix(mat);
        SHADER.m_spriteShader.DrawTex(
            &m_numberTex, 0, 0, (int)drawW, (int)drawH, &srcRect);
    }
}

void PauseScene::Update()
{
    // 上下キーで選択切り替え
    bool upNow = InputManager::GetInstance().IsKeyPressed(VK_UP);
    bool downNow = InputManager::GetInstance().IsKeyPressed(VK_DOWN);

    if ((upNow && !m_prevUpKey) || (downNow && !m_prevDownKey))
        m_selectIndex = (m_selectIndex + 1) % 2;

    m_prevUpKey = upNow;
    m_prevDownKey = downNow;

    // 点滅
    m_blinkTimer++;
    if (m_blinkTimer >= 40)
    {
        m_blinkVisible = !m_blinkVisible;
        m_blinkTimer = 0;
    }

    // 決定（Enterキー）
    bool enterNow = InputManager::GetInstance().IsKeyPressed(VK_RETURN);
    if (enterNow && !m_prevEnterKey)
    {
        if (m_selectIndex == 0)
        {
            SceneManager::GetInstance().ChangeScene(new GameScene(m_currentStage));
            return;
        }
        else
        {
            SceneManager::GetInstance().ChangeScene(new TitleScene());
            return;
        }
    }
    m_prevEnterKey = enterNow;

    // Pキーでも再開
    bool pNow = InputManager::GetInstance().IsKeyPressed('P');
    if (pNow && !m_prevPKey)
    {
        SceneManager::GetInstance().ChangeScene(new GameScene(m_currentStage));
        return;
    }
    m_prevPKey = pNow;
}

void PauseScene::Draw()
{
    // ===== 背景タイル描画 =====
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

            if (m_backgroundMap[y][x] == 1) SHADER.m_spriteShader.DrawTex(&m_wallTex, 0, 0, &srcRect);
            else if (m_backgroundMap[y][x] == 5) SHADER.m_spriteShader.DrawTex(&m_breakableWallTex, 0, 0, &srcRect);
            else if (m_backgroundMap[y][x] == 6) SHADER.m_spriteShader.DrawTex(&m_holeTex, 0, 0, &srcRect);
            else                                  SHADER.m_spriteShader.DrawTex(&m_dirtTex, 0, 0, &srcRect);
        }
    }

    // ===== PAUSEロゴ（512x128） =====
    {
        Math::Rectangle rect = { 0, 0, 512, 128 };
        Math::Matrix mat = Math::Matrix::CreateTranslation(0.0f, 180.0f, 0);
        SHADER.m_spriteShader.SetMatrix(mat);
        SHADER.m_spriteShader.DrawTex(&m_pauseTex, 0, 0, &rect);
    }

    // ===== ステージ表示 =====
    // "Stage" ラベル（512x128 を半分サイズで）
    {
        Math::Rectangle rect = { 0, 0, 512, 128 };
        Math::Matrix mat = Math::Matrix::CreateTranslation(-80.0f, 60.0f, 0);
        SHADER.m_spriteShader.SetMatrix(mat);
        SHADER.m_spriteShader.DrawTex(&m_stageTex, 0, 0, 256, 64, &rect);
    }
    // ステージ番号（number.bmpから描画、scale=2.0で少し大きく）
    DrawNumber(m_currentStage, 120.0f, 60.0f, 2.0f);

    // ===== 敵残数（戦車アイコン + 数字） =====
    // 茶色戦車アイコン
    {
        Math::Rectangle rect = { 0, 0, 64, 64 };
        Math::Matrix mat = Math::Matrix::CreateTranslation(-120.0f, -20.0f, 0);
        SHADER.m_spriteShader.SetMatrix(mat);
        SHADER.m_spriteShader.DrawTex(&m_enemyTexBrown, 0, 0, 48, 48, &rect);
    }
    DrawNumber(m_brownCount, -40.0f, -20.0f, 1.5f);

    // 灰色戦車アイコン
    {
        Math::Rectangle rect = { 0, 0, 64, 64 };
        Math::Matrix mat = Math::Matrix::CreateTranslation(-120.0f, -80.0f, 0);
        SHADER.m_spriteShader.SetMatrix(mat);
        SHADER.m_spriteShader.DrawTex(&m_enemyTexAsh, 0, 0, 48, 48, &rect);
    }
    DrawNumber(m_ashCount, -40.0f, -80.0f, 1.5f);

    // ===== 再開ボタン =====
    {
        Math::Rectangle rect = { 0, 0, 256, 64 };
        Math::Matrix mat = Math::Matrix::CreateTranslation(0.0f, -150.0f, 0);
        SHADER.m_spriteShader.SetMatrix(mat);
        SHADER.m_spriteShader.DrawTex(&m_resumeTex, 0, 0, &rect);
    }

    // ===== タイトルに戻るボタン =====
    {
        Math::Rectangle rect = { 0, 0, 256, 64 };
        Math::Matrix mat = Math::Matrix::CreateTranslation(0.0f, -230.0f, 0);
        SHADER.m_spriteShader.SetMatrix(mat);
        SHADER.m_spriteShader.DrawTex(&m_toTitleTex, 0, 0, &rect);
    }

    // ===== 選択中インジケーター（点滅） =====
    if (m_blinkVisible)
    {
        // 選択中のボタンの左側に数字「▶」代わりとして数字「1」を使うか、
        // 別途インジケーター画像を用意する場合はここで DrawTex する
        // 暫定として選択中ボタンを少し拡大して描画（再描画）
        if (m_selectIndex == 0)
        {
            Math::Rectangle rect = { 0, 0, 256, 64 };
            Math::Matrix mat = Math::Matrix::CreateTranslation(0.0f, -150.0f, 0);
            SHADER.m_spriteShader.SetMatrix(mat);
            // 選択中は少し大きく描画してハイライト
            SHADER.m_spriteShader.DrawTex(&m_resumeTex, 0, 0, 270, 70, &rect);
        }
        else
        {
            Math::Rectangle rect = { 0, 0, 256, 64 };
            Math::Matrix mat = Math::Matrix::CreateTranslation(0.0f, -230.0f, 0);
            SHADER.m_spriteShader.SetMatrix(mat);
            SHADER.m_spriteShader.DrawTex(&m_toTitleTex, 0, 0, 270, 70, &rect);
        }
    }
}