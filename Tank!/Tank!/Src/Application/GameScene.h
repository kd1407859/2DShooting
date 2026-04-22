#pragma once
#include "Scene.h"
#include "GameObject.h"
#include "Player.h"
#include "Enemy.h"
#include "Bullet.h"
#include "Wall.h"


#include "SceneManager.h"

#include <vector>
#include <memory>
#include <string>
#include <algorithm>

class GameScene : public Scene
{
	std::shared_ptr<Player> m_player;
	std::vector<std::shared_ptr<GameObject>> objList;
	KdTexture m_bulletTex;
	KdTexture m_enemyTex;
	KdTexture m_wallTex;
	int m_shootTimer = 0;

	int m_spawnTimer = 0;

	int m_score = 0;

private:
	int m_currentStage; // ★追加：現在のステージ番号を記憶する変数
public:

	GameScene(int stageNum = 1)
	{
		m_currentStage = stageNum; // 受け取った番号を記憶！
	}

	void Init() override;
	
	void Update() override;

	void Draw() override;
	
};