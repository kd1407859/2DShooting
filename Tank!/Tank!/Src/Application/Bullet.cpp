#include "Bullet.h"

Bullet::Bullet(float x, float y, float angle, KdTexture* tex, bool _isEnemy)
{
	pos = { x, y };
	m_pTex = tex;
	m_drawAngle = angle;

	isEnemy = _isEnemy;


	float rad = DirectX::XMConvertToRadians(angle);
	float speed = isEnemy ? 5.0f : 5.0f;

	m_moveX = -sin(rad) * speed;
	m_moveY = cos(rad) * speed;
}

void Bullet::Update()
{

	pos.x += m_moveX;
	pos.y += m_moveY;

	// =====================================
	// X軸（左右の画面端）の反射判定
	// =====================================
	if (pos.x < SCREEN_LEFT || pos.x > SCREEN_RIGHT)
	{
		if (m_bounceCount < MAX_BOUNCE)
		{
			m_moveX = -m_moveX;
			m_bounceCount++;

			if (pos.x < SCREEN_LEFT) pos.x = SCREEN_LEFT;
			if (pos.x > SCREEN_RIGHT) pos.x = SCREEN_RIGHT;
		}
		else
		{
			// 制限回数を超えたら消滅
			isDead = true;
		}
	}

	// =====================================
	// Y軸（上下の画面端）の反射判定
	// =====================================
	if (pos.y < SCREEN_BOTTOM || pos.y > SCREEN_TOP)
	{
		if (m_bounceCount < MAX_BOUNCE)
		{
			m_moveY = -m_moveY;
			m_bounceCount++;

			if (pos.y < SCREEN_BOTTOM) pos.y = SCREEN_BOTTOM;
			if (pos.y > SCREEN_TOP) pos.y = SCREEN_TOP;
		}
		else
		{
			// 制限回数を超えたら消滅
			isDead = true;
		}
	}

}

void Bullet::OnHitWall(bool isHitX)
{
	if (m_bounceCount < MAX_BOUNCE)
	{
		// isHitXが true なら横の反射、false なら上下の反射
		if (isHitX) m_moveX = -m_moveX;
		else        m_moveY = -m_moveY;

		m_bounceCount++; // 反射回数を増やす
	}
	else
	{
		isDead = true; // 制限回数を超えたら消滅させる
	}
}

void Bullet::Draw()
{
	if (m_pTex == nullptr) return;
	Math::Rectangle srcRect = { 0, 0, 16, 16 };

	// 弾を回転させて描画
	Math::Matrix rotMat = Math::Matrix::CreateRotationZ(DirectX::XMConvertToRadians(m_drawAngle));
	Math::Matrix transMat = Math::Matrix::CreateTranslation(pos.x, pos.y, 0);
	Math::Matrix worldMat = rotMat * transMat;

	SHADER.m_spriteShader.SetMatrix(worldMat);
	SHADER.m_spriteShader.DrawTex(m_pTex, 0, 0, 16, 16, &srcRect);
}
