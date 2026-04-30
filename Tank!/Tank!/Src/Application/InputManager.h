#pragma once

class InputManager
{
private:
    static InputManager* m_instance;
    int m_transitionFrameCounter = 0;
    static const int TRANSITION_DELAY_FRAMES = 15; // シーン遷移後の入力無視フレーム数

    InputManager() {}

public:
    static InputManager& GetInstance()
    {
        if (m_instance == nullptr)
        {
            m_instance = new InputManager();
        }
        return *m_instance;
    }

    // シーン遷移開始
    void OnSceneTransition()
    {
        m_transitionFrameCounter = TRANSITION_DELAY_FRAMES;
    }

    // 毎フレーム呼び出し
    void Update()
    {
        if (m_transitionFrameCounter > 0)
        {
            m_transitionFrameCounter--;
        }
    }

    // 入力可能かどうか
    bool IsInputEnabled() const
    {
        return m_transitionFrameCounter <= 0;
    }

    // キーが押されているか（入力有効時のみ）
    bool IsKeyPressed(int vKey) const
    {
        if (!IsInputEnabled())
            return false;

        return (GetAsyncKeyState(vKey) & 0x8000) != 0;
    }

    // マウス左クリックが押されているか（入力有効時のみ）
    bool IsMouseLeftPressed() const
    {
        if (!IsInputEnabled())
            return false;

        return (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
    }

    // 遷移中かどうか
    bool IsTransitioning() const
    {
        return m_transitionFrameCounter > 0;
    }
};
