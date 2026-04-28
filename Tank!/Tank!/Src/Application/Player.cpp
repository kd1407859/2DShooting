#include "Player.h"

Player::Player()
{
    m_tex.Load("Texture/Player/player.png");

    pos = { 0,0 };

    m_angle = 0.0f;

    success = false;
}

void Player::Update()
{
    float speed = 2.0f;

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

    // --- ② マウスの方向に向く処理 ---
    POINT mousePos;
    GetCursorPos(&mousePos);

    ScreenToClient(APP.m_window.GetWndHandle(), &mousePos);

    float worldMouseX = (float)mousePos.x - (SCREEN_WIDTH / 2.0f);
    float worldMouseY = (SCREEN_HEIGHT / 2.0f) - (float)mousePos.y;


    float dx = worldMouseX - pos.x;
    float dy = worldMouseY - pos.y;

    float rad = atan2(-dx, dy);

    m_angle = DirectX::XMConvertToDegrees(rad);

    const float BODY_SIZE = 32.0f;

    // 左端
    if (pos.x < SCREEN_LEFT + BODY_SIZE)  pos.x = SCREEN_LEFT + BODY_SIZE;

    // 右端
    if (pos.x > SCREEN_RIGHT - BODY_SIZE) pos.x = SCREEN_RIGHT - BODY_SIZE;

    // 下端
    if (pos.y < SCREEN_BOTTOM + BODY_SIZE) pos.y = SCREEN_BOTTOM + BODY_SIZE;

    // 上端
    if (pos.y > SCREEN_TOP - BODY_SIZE)    pos.y = SCREEN_TOP - BODY_SIZE;



    Math::Matrix worldMat = Math::Matrix::CreateRotationZ(DirectX::XMConvertToRadians(m_angle)) *
        Math::Matrix::CreateTranslation(pos.x, pos.y, 0);

    m_mat = worldMat;

}

void Player::Draw()
{
    // 自分の描画
    Math::Rectangle srcRect = { 0, 0, 64, 64 };

    SHADER.m_spriteShader.SetMatrix(m_mat);
    SHADER.m_spriteShader.DrawTex(&m_tex, 0, 0, 64, 64, &srcRect);
}
