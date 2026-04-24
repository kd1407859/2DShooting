#pragma once
#include "GameObject.h"
#include "main.h"
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
	bool m_canSeePlayer = false;

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

		wantToShoot = false;

		//AI(CPU)の動き
		if (m_targetPlayer && !m_targetPlayer->isDead)
		{
			//プレイヤーが見えている時
			if (m_canSeePlayer)
			{
				//① プレイヤーの方向を向く
				float dx = m_targetPlayer->pos.x - pos.x;
				float dy = m_targetPlayer->pos.y - pos.y;
				float radToPlayer = atan2(-dx, dy);
				m_angle = radToPlayer * (180.0f / 3.14159265f);

				//② 弾を撃つ(クールダウン処理と同時並行)
				m_shootCooldown--;
				if (m_shootCooldown <= 0) {
					wantToShoot = true;
					m_shootCooldown = 60 + (rand() % 60);
				}

				//③ 灰色（Ash）の場合はプレイヤーに向かって進む
				if (m_type == EnemyType::Ash) {
					float speed = 2.0f;
					pos.x += -sin(radToPlayer) * speed;
					pos.y += cos(radToPlayer) * speed;
				}
			}
			//プレイヤーが見えない時
			else
			{

				if (m_type == EnemyType::Ash) {
					m_moveTimer--;
					if (m_moveTimer <= 0) {
						//タイマーが切れたらランダムな方向を向く
						m_moveTimer = 60 + (rand() % 120);
						m_angle = (float)(rand() % 360);
					}

					// 向いている方向にただ前進する
					float rad = m_angle * (3.14159265f / 180.0f);
					float speed = 1.5f;
					pos.x += -sin(rad) * speed;
					pos.y += cos(rad) * speed;
				}
			}
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