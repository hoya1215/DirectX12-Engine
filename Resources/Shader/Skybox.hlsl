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
    float3 posWorld : POSITION;
    float2 texcoord : TEXCOORD;
};

struct PSOutput
{
    float4 posWorld : SV_Target0;
    float4 normal : SV_Target1;
    float4 color : SV_Target2;
};

PSInput VS(VSInput input)
{
    PSInput output;

    output.posProj = mul(float4(input.pos, 1.f), world);
    output.posWorld = output.posProj;
    output.posProj = mul(output.posProj, viewProj);

    // w/w=1이기 때문에 항상 깊이가 1로 유지된다
    output.posProj = output.posProj.xyww;
    output.texcoord = input.texcoord;

    return output;
}

PSOutput PS(PSInput input) : SV_Target
{
    PSOutput output;

    //float4 color = float4(1.0f, 1.f, 1.f, 1.f);
     float4 TexColor = m_texture.Sample(g_sampler, input.texcoord);
     output.posWorld = float4(input.posWorld, 0.f);
     output.normal = float4(0.f, 0.f, 0.f, 0.f);
     output.color = TexColor;

     return output;
}

