#include "TransitionManager.h"
#include "SceneManager.h"

void TransitionManager::RequestChange(Scene* nextScene)
{
    if (m_state != State::None)
    {
        // 既にトランジション中なら無視（二重呼び出し防止）
        delete nextScene;
        return;
    }
    m_nextScene = nextScene;
    m_state = State::Closing;
    m_radius = m_maxRadius;   // 大きい状態から縮み始める
}

void TransitionManager::Update()
{
    if (m_state == State::Closing)
    {
        m_radius -= m_speed;
        if (m_radius <= 0.0f)
        {
            m_radius = 0.0f;
            // 完全に閉じた → シーン切り替え実行
            SceneManager::GetInstance().ChangeScene(m_nextScene);
            m_nextScene = nullptr;
            m_state = State::Opening;
        }
    }
    else if (m_state == State::Opening)
    {
        m_radius += m_speed;
        if (m_radius >= m_maxRadius)
        {
            m_radius = m_maxRadius;
            m_state = State::None;  // トランジション完了
        }
    }
}

void TransitionManager::Draw()
{
    if (m_state == State::None) return;

    // 円形ワイプを描画する
    // ※実装方法はエンジンのAPI次第（後述）
    SHADER.m_spriteShader.DrawCircle(0, 0, (int)m_radius, nullptr, true);
}
