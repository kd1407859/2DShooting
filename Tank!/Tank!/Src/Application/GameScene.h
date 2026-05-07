#pragma once
#include "Scene.h"
#include "GameObject.h"
#include "Player.h"
#include "Enemy.h"
#include "Bullet.h"
#include "Wall.h"
#include "BreakableWall.h"
#include "Hole.h"

class GameScene : public Scene
{
	std::shared_ptr<Player> m_player;
	std::vector<std::shared_ptr<GameObject>> objList;
	static KdTexture m_bulletTex;
	static KdTexture m_enemyTexBrown; // 茶色の敵用
	static KdTexture m_enemyTexAsh;   // 灰色の敵用
	static KdTexture m_wallTex;
	static KdTexture m_dirtTex;
	static KdTexture m_holeTex;
	static KdTexture m_breakableWallTex;
	static KdTexture m_explosionTex;

	int m_shootTimer = 0;

	int m_spawnTimer = 0;

	int m_score = 0;

	bool m_isPaused = false;

	// ゲーム状態保存用（ポーズ時に保存）
	static std::vector<std::shared_ptr<GameObject>> s_savedObjList;
	static std::shared_ptr<Player> s_savedPlayer;
	static int s_savedScore;
	static int s_savedShootTimer;
	static int s_savedSpawnTimer;
	static bool s_hasGameState;

private:
	int m_currentStage;

	static const int MAX_STAGE = 3;
public:

	GameScene(int stageNum = 1);

	// ゲーム状態の保存
	void SaveGameState();

	// ゲーム状態の復元
	void RestoreGameState();

	void Init() override;

	void Update() override;

	void Draw() override;

};