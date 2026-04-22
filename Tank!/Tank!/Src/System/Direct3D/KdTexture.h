#pragma once

//====================================================
//
// テクスチャクラス
//
//====================================================
class KdTexture
{
public:

	//====================================================
	//
	// 取得
	//
	//====================================================
	// シェーダリソースビュー取得
	ID3D11ShaderResourceView*	GetSRView() const { return m_srv; }
	// レンダーターゲットビュー取得
	ID3D11RenderTargetView*		GetRTView() const { return m_rtv; }
	// 深度ステンシルビュー取得
	ID3D11DepthStencilView*		GetDSView() const { return m_dsv; }

	// シェーダリソースビューのアドレス取得
	ID3D11ShaderResourceView* const*	GetSRViewAddress() const { return &m_srv; }

	// 画像情報取得
	const D3D11_TEXTURE2D_DESC& GetInfo() const { return m_desc; }

	//====================================================
	//
	// 読み込み・作成
	//
	//====================================================

	// 画像ファイルを読み込む。ShaderResourceViewは必ず生成される。
	// ・filename		… 画像ファイル名
	// ・renderTarget	… レンダーターゲットビューを生成する
	// ・depthStencil	… 深度ステンシルビューを生成する
	// ・generateMipmap	… ミップマップ生成する？
	bool Load(const std::string& filename, bool renderTarget = false, bool depthStencil = false, bool generateMipmap = false);

	// desc情報からテクスチャリソースを作成する
	// ・desc		… 作成するテクスチャリソースの情報
	// ・fillData	… バッファに書き込むデータ　nullptrだと書き込みなし
	// 戻り値：true … 成功
	bool Create(const D3D11_TEXTURE2D_DESC& desc, const D3D11_SUBRESOURCE_DATA* fillData = nullptr);

	// 通常テクスチャとして作成
	// ※テクスチャリソースを作成し、ShaderResourceViewのみを作成します
	// ・w			… 画像の幅(ピクセル)
	// ・h			… 画像の高さ(ピクセル)
	// ・format		… 画像の形式　DXGI_FORMATを使用
	// ・arrayCnt	… 「テクスチャ配列」を使用する場合、その数。1で通常の1枚テクスチャ
	// ・fillData	… バッファに書き込むデータ　nullptrだと書き込みなし
	// 戻り値：true … 成功
	bool Create(int w, int h, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM, UINT arrayCnt = 1, const D3D11_SUBRESOURCE_DATA* fillData = nullptr);

	// レンダーターゲットテクスチャとして作成
	// ※テクスチャリソースを作成し、RenderTargetViewとShaderResourceViewの2種類を作成します
	// ・w			… 画像の幅(ピクセル)
	// ・h			… 画像の高さ(ピクセル)
	// ・format		… 画像の形式　DXGI_FORMATを使用
	// ・arrayCnt	… 「テクスチャ配列」を使用する場合、その数。1で通常の1枚テクスチャ
	// ・fillData	… バッファに書き込むデータ　nullptrだと書き込みなし
	// 戻り値：true … 成功
	bool CreateRenderTarget(int w, int h, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM, UINT arrayCnt = 1, const D3D11_SUBRESOURCE_DATA* fillData = nullptr);

	// Zバッファテクスチャとして作成
	// ※テクスチャリソースを作成し、DepthStencilViewとShaderResourceViewの2種類を作成します
	// ・w			… 画像の幅(ピクセル)
	// ・h			… 画像の高さ(ピクセル)
	// ・format		… 画像の形式　DXGI_FORMATを使用　32bit：DXGI_FORMAT_R32_TYPELESS  ステンシル付き：DXGI_FORMAT_R24G8_TYPELESS
	// ・arrayCnt	… 「テクスチャ配列」を使用する場合、その数。1で通常の1枚テクスチャ
	// ・fillData	… バッファに書き込むデータ　nullptrだと書き込みなし
	// 戻り値：true … 成功
	bool CreateDepth(int w, int h, DXGI_FORMAT format = DXGI_FORMAT_R32_TYPELESS, UINT arrayCnt = 1, const D3D11_SUBRESOURCE_DATA* fillData = nullptr);

	//===================================================================
	//
	// クリア関係
	//
	//===================================================================

	// RenderTargetをクリア
	// ※RenderTargetViewを作成する必要があります
	// ・ColorRGBA	… クリアする色
	void ClearRenerTarget(const Math::Color& ColorRGBA);

	// DepthStencilをクリア
	// ※DepthStencilViewを作成する必要があります
	// ・bDepth		… Zバッファクリアする？
	// ・bStencil	… ステンシルバッファクリアする？
	// ・depth		… クリアするZバッファの値
	// ・stencil	… クリアするステンシルバッファの値
	void ClearDepth(bool bDepth = true, bool bStencil = true, FLOAT depth = 1.0f, UINT8 stencil = 0);

	// 
	KdTexture() {}

	//====================================================
	// 解放
	//====================================================
	void Release();

	// 
	~KdTexture()
	{
		Release();
	}

	//====================================================
	// 静的関数
	//====================================================

	// viewから画像情報を取得
	static void GetTextureInfo(ID3D11View* view, D3D11_TEXTURE2D_DESC& outDesc);


	//レンダーターゲットの切り替え(１年生用)
	void SetRenderTarget()
	{
		D3D.GetDevContext()->OMSetRenderTargets(1, &m_rtv, D3D.GetZBuffer());
	}

private:

	//--------------------------------------------------------------------------
	// m_resourceの設定を元に、各ビューを作成する
	//--------------------------------------------------------------------------
	bool CreateViewsFromResource();

private:

	// 画像リソース
	ID3D11Texture2D*			m_resource = nullptr;
	// 画像情報
	D3D11_TEXTURE2D_DESC		m_desc = {};

	// シェーダリソースビュー(読み取り用)
	ID3D11ShaderResourceView*	m_srv = nullptr;
	// レンダーターゲットビュー(書き込み用)
	ID3D11RenderTargetView*		m_rtv = nullptr;
	// 深度ステンシルビュー(Zバッファ用)
	ID3D11DepthStencilView*		m_dsv = nullptr;

	// 画像ファイル名
	std::string					m_filepath;

private:
	// コピー禁止用
	KdTexture(const KdTexture& src) = delete;
	void operator=(const KdTexture& src) = delete;
};

