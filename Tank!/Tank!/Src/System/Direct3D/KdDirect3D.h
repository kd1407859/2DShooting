#pragma once

// 便利機能
#include "KdDirect3DHelper.h"

class KdTexture;

//=======================================================================================
//
// Direct3Dクラス
//
//	Direct3D11に必要な変数や、よく行う操作をまとめています。
//
//
//=======================================================================================
class KdDirect3D {
public:

	//==============================================================
	//
	// 取得
	//
	//==============================================================

	// Direct3Dデバイス取得
	ID3D11Device*			GetDev() { return m_pDevice; }

	// Direct3Dデバイスコンテキスト取得
	ID3D11DeviceContext*	GetDevContext() { return m_pDeviceContext; }

	// SwapChain取得
	IDXGISwapChain*			GetSwapChain() { return m_pGISwapChain; }

	// バックバッファ取得
	ID3D11RenderTargetView*	GetBackBuffer() { return m_rtvBackBuffer; }

	// Zバッファ取得
	ID3D11DepthStencilView*	GetZBuffer() { return m_dsvZBuffer; }

	// ビューポート取得
	void					GetViewport(Math::Viewport& out) const;

	// 1x1 白テクスチャ取得
	const std::shared_ptr<KdTexture>&		GetWhiteTex() const { return m_texWhite; }

	//==============================================================
	//
	// 初期化・解放
	//
	//==============================================================

	// 初期化
	bool Init(HWND hWnd, int w, int h, bool deviceDebug, std::string& errMsg);

	// 解放
	void Release();

	//==============================================================
	//
	// 簡易ステート設定関数
	// ※毎回作成・解放するため効率は悪い
	//
	//==============================================================

	// 簡易版 深度ステンシルステート設定　※毎回作成・解放するため効率は悪い
	// ・enable				… 深度テスト有効/無効
	// ・writeEnable		… 深度書き込み有効/無効
	void SetDepthStencilState(bool enable, bool writeEnable);

	// 簡易版 ライスタライザーステート設定
	// ・cullMode			… カリングモード
	// ・fillMode			… 三角形の描画モード
	// ・depthClipEnable	… Zクリッピングを有効にする
	void SetRasterizerState(D3D11_CULL_MODE cullMode, D3D11_FILL_MODE fillMode, bool depthClipEnable, bool scissorEnable);

	// 簡易版 サンプラーステート設定　※毎回作成・解放するため効率は悪い
	// ・slotNo				… セットするスロット番号
	// ・filterType			… フィルター
	// ・maxAnisotropy		… 異方性フィルタ(Anisotropic)時の最大値　2, 4, 6, 8, 10, 12, 14, 16 のいずれか
	// ・addressingMode		… テクスチャアドレッシングモード
	// ・comparisonModel	… 比較モードON　シャドウマッピングなどで使用する
	void SetSamplerState(int slotNo, SamplerFilterMode filterType, UINT maxAnisotropy, SamplerAddressingMode addressingMode, bool comparisonModel);

	// 簡易版 ブレンドステート設定　※毎回作成・解放するため効率は悪い
	// ・mode				… モード
	void SetBlendState(BlendMode mode);

	//==============================================================

	// 頂点を描画する簡易的な関数
	// ※頂点バッファを使い回して描画していますので効率は悪め。よりパフォーマンスを上げたい場合は工夫する必要あり。
	// ・topology		… 頂点をどのような形状で描画するか　D3D_PRIMITIVE_TOPOLOGYマクロを使用
	// ・vertexCount	… 頂点数
	// ・pVertexStream	… 頂点配列の先頭アドレス
	// ・stride			… １頂点のバイトサイズ
	void DrawVertices(D3D_PRIMITIVE_TOPOLOGY topology, int vertexCount, const void* pVertexStream, UINT stride);

	//==============================================================

	//バックバッファへ切り替え(１年生用）
	void SetBackBuffer()
	{
		m_pDeviceContext->OMSetRenderTargets(1, &m_rtvBackBuffer, m_dsvZBuffer);
	}

	// 
	~KdDirect3D() {
		Release();
	}

private:

	ID3D11Device*			m_pDevice = nullptr;			// Direct3D11の中心になるクラス。全体の管理とバッファ、シェーダ、テクスチャなどのリソース作成などを行う。D3D9とは違って、このクラスは描画関係のメンバ関数を持たない。
	ID3D11DeviceContext*	m_pDeviceContext = nullptr;		// 描画処理を行うクラス。
															// 内部的には、レンダリングコマンドと呼ばれるバイナリデータを作成し、GPUに送る。GPUがレンダリングコマンドを解析することで実際の描画処理が行われる。

	IDXGIAdapter*			m_pGIAdapter = nullptr;			// ディスプレイ サブシステム(1つまたは複数のGPU、DAC、およびビデオ メモリー)。
	IDXGIFactory*			m_pGIFactory = nullptr;			// フルスクリーン切り替えなどで使用。
	IDXGISwapChain*			m_pGISwapChain = nullptr;		// ウインドウへの表示やダブルバッファリングなどを行うクラス。マルチサンプリング、リフレッシュレートなどの設定もできるみたい。

	// バッファ
	ID3D11RenderTargetView*	m_rtvBackBuffer = nullptr;		// バックバッファ
	ID3D11DepthStencilView*	m_dsvZBuffer = nullptr;			// Zバッファ

	// DrawVertices用 頂点バッファ
	KdBuffer					m_tempVertexBuffer;

	//------------------------
	// 便利テクスチャ
	//------------------------
	// 1x1 白テクスチャ
	std::shared_ptr<KdTexture>	m_texWhite;

//-------------------------------
// シングルトン
//-------------------------------
private:

	KdDirect3D() {
	}

public:
	static KdDirect3D &GetInstance() {
		static KdDirect3D instance;
		return instance;
	}
};

// 簡単にアクセスできるようにするためのマクロ
#define D3D KdDirect3D::GetInstance()
