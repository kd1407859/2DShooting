#include "System/KdSystem.h"

void KdDirect3D::GetViewport(Math::Viewport & out) const
{
	UINT numVPs = 1;
	D3D11_VIEWPORT vp;
	m_pDeviceContext->RSGetViewports(&numVPs, &vp);
	out = vp;
}

bool KdDirect3D::Init(HWND hWnd, int w, int h, bool deviceDebug, std::string& errMsg)
{
	HRESULT hr;

	//=====================================================
	// ファクトリー作成(ビデオ グラフィックの設定の列挙や指定に使用されるオブジェクト)
	//=====================================================
	hr = CreateDXGIFactory1(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&m_pGIFactory));
	if (FAILED(hr)) {
		errMsg = "ファクトリー作成失敗";

		Release();
		return false;
	}

	//=====================================================
	//デバイス生成(主にリソース作成時に使用するオブジェクト)
	//=====================================================
	UINT creationFlags = 0;

	if (deviceDebug) {
		// Direct3Dのデバッグを有効にする(すごく重いが細かいエラーがわかる)
		creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
	}

	D3D_FEATURE_LEVEL featureLevels[] = 
	{
		D3D_FEATURE_LEVEL_11_1,	// Direct3D 11.1  ShaderModel 5
		D3D_FEATURE_LEVEL_11_0,	// Direct3D 11    ShaderModel 5
		D3D_FEATURE_LEVEL_10_1,	// Direct3D 10.1  ShaderModel 4
		D3D_FEATURE_LEVEL_10_0,	// Direct3D 10.0  ShaderModel 4
		D3D_FEATURE_LEVEL_9_3,	// Direct3D 9.3   ShaderModel 3
		D3D_FEATURE_LEVEL_9_2,	// Direct3D 9.2   ShaderModel 3
		D3D_FEATURE_LEVEL_9_1,	// Direct3D 9.1   ShaderModel 3
	};

	// デバイスとでデバイスコンテキストを作成
	hr = D3D11CreateDevice(
				nullptr,
				D3D_DRIVER_TYPE_HARDWARE,
				nullptr,
				creationFlags,
				featureLevels,
				_countof(featureLevels),
				D3D11_SDK_VERSION,
				&m_pDevice,
				nullptr,
				&m_pDeviceContext);
	if FAILED( hr )
	{
		errMsg = "Direct3D11デバイス作成失敗";

		Release();
		return false;
	}

	//=====================================================
	// スワップチェイン作成(フロントバッファに表示可能なバックバッファを持つもの)
	//=====================================================
	DXGI_SWAP_CHAIN_DESC	DXGISwapChainDesc = {};		// スワップチェーンの設定データ
	DXGISwapChainDesc.BufferDesc.Width = w;
	DXGISwapChainDesc.BufferDesc.Height = h;
	DXGISwapChainDesc.BufferDesc.RefreshRate.Numerator  = 0;
	DXGISwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	DXGISwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGISwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	DXGISwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;// DXGI_MODE_SCALING_CENTERED;	// DXGI_MODE_SCALING_UNSPECIFIED
	DXGISwapChainDesc.SampleDesc.Count = 1;
	DXGISwapChainDesc.SampleDesc.Quality = 0;
	DXGISwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;
	DXGISwapChainDesc.BufferCount = 2;
	DXGISwapChainDesc.OutputWindow = hWnd;
	DXGISwapChainDesc.Windowed = TRUE;
	DXGISwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	DXGISwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	if(FAILED(m_pGIFactory->CreateSwapChain(m_pDevice, &DXGISwapChainDesc, &m_pGISwapChain))){
		errMsg = "スワップチェイン作成失敗";

		Release();
		return false;
	}

	// スワップチェインからバックバッファ取得
	ID3D11Texture2D* pBackBuffer;
	if (FAILED(m_pGISwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer))) {
		errMsg = "バックバッファ取得失敗";

		Release();
		return false;
	}

	// バックバッファ用のレンダーターゲットビュー作成
	{
		D3D11_TEXTURE2D_DESC desc;
		pBackBuffer->GetDesc(&desc);

		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = desc.Format;	// Format
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;			// 単品テクスチャ

		// レンダーターゲットビュー作成
		HRESULT hr = m_pDevice->CreateRenderTargetView(pBackBuffer, &rtvDesc, &m_rtvBackBuffer);
		if (FAILED(hr)) {
			errMsg = "バックバッファ作成失敗";
			Release();
			return false;
		}
	}

	pBackBuffer->Release();

	/*
	// ALT+Enterでフルスクリーン不可にする
	{
		IDXGIDevice* pDXGIDevice;
		m_pDevice->QueryInterface<IDXGIDevice>(&pDXGIDevice);

		IDXGIAdapter* pDXGIAdapter;
		pDXGIDevice->GetParent(__uuidof(IDXGIAdapter), (void **)&pDXGIAdapter);

		IDXGIFactory* pIDXGIFactory;
		pDXGIAdapter->GetParent(__uuidof(IDXGIFactory), (void **)&pIDXGIFactory);

		pIDXGIFactory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER);

		pDXGIDevice->Release();
		pDXGIAdapter->Release();
		pIDXGIFactory->Release();
	}
	*/

	//=========================================================
	// Zバッファ作成
	//=========================================================
	{
		// バッファ作成
		D3D11_TEXTURE2D_DESC desc = {};
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.Format = DXGI_FORMAT_R32_TYPELESS;
		desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		desc.Width = (UINT)w;
		desc.Height = (UINT)h;
		desc.CPUAccessFlags = 0;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;

		ID3D11Texture2D* buffer = nullptr;
		HRESULT hr = m_pDevice->CreateTexture2D(&desc, nullptr, &buffer);
		if (FAILED(hr)) {
			errMsg = "Zバッファ作成失敗";
			Release();
			return false;
		}

		// ビュー作成
		D3D11_DEPTH_STENCIL_VIEW_DESC dpDesc = {};
		dpDesc.Format = DXGI_FORMAT_D32_FLOAT;
		dpDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		dpDesc.Texture2D.MipSlice = 0;

		// デプスステンシルビュー作成
		hr = m_pDevice->CreateDepthStencilView(buffer, &dpDesc, &m_dsvZBuffer);
		if (FAILED(hr)) {
			errMsg = "Zバッファ作成失敗";
			Release();
			return false;
		}

		buffer->Release();
	}
	/*
	m_texDepthStencil = std::make_shared<KdTexture>();
	if (m_texDepthStencil->CreateDepth(w, h, DXGI_FORMAT_R24G8_TYPELESS) == false) {
		errMsg = "Zバッファ作成失敗";
		Release();
		return false;
	}
	*/

	//=========================================================
	// バックバッファ、Zバッファ をデバイスコンテキストへセットする
	//=========================================================
	{
		ID3D11RenderTargetView* rtvs[] = { m_rtvBackBuffer };
		m_pDeviceContext->OMSetRenderTargets(1, rtvs, m_dsvZBuffer);
	}

	//=========================================================
	// ビューポートの設定
	//=========================================================
	{
		// ビューポートの設定
		D3D11_VIEWPORT vp;
		vp.Width = (float)w;
		vp.Height = (float)h;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		m_pDeviceContext->RSSetViewports(1, &vp);
	}

	//=========================================================
	// ブレンドステートセットをセットしておく
	//=========================================================

	// ブレンドステートセット
	SetBlendState(BlendMode::Alpha);
	// サンプラーステートセット
	SetSamplerState(0, SamplerFilterMode::Anisotropic, 4, SamplerAddressingMode::Wrap, false);
	SetSamplerState(1, SamplerFilterMode::Anisotropic, 4, SamplerAddressingMode::Clamp, false);
	// ラスタライザーステートセット
	SetRasterizerState(D3D11_CULL_BACK, D3D11_FILL_SOLID, true, false);
	// 深度ステンシルステートセット
	SetDepthStencilState(true, true);

	//=========================================================
	// 1x1の白テクスチャ作成
	//=========================================================
	{
		// 0xAABBGGRR
		auto col = Math::Color(1, 1, 1, 1).RGBA();
		D3D11_SUBRESOURCE_DATA srd;
		srd.pSysMem = &col;
		srd.SysMemPitch = 4;
		srd.SysMemSlicePitch = 0;

		m_texWhite = std::make_shared<KdTexture>();
		m_texWhite->Create(1, 1, DXGI_FORMAT_R8G8B8A8_UNORM, 1, &srd);
	}

	return true;
}

