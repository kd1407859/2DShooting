#pragma once
#include <cmath>
#include <DirectXMath.h>
#include "GameObject.h"
#include "main.h"

class Bullet : public GameObject
{
	KdTexture* m_pTex = nullptr;

	// 弾の移動量
	float m_moveX = 0.0f;
	float m_moveY = 0.0f;

	// 回転用（見た目用）
	float m_drawAngle = 0.0f;

	//弾の反射回数の管理
	int m_bounceCount = 0;
	const int MAX_BOUNCE = 1;

public:

	bool isEnemy = false;

	Math::Vector2 moveDir = {};

	// コンストラクタ
	Bullet(float x, float y, float angle, KdTexture* tex, bool _isEnemy)
	{
		pos = { x, y };
		m_pTex = tex;
		m_drawAngle = angle;

		isEnemy = _isEnemy;


		float rad = DirectX::XMConvertToRadians(angle);
		float speed = isEnemy ? 6.0f : 10.0f;

		m_moveX = -sin(rad) * speed;
		m_moveY = cos(rad) * speed;
	}

	void Update() override
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

				// 壁にめり込まないように位置を補正
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

				// 壁にめり込まないように位置を補正
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

	// 壁に当たった時の反射処理
	void OnHitWall(bool isHitX)
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

	void Draw() override
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
};