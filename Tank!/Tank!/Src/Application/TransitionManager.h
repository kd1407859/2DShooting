#pragma once

class Scene;

class TransitionManager
{
public:
    enum class State
    {
        None,       // トランジションなし（通常状態）
        Closing,    // アイリスアウト中（円が縮む）
        Opening,    // アイリスイン中（円が広がる）
    };

    static TransitionManager& GetInstance()
    {
        static TransitionManager instance;
        return instance;
    }

    // シーン切り替え要求（SceneManagerのChangeSceneの代わりに呼ぶ）
    void RequestChange(Scene* nextScene);

    void Update();
    void Draw();    // 最前面に描画する

    bool IsTransitioning() const { return m_state != State::None; }

private:
    TransitionManager() {}

    State  m_state = State::None;
    float  m_radius = 0.0f;           // 現在の円の半径
    float  m_maxRadius = 900.0f;        // 画面全体を覆える半径
    float  m_speed = 30.0f;          // 半径の変化速度（調整可）
    Scene* m_nextScene = nullptr;       // 切り替え先
};

#define TRANSITION TransitionManager::GetInstance()