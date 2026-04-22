#pragma once

//========================================================
//
// Direct3D用便利機能
//
//========================================================



//============================
// ステート作成便利関数
//============================

// 深度ステンシルステート作成
// ・device				… D3Dデバイス
// ・zEnable			… 深度テスト有効
// ・zWriteEnable		… 深度書き込み有効
ID3D11DepthStencilState* KdCreateDepthStencilState(ID3D11Device* device, bool zEnable, bool zWriteEnable);

// ラスタライザーステート作成
// ・device				… D3Dデバイス
// ・cullMode			… カリングモード
// ・fillMode			… 三角形の描画モード
// ・depthClipEnable	… Zクリッピングを有効にする
// ・scissorEnable		… 切り抜き範囲を有効にする
ID3D11RasterizerState* KdCreateRasterizerState(ID3D11Device* device, D3D11_CULL_MODE cullMode, D3D11_FILL_MODE fillMode, bool depthClipEnable, bool scissorEnable);

// サンプラステート
enum class SamplerFilterMode {
	Point,			// ポイントフィルタ
	Linear,			// 線形フィルタ
	Anisotropic,	// 異方性フィルタ
};

enum class SamplerAddressingMode {
	Wrap,			// ラップモード
	Clamp,			// クランプモード
};

// ・device				… D3Dデバイス
// ・filterType			… 0:補間なし 1:線形フィルタ 2:異方性フィルタ
// ・maxAnisotropy		… 異方性フィルタ時の最大値　2, 4, 6, 8, 10, 12, 14, 16 のいずれか
// ・addressingMode		… テクスチャアドレッシングモード 0:Wrap 1:Clamp
// ・comparisonModel	… 比較モードON　シャドウマッピングなどで使用する
ID3D11SamplerState* KdCreateSamplerState(ID3D11Device* device, SamplerFilterMode filterType, UINT maxAnisotropy, SamplerAddressingMode addressingMode, bool comparisonModel);

// ブレンドモード用定数
enum class BlendMode {
	NoBlend,		// ブレンドをしない
	Alpha,			// 半透明ブレンド
	Add,			// 加算ブレンド
	Stencil			// ステンシル
};

// ブレンドステート作成
// ・device				… D3Dデバイス
// ・mode				… 合成モード
ID3D11BlendState* KdCreateBlendState(ID3D11Device* device, BlendMode mode);


