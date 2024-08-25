#include "Common.hlsli"
#include "Function.hlsli"



Texture2D m_texture : register(t0);
Texture2D t_normalMap : register(t1);
Texture2D t_ao : register(t2);
Texture2D t_metallic : register(t3);
Texture2D t_roughness : register(t4);
Texture2D t_emission : register(t5);


struct VSInput
{
	float3 pos : POSITION;
	float2 texcoord : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
};

struct PSInput
{
	float4 posProj : SV_POSITION;
};


PSInput VS(VSInput input)
{
	PSInput output;


	float4 posWorld = mul(float4(input.pos, 1.0), world);

	//output.posProj = mul(posWorld, viewProj);
	output.posProj = mul(posWorld, lightInfo[0].viewProj_L);


	return output;
}

float4 PS(PSInput input) : SV_Target
{



	return float4(1.f, 1.f, 1.f, 1.f);
}