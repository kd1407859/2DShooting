#include "GameScene.h"
#include "GameOverScene.h"
#include "GameObject.h"
#include "Player.h"
#include "Enemy.h"
#include "Wall.h"
#include "BreakableWall.h"
#include "Hole.h"
#include "Bullet.h"
#include "SceneManager.h"
#include "PauseScene.h"
#include "ClearScene.h"
#include "InputManager.h"
#include "main.h"

GameScene::GameScene(int stageNum)
{
	m_currentStage = stageNum;
}

void GameScene::Init()
{

	srand((unsigned int)time(NULL));

	m_bulletTex.Load("Texture/Player/bullet.png");
	m_enemyTexBrown.Load("Texture/Enemy/enemy_brown.png");
	m_enemyTexAsh.Load("Texture/Enemy/enemy_ash.png");
	m_wallTex.Load("Texture/Map/wall.png");
	m_dirtTex.Load("Texture/Map/dirt.png");
	m_breakableWallTex.Load("Texture/Map/breakwall.png");
	m_holeTex.Load("Texture/Map/hole.png");

	objList.clear();

	const int MAP_W = 20;
	const int MAP_H = 12;
	const float CHIP_SIZE = 64.0f;
	const float chipshiftdown = 24.0f;


	int stageData[MAP_H][MAP_W] = { 0 };

	std::string fileName = "Texture/Map/stage" + std::to_string(m_currentStage) + ".txt";


	std::ifstream file(fileName);


	if (file.is_open())
	{
		std::string line;
		int y = 0;


		while (std::getline(file, line) && y < MAP_H)
		{

			for (int x = 0; x < MAP_W && x < line.length(); x++)
			{

				if (line[x] == '1') stageData[y][x] = 1;
				else if (line[x] == '2') stageData[y][x] = 2;
				else if (line[x] == '3') stageData[y][x] = 3;
				else if (line[x] == '4') stageData[y][x] = 4; // �G�i�D�j
				else if (line[x] == '5') stageData[y][x] = 5; // �󂹂��
				else if (line[x] == '6') stageData[y][x] = 6; // ��
			}
			y++;
		}
		file.close();
	}
	else
	{
		SceneManager::GetInstance().ChangeScene(new GameOverScene(m_score));
		return;
	}


	m_player = std::make_shared<Player>();
	objList.push_back(m_player);


	for (int y = 0; y < MAP_H; y++) {
		for (int x = 0; x < MAP_W; x++) {


			float posX = SCREEN_LEFT + (x * CHIP_SIZE) + (CHIP_SIZE / 2.0f);
			float posY = SCREEN_TOP - (y * CHIP_SIZE) - (CHIP_SIZE / 2.0f) + chipshiftdown;

			if (stageData[y][x] == 1) {
				
				objList.push_back(std::make_shared<Wall>(posX, posY, &m_wallTex));
			}
			else if (stageData[y][x] == 2) {
			
				m_player->pos = { posX, posY };
			}
			else if (stageData[y][x] == 3) {
				
				objList.push_back(std::make_shared<Enemy>(posX, posY, &m_enemyTexBrown, EnemyType::Brown, m_player));
			}
			else if (stageData[y][x] == 4) {
				
				objList.push_back(std::make_shared<Enemy>(posX, posY, &m_enemyTexAsh, EnemyType::Ash, m_player));
			}
			else if (stageData[y][x] == 5) {

				objList.push_back(std::make_shared<BreakableWall>(posX, posY, &m_breakableWallTex));
			}
			else if (stageData[y][x] == 6) {
				
				objList.push_back(std::make_shared<Hole>(posX, posY, &m_holeTex));
			}
		}
	}

	m_shootTimer = 0;

	m_spawnTimer = 120;

	m_score = 0;
}

