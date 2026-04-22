#pragma once
#include<Windows.h>
#include <d3d11.h>

// ★追加：画面サイズ定義
const float SCREEN_WIDTH = 1280.0f;
const float SCREEN_HEIGHT = 720.0f;

// ★追加：画面の端の座標
const float SCREEN_RIGHT = SCREEN_WIDTH / 2.0f;		//  640.0f
const float SCREEN_LEFT = -SCREEN_WIDTH / 2.0f;		// -640.0f
const float SCREEN_TOP = SCREEN_HEIGHT / 2.0f;		//  360.0f
const float SCREEN_BOTTOM = -SCREEN_HEIGHT / 2.0f;	// -360.0f

//============================================================
// アプリケーションクラス
//	APP.～ でどこからでもアクセス可能
//============================================================
class Application
{
// メンバ
public:

	// アプリケーション実行
	void Execute();

	// アプリケーション終了
	void End() { m_endFlag = true; }


	//=====================================================
	// グローバルデータ
	//=====================================================

	// ゲームウィンドウクラス
	KdGameWindow		m_window;

	// FPS制御
	int				m_fps = 0;					// 現在のFPS値
	int				m_maxFps = 60;				// 最大FPS


private:

	// アプリケーション初期化
	bool Init(int w, int h);

	// アプリケーション解放
	void Release();

	// ゲーム終了フラグ trueで終了する
	bool		m_endFlag = false;


//=====================================================
// シングルトンパターン
//=====================================================
private:
	// 
	Application() {}

public:
	static Application &GetInstance(){
		static Application Instance;
		return Instance;
	}
};

// 簡単にアクセスできるようにするためのマクロ
#define APP Application::GetInstance()
