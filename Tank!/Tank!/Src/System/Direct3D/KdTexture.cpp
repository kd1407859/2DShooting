#include "System/KdSystem.h"

#include "KdTexture.h"


bool KdTexture::Load(const std::string & filename, bool renderTarget, bool depthStencil, bool generateMipmap)
{
	Release();
	if (filename.empty())return false;

	// ファイル名をWideCharへ変換
	std::wstring wFilename = sjis_to_wide(filename);

	// Bind Flags
	UINT bindFlags = 0;
	bindFlags |= D3D11_BIND_SHADER_RESOURCE;
	if (renderTarget)bindFlags |= D3D11_BIND_RENDER_TARGET;
	if (depthStencil)bindFlags |= D3D11_BIND_DEPTH_STENCIL;

	// ※DirectX Texライブラリを使用して画像を読み込む

	DirectX::TexMetadata meta;
	DirectX::ScratchImage image;

	bool bLoaded = false;

	// WIC画像読み込み
	//  WIC_FLAGS_ALL_FRAMES … gifアニメなどの複数フレームを読み込んでくれる
	if (SUCCEEDED(DirectX::LoadFromWICFile(wFilename.c_str(), DirectX::WIC_FLAGS_ALL_FRAMES, &meta, image)))
	{
		bLoaded = true;
	}

	// DDS画像読み込み
	if (bLoaded == false) {
		if (SUCCEEDED(DirectX::LoadFromDDSFile(wFilename.c_str(), DirectX::DDS_FLAGS_NONE, &meta, image)))
		{
			bLoaded = true;
		}
	}

	// TGA画像読み込み
	if (bLoaded == false) {
		if (SUCCEEDED(DirectX::LoadFromTGAFile(wFilename.c_str(), &meta, image)))
		{
			bLoaded = true;
		}
	}

	// HDR画像読み込み
	if (bLoaded == false) {
		if (SUCCEEDED(DirectX::LoadFromHDRFile(wFilename.c_str(), &meta, image)))
		{
			bLoaded = true;
		}
	}

	// 失敗
	if (bLoaded == false)
	{
		Release();
		return false;
	}

	// ミップマップ生成
	if (meta.mipLevels == 1 && generateMipmap)
	{
		DirectX::ScratchImage mipChain;
		if (SUCCEEDED(DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_DEFAULT, 0, mipChain)))
		{
			image.Release();
			image = std::move(mipChain);
		}
	}

	//------------------------------------
	// テクスチャリソース作成
	//------------------------------------
	D3D11_USAGE usage = D3D11_USAGE_DEFAULT;
	UINT cpuAccessFlags = 0;

	if (FAILED(DirectX::CreateTextureEx(
		D3D.GetDev(),						// Direct3D Device
		image.GetImages(),
		image.GetImageCount(),
		image.GetMetadata(),
		usage,								// Usage
		bindFlags,							// Bind Flags
		cpuAccessFlags,						// CPU Access Flags
		0,									// MiscFlags
		false,								// ForceSRGB
		(ID3D11Resource**)&m_resource)
	)) {
		Release();
		return false;
	}

	//------------------------------------
	// テクスチャリソース(m_resource)から、各ビューを作成する
	//------------------------------------
	if (CreateViewsFromResource() == false)
	{
		Release();
		return false;
	}

	m_filepath = filename;

	return true;
}

bool KdTexture::Create(const D3D11_TEXTURE2D_DESC & desc, const D3D11_SUBRESOURCE_DATA * fillData)
{
	Release();

	//--------------------------------------------
	// 2Dテクスチャの生成
	//--------------------------------------------
	HRESULT hr = D3D.GetDev()->CreateTexture2D(&desc, fillData, &m_resource);
	if (FAILED(hr)) {
		Release();
		return false;
	}

	//--------------------------------------------
	// m_pTexture2Dから、各ビューを作成する
	//--------------------------------------------
	if (CreateViewsFromResource() == false) {
		Release();
		return false;
	}

	return true;
}

bool KdTexture::Create(int w, int h, DXGI_FORMAT format, UINT arrayCnt, const D3D11_SUBRESOURCE_DATA * fillData)
{
	Release();

	// 作成する2Dテクスチャ設定
	D3D11_TEXTURE2D_DESC desc = {};
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.Format = format;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.Width = (UINT)w;
	desc.Height = (UINT)h;
	desc.CPUAccessFlags = 0;
	desc.MipLevels = 1;
	desc.ArraySize = arrayCnt;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;

	// 作成
	if (Create(desc, fillData) == false)return false;

	return true;
}

bool KdTexture::CreateRenderTarget(int w, int h, DXGI_FORMAT format, UINT arrayCnt, const D3D11_SUBRESOURCE_DATA * fillData)
{
	Release();

	// 作成する2Dテクスチャ設定
	D3D11_TEXTURE2D_DESC desc = {};
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.Format = format;
	desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	desc.Width = (UINT)w;
	desc.Height = (UINT)h;
	desc.CPUAccessFlags = 0;
	desc.MipLevels = 1;
	desc.ArraySize = arrayCnt;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;

	// 作成
	if (Create(desc, fillData) == false)return false;

	// ファイル名記憶
	m_filepath = "";

	return true;
}

bool KdTexture::CreateDepth(int w, int h, DXGI_FORMAT format, UINT arrayCnt, const D3D11_SUBRESOURCE_DATA* fillData)
{
	Release();

	// 2Dテクスチャの設定
	D3D11_TEXTURE2D_DESC desc = {};
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.Format = format;
	desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	desc.Width = (UINT)w;
	desc.Height = (UINT)h;
	desc.CPUAccessFlags = 0;
	desc.MipLevels = 1;
	desc.ArraySize = arrayCnt;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;

	// 作成
	if (Create(desc, fillData) == false)return false;

	// ファイル名記憶
	m_filepath = "";

	return true;
}

