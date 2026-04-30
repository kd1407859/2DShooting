#pragma once
#include <Windows.h>
#include <d3d11.h>

class CursorManager
{
private:
    static CursorManager* m_instance;
    HCURSOR m_customCursor = nullptr;
    bool m_isInitialized = false;

    CursorManager() {}

public:
    static CursorManager& GetInstance()
    {
        if (m_instance == nullptr)
        {
            m_instance = new CursorManager();
        }
        return *m_instance;
    }

    // カスタムカーソルの初期化
    bool Init(const char* imagePath);

    // カスタムカーソルの適用
    void Apply();

    // 終了処理
    void Release();

    bool IsInitialized() const { return m_isInitialized; }
};

#define CURSOR_MGR CursorManager::GetInstance()
