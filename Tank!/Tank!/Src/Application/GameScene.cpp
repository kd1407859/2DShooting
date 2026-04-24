#include "GameScene.h"
#include "GameOverScene.h"
#include "SceneManager.h"
#include <ctime>

#include<fstream>
#include<string>

void GameScene::Init()
{
	
	srand((unsigned int)time(NULL));

	m_bulletTex.Load("Texture/Player/bullet.png");
	m_enemyTexBrown.Load("Texture/Enemy/enemy_brown.png");
	m_enemyTexAsh.Load("Texture/Enemy/enemy_ash.png");
	m_wallTex.Load("Texture/Map/wall.png");
	m_dirtTex.Load("Texture/Map/dirt.png");

	objList.clear();

	const int MAP_W = 20;
	const int MAP_H = 12;
	const float CHIP_SIZE = 64.0f;
	const float chipshiftdown = 24.0f;

	// ① まず配列を全部 0（空白）で作っておく
	int stageData[MAP_H][MAP_W] = { 0 };

	std::string fileName = "Texture/Map/stage" + std::to_string(m_currentStage) + ".txt";

	// ② テキストファイルを開く
	std::ifstream file(fileName);

	// ③ ファイルがちゃんと開けた場合のみ読み込む
	if (file.is_open())
	{
		std::string line;
		int y = 0;

		// ファイルから1行ずつ文字を読み込む
		while (std::getline(file, line) && y < MAP_H)
		{
			// 1行の中の文字を左から順番にチェック
			for (int x = 0; x < MAP_W && x < line.length(); x++)
			{
				// 文字に応じて配列に数字を入れる
				if (line[x] == '1') stageData[y][x] = 1; // 壁
				else if (line[x] == '2') stageData[y][x] = 2; // プレイヤー
				else if (line[x] == '3') stageData[y][x] = 3; // 敵（茶）
				else if (line[x] == '4') stageData[y][x] = 4; // 敵（灰）
				// '0' の場合はそのまま（配列の初期値が0なので）
			}
			y++; // 次の行へ
		}
		file.close(); // 読み終わったらファイルを閉じる
	}
	else
	{
		SceneManager::GetInstance().ChangeScene(new GameOverScene(m_score));
		return;
	}


	m_player = std::make_shared<Player>();
	objList.push_back(m_player);

	// 二次元配列をループして、壁・プレイヤー・敵を配置
	for (int y = 0; y < MAP_H; y++) {
		for (int x = 0; x < MAP_W; x++) {

			// 配列のインデックスから画面座標(x, y)を計算
			float posX = SCREEN_LEFT + (x * CHIP_SIZE) + (CHIP_SIZE / 2.0f);
			float posY = SCREEN_TOP - (y * CHIP_SIZE) - (CHIP_SIZE / 2.0f) + chipshiftdown;

			if (stageData[y][x] == 1) {
				// 1: 壁の生成
				objList.push_back(std::make_shared<Wall>(posX, posY, &m_wallTex));
			}
			else if (stageData[y][x] == 2) {
				// 2: プレイヤーの初期位置をマップデータに合わせて移動させる
				m_player->pos = { posX, posY };
			}
			else if (stageData[y][x] == 3) {
				// 3: 茶色戦車の生成（動かない）
				objList.push_back(std::make_shared<Enemy>(posX, posY, &m_enemyTexBrown, EnemyType::Brown, m_player));
			}
			else if (stageData[y][x] == 4) {
				// 4: 灰色戦車の生成（動く）
				objList.push_back(std::make_shared<Enemy>(posX, posY, &m_enemyTexAsh, EnemyType::Ash, m_player));
			}
		}
	}

	m_shootTimer = 0;

	m_spawnTimer = 120;

	m_score = 0;
}

