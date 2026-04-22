#include "System/KdSystem.h"

#include "KdDirect3DHelper.h"


// 深度ステンシルステート作成
ID3D11DepthStencilState* KdCreateDepthStencilState(ID3D11Device* device, bool zEnable, bool zWriteEnable)
{
	// Zバッファの動作設定
	D3D11_DEPTH_STENCIL_DESC desc = {};

	desc.DepthEnable = zEnable;							// 深度テストを使用する
	desc.DepthWriteMask = zWriteEnable ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
	desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	desc.StencilEnable = FALSE;
	desc.StencilReadMask = 0;
	desc.StencilWriteMask = 0;

	// ステートオブジェクトを作成
	ID3D11DepthStencilState* state = nullptr;
	if (FAILED(device->CreateDepthStencilState(&desc, &state)))return nullptr;

	return state;
}

ID3D11RasterizerState* KdCreateRasterizerState(ID3D11Device* device, D3D11_CULL_MODE cullMode, D3D11_FILL_MODE fillMode, bool depthClipEnable, bool scissorEnable)
{
	// ラスタライズの動作設定
	D3D11_RASTERIZER_DESC desc = {};
	desc.FillMode = fillMode;
	desc.CullMode = cullMode;
	desc.FrontCounterClockwise = FALSE;
	desc.DepthBias = 0;
	desc.DepthBiasClamp = 0;
	desc.SlopeScaledDepthBias = 0;
	desc.DepthClipEnable = depthClipEnable;
	desc.ScissorEnable = scissorEnable;
	desc.MultisampleEnable = FALSE;
	desc.AntialiasedLineEnable = FALSE;

	// ステートオブジェクト作成
	ID3D11RasterizerState* state = nullptr;
	if (FAILED(device->CreateRasterizerState(&desc, &state)))return nullptr;

	return state;
}

ID3D11SamplerState* KdCreateSamplerState(ID3D11Device* device, SamplerFilterMode filterType, UINT maxAnisotropy, SamplerAddressingMode addressingMode, bool comparisonModel)
{
	D3D11_SAMPLER_DESC desc = {};
	// フィルタ
	switch (filterType) {
		// Point
	case SamplerFilterMode::Point:
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		break;
		// Linear
	case SamplerFilterMode::Linear:
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		break;
		// Anisotropic
	case SamplerFilterMode::Anisotropic:
		desc.Filter = D3D11_FILTER_ANISOTROPIC;
		desc.MaxAnisotropy = maxAnisotropy;
		break;
	}

	// アドレッシングモード
	switch (addressingMode) {
		// Wrap
	case SamplerAddressingMode::Wrap:
		desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		break;
		// Clamp
	case SamplerAddressingMode::Clamp:
		desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		break;
	}

	// 比較モードサンプラ
	if (comparisonModel) {
		desc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
		desc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;	// とりあえず今回は線形フィルタ固定で
	}
	// 通常サンプラ
	else {
		desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	}

	// 
	desc.MipLODBias = 0;
	desc.BorderColor[0] = desc.BorderColor[1] = desc.BorderColor[2] = desc.BorderColor[3] = 0;
	desc.MinLOD = 0;
	desc.MaxLOD = D3D11_FLOAT32_MAX;

	// ステートオブジェクト作成
	ID3D11SamplerState* state = nullptr;
	if (FAILED(device->CreateSamplerState(&desc, &state)))return nullptr;

	return state;
}

ID3D11BlendState* KdCreateBlendState(ID3D11Device* device, BlendMode mode)
{
	D3D11_BLEND_DESC desc = {};
	// 有効
	desc.RenderTarget[0].BlendEnable = TRUE;
	// 書き込みマスク　R,G,B,A全て出力する場合はALL
	desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	desc.IndependentBlendEnable = FALSE;
	desc.AlphaToCoverageEnable = FALSE;

	// ブレンドなし
	if (mode == BlendMode::NoBlend)
	{
		// 色の合成方法
		desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
		// アルファの合成方法
		desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	}
	// 半透明ブレンド
	else if (mode == BlendMode::Alpha)
	{
		// 色の合成方法
		desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		// アルファの合成方法
		desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
	}
	// 加算ブレンド
	else if (mode == BlendMode::Add)
	{
		// 色の合成方法
		desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		// アルファの合成方法
		desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
	}
	// ステンシル
	else if (mode == BlendMode::Stencil)
	{
		// 色の合成方法
		desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ZERO;
		desc.RenderTarget[0].DestBlend = D3D11_BLEND_SRC_COLOR;
		// アルファの合成方法
		desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
	}

	// ステートオブジェクト作成
	ID3D11BlendState* state = nullptr;
	if (FAILED(device->CreateBlendState(&desc, &state)))return nullptr;

	return state;

}

