#pragma once
#include "Scene.h"
#include "GameObject.h"
#include "Player.h"
#include "Enemy.h"
#include "Bullet.h"
#include "Wall.h"
class GameScene : public Scene
{
	std::shared_ptr<Player> m_player;
	std::vector<std::shared_ptr<GameObject>> objList;
	KdTexture m_bulletTex;
	KdTexture m_enemyTexBrown; // 茶色の敵用
	KdTexture m_enemyTexAsh;   // 灰色の敵用
	KdTexture m_wallTex;
	KdTexture m_dirtTex;

	int m_shootTimer = 0;

	int m_spawnTimer = 0;

	int m_score = 0;

private:
	int m_currentStage;
public:

	GameScene(int stageNum = 1)
	{
		m_currentStage = stageNum;
	}

	void Init() override;
	
	void Update() override;

	void Draw() override;
	
};