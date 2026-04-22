#include "../inc_KdCommon.hlsli"
#include "inc_KdEffectShader.hlsli"

// テクスチャ
Texture2D g_tex : register(t0);

// サンプラ
SamplerState g_ss : register(s0);

//================================
// エフェクト用ピクセルシェーダ
//================================
float4 main(VSOutput In) : SV_Target0
{
    return g_tex.Sample(g_ss, In.UV) * In.Color;
}
