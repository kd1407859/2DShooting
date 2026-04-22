#include "System/KdSystem.h"

#include "KdEffectShader.h"


void KdEffectShader::DrawLine(const Math::Vector3 & p1, const Math::Vector3 & p2, const Math::Color & color)
{
	Vertex vertex[2] = {
		{p1, {0,0}, color},
		{p2, {1,1}, color},
	};

	// 頂点を描画
	D3D.DrawVertices(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP, 2, &vertex[0], sizeof(Vertex));
}

bool KdEffectShader::Init()
{
	//-------------------------------------
	// 頂点シェーダ
	//-------------------------------------
	{
		// コンパイル済みのシェーダーヘッダーファイルをインクルード
		#include "KdEffectShader_VS.inc"

		// 頂点シェーダー作成
		if (FAILED(D3D.GetDev()->CreateVertexShader(compiledBuffer, sizeof(compiledBuffer), nullptr, &m_VS))) {
			assert(0 && "頂点シェーダー作成失敗");
			Release();
			return false;
		}

		// １頂点の詳細な情報
		std::vector<D3D11_INPUT_ELEMENT_DESC> layout = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,		0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,			0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR",	  0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		// 頂点入力レイアウト作成
		if (FAILED(D3D.GetDev()->CreateInputLayout(
			&layout[0],			// 入力エレメント先頭アドレス
			layout.size(),		// 入力エレメント数
			&compiledBuffer[0],				// 頂点バッファのバイナリデータ
			sizeof(compiledBuffer),			// 上記のバッファサイズ
			&m_inputLayout))					// 
		) {
			assert(0 && "CreateInputLayout失敗");
			Release();
			return false;
		}
	}


	//-------------------------------------
	// ピクセルシェーダ
	//-------------------------------------
	{
		// コンパイル済みのシェーダーヘッダーファイルをインクルード
		#include "KdEffectShader_PS.inc"

		if (FAILED(D3D.GetDev()->CreatePixelShader(compiledBuffer, sizeof(compiledBuffer), nullptr, &m_PS))) {
			assert(0 && "ピクセルシェーダー作成失敗");
			Release();
			return false;
		}
	}

	//-------------------------------------
	// 定数バッファ作成
	//-------------------------------------
	m_cb0.Create();

	return true;
}

void KdEffectShader::Release()
{
	KdSafeRelease(m_VS);
	KdSafeRelease(m_PS);
	KdSafeRelease(m_inputLayout);
	m_cb0.Release();
}

void KdEffectShader::SetToDevice()
{
	// 頂点シェーダをセット
	D3D.GetDevContext()->VSSetShader(m_VS, 0, 0);
	// 頂点レイアウトをセット
	D3D.GetDevContext()->IASetInputLayout(m_inputLayout);

	// ピクセルシェーダをセット
	D3D.GetDevContext()->PSSetShader(m_PS, 0, 0);

	// 定数バッファをセット
	D3D.GetDevContext()->VSSetConstantBuffers(0, 1, m_cb0.GetAddress());
	D3D.GetDevContext()->PSSetConstantBuffers(0, 1, m_cb0.GetAddress());
}

// テクスチャセット

void KdEffectShader::SetTexture(ID3D11ShaderResourceView * srv)
{
	if (srv)
	{
		D3D.GetDevContext()->PSSetShaderResources(0, 1, &srv);
	}
	else
	{
		// テクスチャが無い場合は、白テクスチャをセットする
		D3D.GetDevContext()->PSSetShaderResources(0, 1, D3D.GetWhiteTex()->GetSRViewAddress());
	}
}