void KdTexture::ClearRenerTarget(const Math::Color & ColorRGBA)
{
	assert(m_rtv != nullptr && "RenderTargetじゃないのでClearできません");

	D3D.GetDevContext()->ClearRenderTargetView(m_rtv, ColorRGBA);
}

void KdTexture::ClearDepth(bool bDepth, bool bStencil, FLOAT depth, UINT8 stencil)
{
	assert(m_dsv != nullptr && "DepthStencilじゃないのでClearできません");

	UINT flags = 0;
	if (bDepth)flags |= D3D11_CLEAR_DEPTH;		// Zバッファ部分をクリア
	if (bStencil)flags |= D3D11_CLEAR_STENCIL;	// ステンシルバッファ部分をクリア

	D3D.GetDevContext()->ClearDepthStencilView(m_dsv, flags, depth, stencil);
}


void KdTexture::Release()
{
	KdSafeRelease(m_resource);
	KdSafeRelease(m_srv);
	KdSafeRelease(m_rtv);
	KdSafeRelease(m_dsv);

	m_filepath = "";
}

bool KdTexture::CreateViewsFromResource()
{
	// リソースが無い
	if (m_resource == nullptr)return false;

	// テクスチャ本体の情報取得
	m_resource->GetDesc(&m_desc);

	//===========================================================
	//
	// RenderTargetViewを作成する
	//
	//===========================================================
	KdSafeRelease(m_rtv);

	// レンダーターゲットフラグがついてる時のみ
	if (m_desc.BindFlags & D3D11_BIND_RENDER_TARGET)
	{
		// 作成するビューの設定
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = m_desc.Format;	// Format
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;			// 単品テクスチャ

		// レンダーターゲットビュー作成
		HRESULT hr = D3D.GetDev()->CreateRenderTargetView(m_resource, &rtvDesc, &m_rtv);
		if (FAILED(hr))
		{
			Release();
			assert(0 && "RenderTargetViewの作成に失敗");
			return false;
		}
	}

	//===========================================================
	//
	// ShaderResourceViewの情報を作成する
	//
	//===========================================================
	KdSafeRelease(m_srv);

	// シェーダリソースビューフラグがついてる時のみ
	if (m_desc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
	{
		// 作成するビューの設定
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};

		// テクスチャがZバッファの場合は、最適なフォーマットにする
		if (m_desc.BindFlags & D3D11_BIND_DEPTH_STENCIL)
		{
			switch (m_desc.Format) {
				// 16ビット
			case DXGI_FORMAT_R16_TYPELESS:
				srvDesc.Format = DXGI_FORMAT_R16_UNORM;
				break;
				// 32ビット
			case DXGI_FORMAT_R32_TYPELESS:
				srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
				break;
				// 24ビット(Zバッファ) + 8ビット(ステンシルバッファ) 
			case DXGI_FORMAT_R24G8_TYPELESS:
				srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
				break;
			default:
				assert(0 && "[ShaderResource] 対応していないフォーマットです");
				break;
			}
		}
		// Zバッファでない場合は、そのまま同じフォーマットを使用
		else
		{
			srvDesc.Format = m_desc.Format;
		}

		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = m_desc.MipLevels;
		if (srvDesc.Texture2D.MipLevels <= 0)srvDesc.Texture2D.MipLevels = -1;

		// シェーダリソースビュー作成
		HRESULT hr = D3D.GetDev()->CreateShaderResourceView(m_resource, &srvDesc, &m_srv);
		if (FAILED(hr))
		{
			Release();
			assert(0 && "ShaderResourceViewの作成に失敗");
			return false;
		}
	}

	//===========================================================
	//
	// DepthStencilViewを作成する
	//
	//===========================================================
	KdSafeRelease(m_dsv);

	// Zバッファフラグがついてる時のみ
	if (m_desc.BindFlags & D3D11_BIND_DEPTH_STENCIL) {
		// 作成するビューの設定
		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};

		// テクスチャー作成時に指定したフォーマットと互換性があり、深度ステンシルビューとして指定できるフォーマットを指定する
		switch (m_desc.Format) {
			// 16ビット
		case DXGI_FORMAT_R16_TYPELESS:
			dsvDesc.Format = DXGI_FORMAT_D16_UNORM;
			break;
			// 32ビット
		case DXGI_FORMAT_R32_TYPELESS:
			dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
			break;
			// 24ビット(Zバッファ) + 8ビット(ステンシルバッファ) 
		case DXGI_FORMAT_R24G8_TYPELESS:
			dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			break;
		default:
			assert(0 && "[DepthStencil] 対応していないフォーマットです");
			break;
		}

		dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Texture2D.MipSlice = 0;

		//-------------------------------
		// デプスステンシルビュー作成
		//-------------------------------
		HRESULT hr = D3D.GetDev()->CreateDepthStencilView(m_resource, &dsvDesc, &m_dsv);
		if (FAILED(hr)) {
			Release();
			assert(0 && "DepthStencilViewの作成に失敗");
			return false;
		}
	}

	return true;
}

void KdTexture::GetTextureInfo(ID3D11View * view, D3D11_TEXTURE2D_DESC & outDesc)
{
	outDesc = {};

	ID3D11Resource* res;
	view->GetResource(&res);

	ID3D11Texture2D* tex2D;
	if (SUCCEEDED(res->QueryInterface<ID3D11Texture2D>(&tex2D)))
	{
		tex2D->GetDesc(&outDesc);
		tex2D->Release();
	}
	res->Release();
}