void KdDirect3D::Release()
{
	m_tempVertexBuffer.Release();

	KdSafeRelease(m_rtvBackBuffer);
	KdSafeRelease(m_dsvZBuffer);

	KdSafeRelease(m_pGISwapChain);
	KdSafeRelease(m_pGIAdapter);
	KdSafeRelease(m_pGIFactory);
	KdSafeRelease(m_pDeviceContext);
	KdSafeRelease(m_pDevice);

	m_texWhite = nullptr;
}

void KdDirect3D::SetDepthStencilState(bool enable, bool writeEnable)
{
	// ステート作成
	ID3D11DepthStencilState* state = KdCreateDepthStencilState(m_pDevice, enable, writeEnable);

	// デバイスコンテキストへセット
	GetDevContext()->OMSetDepthStencilState(state, 0);

	// 解放(ここでは参照カウンタが-1されるだけ)
	state->Release();
}

void KdDirect3D::SetRasterizerState(D3D11_CULL_MODE cullMode, D3D11_FILL_MODE fillMode, bool depthClipEnable, bool scissorEnable)
{
	// ステート作成
	ID3D11RasterizerState* state = KdCreateRasterizerState(m_pDevice, cullMode, fillMode, depthClipEnable, scissorEnable);

	// デバイスコンテキストへセット
	GetDevContext()->RSSetState(state);

	// 解放(ここでは参照カウンタが-1されるだけ)
	state->Release();
}

