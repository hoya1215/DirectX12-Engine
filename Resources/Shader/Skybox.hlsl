#include "Common.hlsli"



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
    float4 PBR_Info1 : SV_Target3; // ao , metallic, roughness
    float4 PBR_Info2 : SV_Target4; // emission 
};

PSInput VS(VSInput input)
{
    PSInput output;

    output.posProj = mul(float4(input.pos, 1.f), world);
    output.posWorld = output.posProj;
    output.posProj = mul(output.posProj, viewProj);

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
     output.PBR_Info1 = float4(0.f, 0.f, 0.f, 0.f);
     output.PBR_Info2 = float4(0.f, 0.f, 0.f, 0.f);

     return output;
}

