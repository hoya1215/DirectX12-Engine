#include "Common.hlsli"

cbuffer MeshConstant : register(b1)
{
    matrix world;
    float4 pos;
}

Texture2D m_texture : register(t0);

struct VSInput
{
    float3 pos : POSITION;
    float2 texcoord : TEXCOORD;
};

struct PSInput
{
    float4 posProj : SV_Position;
    float2 texcoord : TEXCOORD;
};

PSInput VS(VSInput input)
{
    PSInput output;

    output.posProj = mul(float4(input.pos, 1.f), world);
    output.posProj = mul(output.posProj, viewProj);

    // w/w=1이기 때문에 항상 깊이가 1로 유지된다
    //output.posProj = output.posProj.xyww;
    output.texcoord = input.texcoord;

    return output;
}

float4 PS(PSInput input) : SV_Target
{
    //float4 color = float4(1.0f, 1.f, 1.f, 1.f);
     float4 color = m_texture.Sample(g_sampler, input.texcoord);
     return color;
}

