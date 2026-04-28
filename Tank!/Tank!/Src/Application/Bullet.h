#pragma once
#include "GameObject.h"
#include "main.h"

class Bullet : public GameObject
{
	KdTexture* m_pTex = nullptr;

	// 弾の移動量
	float m_moveX = 0.0f;
	float m_moveY = 0.0f;

	// 回転用
	float m_drawAngle = 0.0f;

	//弾の反射回数の管理
	int m_bounceCount = 0;
	const int MAX_BOUNCE = 1;

public:

	bool isEnemy = false;

	Math::Vector2 moveDir = {};

	// コンストラクタ
	Bullet(float x, float y, float angle, KdTexture* tex, bool _isEnemy);
	

	void Update() override;
	

	// 壁に当たった時の反射処理
	void OnHitWall(bool isHitX);
	

	void Draw() override;
	
};