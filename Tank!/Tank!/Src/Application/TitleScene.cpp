#include "TitleScene.h"
#include "GameScene.h"
#include "SceneManager.h"

void TitleScene::Init()
{
    m_dirtTex.Load("Texture/Map/dirt.png");
    m_wallTex.Load("Texture/Map/wall.png");
    m_titleTex.Load("Texture/Font/title_logo.png");
    m_pressEnterTex.Load("Texture/Font/press_click.png");
    m_enemyTexBrown.Load("Texture/Enemy/enemy_brown.png");
    m_enemyTexAsh.Load("Texture/Enemy/enemy_ash.png");

    m_tanks.clear();

    // タイトル用マップデータの読み込み
    std::ifstream file("Texture/Map/title.txt");
    if (file.is_open())
    {
        std::string line;
        int y = 0;
        while (std::getline(file, line) && y < MAP_H)
        {
            for (int x = 0; x < MAP_W && x < (int)line.length(); x++)
            {
                const float CHIP_SIZE = 64.0f;
                float posX = (x * CHIP_SIZE) - (SCREEN_WIDTH / 2.0f) + (CHIP_SIZE / 2.0f);
                float posY = (SCREEN_HEIGHT / 2.0f) - (y * CHIP_SIZE) - (CHIP_SIZE / 2.0f);

                if (line[x] == '1')
                {
                    m_titleMap[y][x] = 1;
                    m_walls.push_back(std::make_shared<Wall>(posX, posY, &m_wallTex));
                }
                else if (line[x] == '5')
                {
                    m_tanks.push_back(std::make_shared<Enemy>(
                        posX, posY, &m_enemyTexBrown, EnemyType::Brown, nullptr));
                }
                else if (line[x] == '6')
                {
                    m_tanks.push_back(std::make_shared<Enemy>(
                        posX, posY, &m_enemyTexAsh, EnemyType::Ash, nullptr));
                }
                else m_titleMap[y][x] = 0;
            }
            y++;
        }
        file.close();
    }

    m_logoY = 400.0f;
    m_blinkTimer = 0;
    m_blinkVisible = true;
}

void TitleScene::Update()
{
    // --- タイトルロゴのスライドイン ---
    if (m_logoY > m_logoTargetY)
    {
        m_logoY -= (m_logoY - m_logoTargetY) * 0.08f; // イージング
        if (m_logoY < m_logoTargetY + 0.5f)
            m_logoY = m_logoTargetY;
    }

    // --- PRESS ENTERの点滅 ---
    m_blinkTimer++;
    if (m_blinkTimer >= 40)
    {
        m_blinkVisible = !m_blinkVisible;
        m_blinkTimer = 0;
    }

    // 戦車の更新
    for (auto& tank : m_tanks)
    {
        tank->m_canSeePlayer = false;
        tank->Update();
    }

    // --- 戦車 vs 壁の当たり判定（GameSceneと同じ） ---
    for (auto& tank : m_tanks)
    {
        for (auto& wall : m_walls)
        {
            float radiusA = 24.0f;
            float radiusB = 32.0f;

            float dx = wall->pos.x - tank->pos.x;
            float dy = wall->pos.y - tank->pos.y;
            float dist = std::sqrt(dx * dx + dy * dy);

            if (dist > 0.0f && dist < (radiusA + radiusB))
            {
                float overlap = (radiusA + radiusB) - dist;
                float nx = dx / dist;
                float ny = dy / dist;
                tank->pos.x -= nx * overlap;
                tank->pos.y -= ny * overlap;
            }
        }
    }

    // --- 戦車同士の当たり判定（GameSceneと同じ） ---
    for (size_t i = 0; i < m_tanks.size(); i++)
    {
        for (size_t j = i + 1; j < m_tanks.size(); j++)
        {
            float dx = m_tanks[j]->pos.x - m_tanks[i]->pos.x;
            float dy = m_tanks[j]->pos.y - m_tanks[i]->pos.y;
            float dist = std::sqrt(dx * dx + dy * dy);

            float radiusA = 20.0f;
            float radiusB = 20.0f;

            if (dist > 0.0f && dist < (radiusA + radiusB))
            {
                float overlap = (radiusA + radiusB) - dist;
                float nx = dx / dist;
                float ny = dy / dist;

                m_tanks[i]->pos.x -= nx * (overlap * 0.5f);
                m_tanks[i]->pos.y -= ny * (overlap * 0.5f);
                m_tanks[j]->pos.x += nx * (overlap * 0.5f);
                m_tanks[j]->pos.y += ny * (overlap * 0.5f);
            }
        }
    }

    // --- ENTERでゲーム開始 ---
    if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
    {
        SceneManager::GetInstance().ChangeScene(new GameScene());
    }
}

void TitleScene::Draw()
{
    // --- タイル背景の描画 ---
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

            if (m_titleMap[y][x] == 1)
                SHADER.m_spriteShader.DrawTex(&m_wallTex, 0, 0, &srcRect);
            else
                SHADER.m_spriteShader.DrawTex(&m_dirtTex, 0, 0, &srcRect);
        }
    }


    // --- 背景の戦車 ---
    for (auto& tank : m_tanks)
    {
        tank->Draw();
    }

    // --- タイトルロゴ ---
    {
        Math::Rectangle srcRect = { 0, 0, 512, 128 }; // 画像サイズに合わせて変更
        Math::Matrix worldMat = Math::Matrix::CreateTranslation(0.0f, m_logoY, 0);
        SHADER.m_spriteShader.SetMatrix(worldMat);
        SHADER.m_spriteShader.DrawTex(&m_titleTex, 0, 0, &srcRect);
    }

    // --- PRESS ENTER（点滅） ---
    if (m_blinkVisible)
    {
        Math::Rectangle srcRect = { 0, 0, 512, 128 }; // 画像サイズに合わせて変更
        Math::Matrix worldMat = Math::Matrix::CreateTranslation(0.0f, -100.0f, 0);
        SHADER.m_spriteShader.SetMatrix(worldMat);
        SHADER.m_spriteShader.DrawTex(&m_pressEnterTex, 0, 0, &srcRect);
    }
}

void TitleScene::Release()
{
}

