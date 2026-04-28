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

	Enemy(float x, float y, KdTexture* tex, EnemyType type, std::shared_ptr<Player> player);
	

	void Update() override;

	void Draw() override;
};