void GameScene::Update()
{
	// Pキーでポーズ画面に遷移
	if (InputManager::GetInstance().IsKeyPressed('P'))
	{
		SceneManager::GetInstance().ChangeScene(new PauseScene(m_currentStage));
		return;
	}

	m_shootTimer--;

	m_spawnTimer--;

	int playerBulletCount = 0;
	const int MAX_PLAYER_BULLETS = 5;
	for (auto& obj : objList) {

		if (auto bullet = std::dynamic_pointer_cast<Bullet>(obj)) {

			if (bullet->isEnemy == false && bullet->isDead == false) {
				playerBulletCount++;
			}
		}
	}

	
	if ((GetAsyncKeyState(VK_LBUTTON) & 0x8000) && m_shootTimer <= 0 && playerBulletCount < MAX_PLAYER_BULLETS)
	{
		if (m_shootTimer <= 0)
		{
			POINT mousePos;
			GetCursorPos(&mousePos);

			ScreenToClient(APP.m_window.GetWndHandle(), &mousePos);

			float gameMouseX = (float)mousePos.x - (SCREEN_WIDTH / 2.0f);
			float gameMouseY = -((float)mousePos.y - (SCREEN_HEIGHT / 2.0f));

			float dx = gameMouseX - m_player->pos.x;
			float dy = gameMouseY - m_player->pos.y;

			float rad = atan2(-dx, dy);


			float degree = rad * (180.0f / 3.14159265f);

			float offset = 34.0f;
			float bulletX = m_player->pos.x - sin(rad) * offset;
			float bulletY = m_player->pos.y + cos(rad) * offset;

			objList.push_back(std::make_shared<Bullet>(
				bulletX,
				bulletY,
				degree,
				&m_bulletTex,
				false
			));


			m_shootTimer = 30;
		}
	}

	for (auto& obj : objList) {
		auto enemy = std::dynamic_pointer_cast<Enemy>(obj);
		if (!enemy) continue;

		
		enemy->m_canSeePlayer = true;

		
		Math::Vector2 rayPos = enemy->pos;
		Math::Vector2 targetPos = m_player->pos;
		Math::Vector2 diff = targetPos - rayPos;
		float distToPlayer = diff.Length();
		diff.Normalize();

		
		float checkStep = 32.0f;
		float currentDist = checkStep;

		while (currentDist < distToPlayer) {
			
			Math::Vector2 checkPos = rayPos + diff * currentDist;

			bool blocked = false;

			
			for (auto& obstacleObj : objList) {
				auto wall = std::dynamic_pointer_cast<Wall>(obstacleObj);
				if (wall) {
					
					float dx = wall->pos.x - checkPos.x;
					float dy = wall->pos.y - checkPos.y;
					if (std::sqrt(dx * dx + dy * dy) < 30.0f) {
						
						enemy->m_canSeePlayer = false;
						blocked = true;
						break;
					}
				}

				auto breakableWall = std::dynamic_pointer_cast<BreakableWall>(obstacleObj);
				if (breakableWall && !breakableWall->isDead) {
					
					float dx = breakableWall->pos.x - checkPos.x;
					float dy = breakableWall->pos.y - checkPos.y;
					if (std::sqrt(dx * dx + dy * dy) < 30.0f) {
						
						enemy->m_canSeePlayer = false;
						blocked = true;
						break;
					}
				}
			}

			if (blocked) break; 
			currentDist += checkStep;
		}
	}

	std::vector<std::shared_ptr<GameObject>> newObjects;

	for (auto& obj : objList)
	{
		obj->Update();

		if (auto enemy = std::dynamic_pointer_cast<Enemy>(obj))
		{

			if (enemy->wantToShoot)
			{
				float enemyRad = enemy->m_angle * (3.14159265f / 180.0f);
				float offset = 34.0f;

				float bulletX = enemy->pos.x - sin(enemyRad) * offset;
				float bulletY = enemy->pos.y + cos(enemyRad) * offset;

				std::shared_ptr<Bullet> bullet = std::make_shared<Bullet>(
					bulletX, bulletY, enemy->m_angle, &m_bulletTex, true
				);

				newObjects.push_back(bullet);

				enemy->wantToShoot = false;
			}
		}
	}

	for (auto& newObj : newObjects)
	{
		objList.push_back(newObj);
	}

	for (auto& obj1 : objList) {

		std::shared_ptr<Bullet> bullet = std::dynamic_pointer_cast<Bullet>(obj1);
		if (!bullet) continue;

		for (auto& obj2 : objList) {

			std::shared_ptr<Wall> wall = std::dynamic_pointer_cast<Wall>(obj2);
			if (!wall) continue;


			float dx = abs(bullet->pos.x - wall->pos.x);
			float dy = abs(bullet->pos.y - wall->pos.y);


			float hitRange = 40.0f;

			if (dx < hitRange && dy < hitRange) {

				
				if (dx > dy) {
				
					bullet->OnHitWall(true);

					
					if (bullet->pos.x < wall->pos.x) bullet->pos.x -= (hitRange - dx);
					else                             bullet->pos.x += (hitRange - dx);
				}
				else {
				
					bullet->OnHitWall(false);

					if (bullet->pos.y < wall->pos.y) bullet->pos.y -= (hitRange - dy);
					else                             bullet->pos.y += (hitRange - dy);
				}

				break;
			}
		}
	}

	for (auto& objA : objList) {
		for (auto& objB : objList) {
			if (objA == objB) continue;


			bool isTankA = std::dynamic_pointer_cast<Player>(objA) || std::dynamic_pointer_cast<Enemy>(objA);

			bool isObstacleB = std::dynamic_pointer_cast<Wall>(objB) ||
				std::dynamic_pointer_cast<BreakableWall>(objB) ||
				std::dynamic_pointer_cast<Hole>(objB);

			if (isTankA && isObstacleB) {
				float radiusA = 24.0f;
				float radiusB = 32.0f; 

				float dx = objB->pos.x - objA->pos.x;
				float dy = objB->pos.y - objA->pos.y;
				float dist = std::sqrt(dx * dx + dy * dy);


				if (dist > 0.0f && dist < (radiusA + radiusB)) {
					float overlap = (radiusA + radiusB) - dist;
					float nx = dx / dist;
					float ny = dy / dist;
					objA->pos.x -= nx * overlap;
					objA->pos.y -= ny * overlap;
				}
			}


			if (auto bullet = std::dynamic_pointer_cast<Bullet>(objA)) {


				if (auto bWall = std::dynamic_pointer_cast<BreakableWall>(objB)) {
					float dx = std::abs(bullet->pos.x - bWall->pos.x);
					float dy = std::abs(bullet->pos.y - bWall->pos.y);
					float hitRange = 40.0f;

					if (dx < hitRange && dy < hitRange) {
						bullet->isDead = true;
						bWall->isDead = true;
					}
				}

			}
		}
	}


	for (auto& obj : objList) {

		bool isObstacle = std::dynamic_pointer_cast<Wall>(obj) ||
			std::dynamic_pointer_cast<BreakableWall>(obj) ||
			std::dynamic_pointer_cast<Hole>(obj);

		if (isObstacle) {

		}
	}


	for (auto& objA : objList)
	{
		for (auto& objB : objList)
		{
			if (objA == objB) continue;
			if (objA->isDead || objB->isDead) continue;

			auto bullet = std::dynamic_pointer_cast<Bullet>(objA);

			if (bullet)
			{
				if (bullet->isEnemy == false)
				{
					auto enemy = std::dynamic_pointer_cast<Enemy>(objB);
					if (enemy)
					{
						if ((bullet->pos - enemy->pos).Length() < 50.0f) {
							bullet->isDead = true;
							enemy->isDead = true;

							m_score += 100;
						}
					}
				}

				if (bullet->isEnemy == true)
				{
					auto player = std::dynamic_pointer_cast<Player>(objB);
					if (player)
					{
						if ((bullet->pos - player->pos).Length() < 30.0f) {
							bullet->isDead = true;
							player->isDead = true;

							SceneManager::GetInstance().ChangeScene(new GameOverScene(m_score));
							return;
						}
					}
				}
			}
		}
	}


	for (size_t i = 0; i < objList.size(); i++) {
		for (size_t j = i + 1; j < objList.size(); j++) {
			auto objA = objList[i];
			auto objB = objList[j];


			if (objA->isDead || objB->isDead) continue;


			bool isTankA = std::dynamic_pointer_cast<Player>(objA) || std::dynamic_pointer_cast<Enemy>(objA);
			bool isTankB = std::dynamic_pointer_cast<Player>(objB) || std::dynamic_pointer_cast<Enemy>(objB);

			if (isTankA && isTankB) {

				float dx = objB->pos.x - objA->pos.x;
				float dy = objB->pos.y - objA->pos.y;
				float dist = std::sqrt(dx * dx + dy * dy);


				float radiusA = 20.0f;
				float radiusB = 20.0f;

				if (dist > 0.0f && dist < (radiusA + radiusB)) {


					float overlap = (radiusA + radiusB) - dist;


					float nx = dx / dist;
					float ny = dy / dist;

					objA->pos.x -= nx * (overlap * 0.5f);
					objA->pos.y -= ny * (overlap * 0.5f);

					objB->pos.x += nx * (overlap * 0.5f);
					objB->pos.y += ny * (overlap * 0.5f);
				}
			}
		}
	}


	for (auto& obj1 : objList) {
		auto bullet = std::dynamic_pointer_cast<Bullet>(obj1);
		if (!bullet) continue;

		for (auto& obj2 : objList) {
			if (obj1 == obj2) continue;
			if (obj2->isDead) continue;


			if (auto bWall = std::dynamic_pointer_cast<BreakableWall>(obj2)) {
				float dx = abs(bullet->pos.x - bWall->pos.x);
				float dy = abs(bullet->pos.y - bWall->pos.y);
				float hitRange = 40.0f;
				if (dx < hitRange && dy < hitRange) {
					bullet->isDead = true;
					bWall->isDead = true;
					break;
				}
			}


		}
	}

	auto it = std::remove_if(objList.begin(), objList.end(), [](std::shared_ptr<GameObject> obj) {
		return obj->isDead;
		});
	objList.erase(it, objList.end());


	int enemyCount = 0;

	for (auto& obj : objList) {
		if (std::dynamic_pointer_cast<Enemy>(obj)) {
			enemyCount++;
		}
	}

	if (enemyCount == 0) {
		SceneManager::GetInstance().ChangeScene(new GameScene(m_currentStage + 1));
		return;
	}

}

