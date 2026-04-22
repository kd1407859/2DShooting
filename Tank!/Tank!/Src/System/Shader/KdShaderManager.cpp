#include "System/KdSystem.h"

#include "KdShaderManager.h"

void KdShaderManager::Init()
{

	//============================================
	// シェーダ
	//============================================

	// 基本シェーダ
	m_standardShader.Init();
	// エフェクトシェーダ
	m_effectShader.Init();
	// 2Dスプライトシェーダ
	m_spriteShader.Init();

	//============================================
	// 定数バッファ
	//============================================

	// カメラ
	m_cb7_Camera.Create();
	D3D.GetDevContext()->VSSetConstantBuffers( 7,   1, m_cb7_Camera.GetAddress());
	D3D.GetDevContext()->PSSetConstantBuffers( 7,   1, m_cb7_Camera.GetAddress());

	// 初期ビュー行列
	m_cb7_Camera.Work().mV = Math::Matrix::CreateTranslation(0, 0, -5);
	m_cb7_Camera.Work().mV = m_cb7_Camera.Work().mV.Invert();

	// 初期射影行列
	D3D11_TEXTURE2D_DESC desc;
	KdTexture::GetTextureInfo(D3D.GetBackBuffer(), desc);
	m_cb7_Camera.Work().mP = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(60), (float)desc.Width / desc.Height, 0.01f, 1000);


	// ライト
	m_cb8_Light.Create();
	D3D.GetDevContext()->VSSetConstantBuffers( 8,	1, m_cb8_Light.GetAddress());
	D3D.GetDevContext()->PSSetConstantBuffers( 8,	1, m_cb8_Light.GetAddress());

}

void KdShaderManager::Release()
{
	m_standardShader.Release();
	m_effectShader.Release();
	m_spriteShader.Release();

	m_cb7_Camera.Release();
	m_cb8_Light.Release();

}