void GameScene::Update()
{
	m_shootTimer--;

	m_spawnTimer--;

	int playerBulletCount = 0;
	const int MAX_PLAYER_BULLETS = 5; // ★同時に出せる弾の上限（お好みで調整してください）

	for (auto& obj : objList) {
		// オブジェクトが「弾 (Bullet)」であるかチェック
		if (auto bullet = std::dynamic_pointer_cast<Bullet>(obj)) {
			// それが「敵の弾ではない（＝プレイヤーの弾）」かつ「まだ消滅していない」ならカウント
			if (bullet->isEnemy == false && bullet->isDead == false) {
				playerBulletCount++;
			}
		}
	}

	// --- プレイヤーの発射 ---
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

	// クールダウンの減少処理
	if (m_shootTimer > 0) {
		m_shootTimer--;
	}

	//敵の視線判定(レイキャスト)
	for (auto& obj : objList) {
		auto enemy = std::dynamic_pointer_cast<Enemy>(obj);
		if (!enemy) continue;

		// --- 視線判定（Line of Sight） ---
		// 基本は見えていると仮定する
		enemy->m_canSeePlayer = true;

		// 敵からプレイヤーへのベクトル
		Math::Vector2 rayPos = enemy->pos;
		Math::Vector2 targetPos = m_player->pos;
		Math::Vector2 diff = targetPos - rayPos;
		float distToPlayer = diff.Length();
		diff.Normalize();

		// 敵からプレイヤーに向かって、少しずつ（例えば32ピクセルずつ）進んで壁に当たるか調べる
		float checkStep = 32.0f;
		float currentDist = checkStep;

		while (currentDist < distToPlayer) {
			// チェック地点を計算
			Math::Vector2 checkPos = rayPos + diff * currentDist;

			// すべてのオブジェクトの中から壁（Wall）を探して衝突判定
			for (auto& wallObj : objList) {
				auto wall = std::dynamic_pointer_cast<Wall>(wallObj);
				if (!wall) continue;

				// 壁との距離をチェック（壁のサイズ64に対して、32以内なら接触とみなす）
				float dx = wall->pos.x - checkPos.x;
				float dy = wall->pos.y - checkPos.y;
				if (std::sqrt(dx * dx + dy * dy) < 30.0f) {
					// 壁に遮られた！
					enemy->m_canSeePlayer = false;
					break;
				}
			}

			if (!enemy->m_canSeePlayer) break; // すでに見えないならループ抜ける
			currentDist += checkStep;
		}
	}

	// --- 全オブジェクト更新 & 敵の発射チェック ---
	std::vector<std::shared_ptr<GameObject>> newObjects;

	// 全オブジェクトの更新
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


	

	// すべてのオブジェクトの中から「弾」と「壁」のペアを探して当たり判定
	for (auto& obj1 : objList) {
		// dynamic_pointer_cast で obj1 が「Bullet(弾)」かチェック
		std::shared_ptr<Bullet> bullet = std::dynamic_pointer_cast<Bullet>(obj1);
		if (!bullet) continue; // 弾じゃなければ次のオブジェクトへ

		for (auto& obj2 : objList) {
			// obj2 が「Wall(壁)」かチェック
			std::shared_ptr<Wall> wall = std::dynamic_pointer_cast<Wall>(obj2);
			if (!wall) continue; // 壁じゃなければ次のオブジェクトへ

			// 弾と壁の中心座標の距離（絶対値）を計算
			float dx = abs(bullet->pos.x - wall->pos.x);
			float dy = abs(bullet->pos.y - wall->pos.y);

			// 壁のサイズは64x64(半分で32)、弾のサイズを仮に16x16(半分で8)とします。
			// 当たり判定の範囲は 32 + 8 = 40.0f
			float hitRange = 40.0f;

			if (dx < hitRange && dy < hitRange) {

				// 横からぶつかったか、上下からぶつかったか（めり込み具合）を比較
				if (dx > dy) {
					// 横からぶつかった場合：X方向の移動を反転させる
					bullet->OnHitWall(true);

					// ★追加：壁の中にめり込んだ分だけ、強引に外へ押し出す！
					if (bullet->pos.x < wall->pos.x) bullet->pos.x -= (hitRange - dx);
					else                             bullet->pos.x += (hitRange - dx);
				}
				else {
					// 上下からぶつかった場合：Y方向の移動を反転させる
					bullet->OnHitWall(false);

					if (bullet->pos.y < wall->pos.y) bullet->pos.y -= (hitRange - dy);
					else                             bullet->pos.y += (hitRange - dy);
				}

				break;
			}
		}
	}

	// --- キャラクター（プレイヤー・敵）と壁の当たり判定（押し出し） ---
	for (auto& obj1 : objList) {
		// obj1 が「Wall(壁)」かチェック
		std::shared_ptr<Wall> wall = std::dynamic_pointer_cast<Wall>(obj1);
		if (!wall) continue; // 壁じゃなければ次のオブジェクトへ

		for (auto& obj2 : objList) {
			if (obj1 == obj2) continue; // 自分自身との判定はスキップ

			// プレイヤーか敵かを確認するためのポインタ
			std::shared_ptr<Player> player = std::dynamic_pointer_cast<Player>(obj2);
			std::shared_ptr<Enemy>  enemy = std::dynamic_pointer_cast<Enemy>(obj2);

			if (!player && !enemy) continue; // どちらでもなければ次のオブジェクトへ

			// 当たり判定を行う対象の「座標ポインタ」を用意
			Math::Vector2* targetPos = nullptr;

			//当たり判定範囲
			float hitRange = 64.0f;

			if (player) targetPos = &player->pos;
			if (enemy)  targetPos = &enemy->pos;

			if (targetPos == nullptr) continue;

			// 対象と壁の中心座標の距離（絶対値）を計算
			float dx = abs(targetPos->x - wall->pos.x);
			float dy = abs(targetPos->y - wall->pos.y);

			// X軸とY軸の距離がどちらも hitRange 未満なら「当たっている（めり込んでいる）」
			if (dx < hitRange && dy < hitRange) {

				// 横からぶつかったか、上下からぶつかったか（めり込み具合）を比較
				if (dx > dy) {
					// 横方向の押し出し
					if (targetPos->x < wall->pos.x) targetPos->x -= (hitRange - dx);
					else                            targetPos->x += (hitRange - dx);
				}
				else {
					// 上下方向の押し出し
					if (targetPos->y < wall->pos.y) targetPos->y -= (hitRange - dy);
					else                            targetPos->y += (hitRange - dy);
				}
			}
		}
	}

	//自機と敵の弾の当たり判定
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

	//自機と敵の当たり判定
	for (size_t i = 0; i < objList.size(); i++) {
		for (size_t j = i + 1; j < objList.size(); j++) {
			auto objA = objList[i];
			auto objB = objList[j];

			// お互いが生きているかチェック
			if (objA->isDead || objB->isDead) continue;

			// 対象が「プレイヤー」か「敵」かを判定
			bool isTankA = std::dynamic_pointer_cast<Player>(objA) || std::dynamic_pointer_cast<Enemy>(objA);
			bool isTankB = std::dynamic_pointer_cast<Player>(objB) || std::dynamic_pointer_cast<Enemy>(objB);

			if (isTankA && isTankB) {

				float dx = objB->pos.x - objA->pos.x;
				float dy = objB->pos.y - objA->pos.y;
				float dist = std::sqrt(dx * dx + dy * dy);

				// 戦車の当たり判定の半径
				float radiusA = 20.0f;
				float radiusB = 20.0f;

				if (dist > 0.0f && dist < (radiusA + radiusB)) {

					// めり込んでいる長さ
					float overlap = (radiusA + radiusB) - dist;

					// 押し出す方向を計算
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

	auto it = std::remove_if(objList.begin(), objList.end(), [](std::shared_ptr<GameObject> obj) {
		return obj->isDead;
		});
	objList.erase(it, objList.end());

	//ステージクリアチェック
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
			SHADER.m_spriteShader.DrawTex(&m_dirtTex, 0, 0,&srcRect);
		}
	}

	for (auto& obj : objList)
	{
		obj->Draw();
	}

	std::string scoreStr = "SCORE: " + std::to_string(m_score);

	SHADER.m_spriteShader.DrawString(-600, 300, scoreStr.c_str(), Math::Vector4(1, 1, 1, 1));
}
