#pragma once
#include "GameObject.h"
#include "main.h"
#include <cstdlib>
#include <cmath>
#include<memory>
#include "Player.h"


enum class EnemyType
{
	Brown,
	Ash
};

class Enemy : public GameObject
{
	KdTexture* m_pTex = nullptr;


	int m_moveTimer = 0;

	int m_shootCooldown = 0;

	EnemyType m_type;
	std::shared_ptr<Player> m_targetPlayer;

public:

	float m_angle = 180.0f;

	bool wantToShoot = false;

	Enemy(float x, float y, KdTexture* tex, EnemyType type, std::shared_ptr<Player> player)
	{
		pos = { x,y };
		m_pTex = tex;
		m_type = type;            // 受け取った種類を記憶
		m_targetPlayer = player;  // 受け取ったプレイヤー情報を記憶

		m_moveTimer = 60;
		m_shootCooldown = 60;
	}

	void Update() override {

		//AI(CPU)の動き
		// ==========================================
		// ① プレイヤーの方向を向く（エイム処理）
		// ==========================================
		if (m_targetPlayer && !m_targetPlayer->isDead)
		{
			// プレイヤーとの距離（XとY）を計算
			float dx = m_targetPlayer->pos.x - pos.x;
			float dy = m_targetPlayer->pos.y - pos.y;

			m_angle = DirectX::XMConvertToDegrees(atan2(-dx, dy));
		}

		// ==========================================
		// ② 敵の種類ごとの移動処理
		// ==========================================
		if (m_type == EnemyType::Ash)
		{
			// 灰色（Ash）は今まで通りウロウロ動く
			m_moveTimer--;
			if (m_moveTimer <= 0)
			{
				m_moveTimer = 60 + (rand() % 120);
			}

			// 前に進む
			float rad = DirectX::XMConvertToRadians(m_angle);
			float speed = 2.0f;
			pos.x += -sin(rad) * speed;
			pos.y += cos(rad) * speed;
		}
		else if (m_type == EnemyType::Brown)
		{
			
		}


	//画面端判定
		if (pos.x < SCREEN_LEFT + 50) {
			pos.x = SCREEN_LEFT + 50;
			m_angle = 90.0f; // 左端
		}
		if (pos.x > SCREEN_RIGHT - 50) {
			pos.x = SCREEN_RIGHT - 50;
			m_angle = 270.0f; // 右端
		}
		if (pos.y < SCREEN_BOTTOM + 50) {
			pos.y = SCREEN_BOTTOM + 50;
			m_angle = 0.0f;   // 上端
		}
		if (pos.y > SCREEN_TOP - 50) {
			pos.y = SCREEN_TOP - 50;
			m_angle = 180.0f; // 下端
		}

		// ==========================================
		// ③ 弾を撃つ（クールダウン）処理
		// ==========================================
		m_shootCooldown--;
		if (m_shootCooldown <= 0)
		{
			wantToShoot = true;
			m_shootCooldown = 120;
		}

	}

	void Draw() override {
		if (m_pTex == nullptr) return;
		Math::Rectangle srcRect = { 0,0,64,64 };


		Math::Matrix rotMat = Math::Matrix::CreateRotationZ(DirectX::XMConvertToRadians(m_angle));
		Math::Matrix transMat = Math::Matrix::CreateTranslation(pos.x, pos.y, 0);
		Math::Matrix worldMat = rotMat * transMat;

		SHADER.m_spriteShader.SetMatrix(worldMat);
		SHADER.m_spriteShader.DrawTex(m_pTex, 0, 0, &srcRect);
	}
};