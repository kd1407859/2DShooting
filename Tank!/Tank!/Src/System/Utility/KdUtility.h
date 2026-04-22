#pragma once

//===========================================
//
// 短縮名
//
//===========================================

// ComPtr短縮名
using Microsoft::WRL::ComPtr;

// 算術系短縮名
namespace Math = DirectX::SimpleMath;

// 角度変換
inline constexpr float ToRadians(float fDegrees) { return fDegrees * (3.141592654f / 180.0f); }
inline constexpr float ToDegrees(float fRadians) { return fRadians * (180.0f / 3.141592654f); }

// 安全にReleaseするための関数
template<class T>
void KdSafeRelease(T*& p)
{
	if (p)
	{
		ULONG c = p->Release();
		p = nullptr;
	}
}

// 安全にDeleteするための関数
template<class T>
void KdSafeDelete(T*& p)
{
	if (p)
	{
		delete p;
		p = nullptr;
	}
}

// ファイルパスから、親ディレクトリまでのパスを取得
inline std::string KdGetDirFromPath(const std::string &path)
{
	const std::string::size_type pos = std::max<signed>(path.find_last_of('/'), path.find_last_of('\\'));
	return (pos == std::string::npos) ? std::string() : path.substr(0, pos + 1);
}

// RECT構造体の追加機能バージョン
struct KdRect : public RECT
{
	KdRect() {
		left = top = right = bottom = 0;
	}

	KdRect(const RECT& rc)
	{
		*this = rc;
	}

	KdRect(LONG left, LONG top, LONG right, LONG bottom)
	{
		SetRect(this, left, top, right, bottom);
	}

};