void KdDirect3D::SetSamplerState(int slotNo, SamplerFilterMode filterType, UINT maxAnisotropy, SamplerAddressingMode addressingMode, bool comparisonModel)
{
	// ステート作成
	ID3D11SamplerState* state = KdCreateSamplerState(m_pDevice, filterType, maxAnisotropy, addressingMode, comparisonModel);

	// 各シェーダへセット
	GetDevContext()->VSSetSamplers(slotNo, 1, &state);
	GetDevContext()->PSSetSamplers(slotNo, 1, &state);
	GetDevContext()->GSSetSamplers(slotNo, 1, &state);
	GetDevContext()->CSSetSamplers(slotNo, 1, &state);

	// 解放(ここでは参照カウンタが-1されるだけ)
	state->Release();
}

void KdDirect3D::SetBlendState(BlendMode mode)
{
	// ステート作成
	ID3D11BlendState* state = KdCreateBlendState(m_pDevice, mode);

	// デバイスコンテキストへセット
	GetDevContext()->OMSetBlendState(state, Math::Color(0,0,0,0), 0xFFFFFFFF);

	// 解放(ここでは参照カウンタが-1されるだけ)
	state->Release();
}

void KdDirect3D::DrawVertices(D3D_PRIMITIVE_TOPOLOGY topology,int vertexCount, const void* pVertexStream, UINT stride)
{
	
	// プリミティブトポロジーをセット
	GetDevContext()->IASetPrimitiveTopology(topology);

	// 全頂点の総バイトサイズ
	UINT totalSize = vertexCount * stride;


#if 1

	// 頂点バッファのサイズが小さいときは、リサイズのため再作成する
	if (m_tempVertexBuffer.GetBufferSize() < totalSize) {
		m_tempVertexBuffer.Create(D3D11_BIND_VERTEX_BUFFER, totalSize, D3D11_USAGE_DYNAMIC, nullptr);
	}

	//============================================================
	//
	// 単純なDISCARDでの書き込み。
	//  DISCARDは、新たなバッファを内部で作成し、前回のバッファは使い終わると無効にするものです。
	//  つまり書き込む度に新規のバッファになる感じです。
	//  バッファのサイズが大きいと、その分新規のバッファを作成する時間がかかってしまいます。
	//  これを改善したい場合は、「DISCARDとNO_OVERWRITEの組み合わせ技」の方法で行うほうがよいです。
	//
	//============================================================

	// 全頂点をバッファに書き込み(DISCARD指定)
	m_tempVertexBuffer.WriteData(pVertexStream, totalSize);

	// 頂点バッファーをデバイスへセット
	{
		UINT offset = 0;
		D3D.GetDevContext()->IASetVertexBuffers(0, 1, m_tempVertexBuffer.GetAddress(), &stride, &offset);
	}

	// 描画
	GetDevContext()->Draw(vertexCount, 0);

#else
	//============================================================
	//
	// DISCARDとNO_OVERWRITEの組み合わせ技
	// https://msdn.microsoft.com/ja-jp/library/ee416245(v=vs.85).aspx
	//
	//  DISCARDのみだと上記の説明で書いているように、バッファサイズが大きくなると
	//  速度低下が激しくなってきます。
	//  この方法だと、バッファの再作成であるDISCARDの回数を減らすことが出来るので、
	//  速度低下をマシにすることが出来ます。
	//
	//============================================================

	// 初回のバッファ作成。大きめに作成しておく。
	if (m_tempVertexBuffer.GetBufferSize() == 0) {
		static const UINT kMaxBufferSize = 1000000;
		m_tempVertexBuffer.Create(D3D11_BIND_VERTEX_BUFFER, kMaxBufferSize, D3D11_USAGE_DYNAMIC, nullptr);
	}

	// 現在のバッファ書き込み位置(byte)
	static UINT currentPos = 0;

	// 現在位置からバッファのサイズをオーバーする場合はDISCARD(つまり新規でバッファを用意する)
	if (currentPos + totalSize >= m_tempVertexBuffer.GetBufferSize()) {

		// バッファは新規で用意されているので、先頭からバッファに書き込み
		char* p = (char*)m_tempVertexBuffer.Map(D3D11_MAP_WRITE_DISCARD);
		memcpy(p, pVertexStream, totalSize);
		m_tempVertexBuffer.Unmap();

		// 頂点バッファーをデバイスへセット
		{
			UINT offset = 0;
			D3D.GetDevContext()->IASetVertexBuffers(0, 1, m_tempVertexBuffer.GetAddress(), &stride, &offset);
		}

		// 描画
		GetDevContext()->Draw(vertexCount, 0);

		// 次回の書き込み位置に進める
		currentPos = totalSize;
	}
	// サイズオーバーしない場合は、NO_OVERWRITEでバッファの途中から書き込む
	else {
		// バッファの途中に書き込み ※currentPos以前の場所は、現在描画で使用中かもしれないので絶対に触ってはダメ
		char* p = (char*)m_tempVertexBuffer.Map(D3D11_MAP_WRITE_NO_OVERWRITE);
		memcpy(&p[currentPos], pVertexStream, totalSize);
		m_tempVertexBuffer.Unmap();

		// 書き込んだ位置を先頭に、頂点バッファーをデバイスへセット
		{
			D3D.GetDevContext()->IASetVertexBuffers(0, 1, m_tempVertexBuffer.GetAddress(), &stride, &currentPos);
		}

		// 描画
		GetDevContext()->Draw(vertexCount, 0);

		// 次に書き込む位置に進める
		currentPos += totalSize;
	}
#endif
	
}