void GameScene::Draw()
{
	Math::Rectangle srcRect = { 0,0,64,64 };

	for (int y = 0; y < 12; ++y)
	{
		for (int x = 0; x < 20; ++x)
		{
			float posX = (x * 64.0f) - (SCREEN_WIDTH / 2.0f) + 32.0f;
			float posY = (SCREEN_HEIGHT / 2.0f) - (y * 64.0f) - 32.0f;

			Math::Matrix mat = Math::Matrix::CreateTranslation(posX, posY, 0.0f);
			SHADER.m_spriteShader.SetMatrix(mat);
			SHADER.m_spriteShader.DrawTex(&m_dirtTex, 0, 0, &srcRect);
		}
	}


	std::shared_ptr<Player> playerPtr = nullptr;
	std::vector<std::shared_ptr<Enemy>> enemyPtrs;

	for (auto& obj : objList)
	{
		auto player = std::dynamic_pointer_cast<Player>(obj);
		if (player) {
			playerPtr = player;
			continue;
		}

		auto enemy = std::dynamic_pointer_cast<Enemy>(obj);
		if (enemy) {
			enemyPtrs.push_back(enemy);
			continue;
		}


		obj->Draw();
	}


	if (playerPtr) {
		playerPtr->Draw();
	}

	for (auto& enemy : enemyPtrs) {
		enemy->Draw();
	}

	std::string scoreStr = "SCORE: " + std::to_string(m_score);

	SHADER.m_spriteShader.DrawString(-600, 300, scoreStr.c_str(), Math::Vector4(1, 1, 1, 1));
}
