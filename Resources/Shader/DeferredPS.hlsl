#include "Common.hlsli"
#include "Function.hlsli"


cbuffer MeshConstant : register(b0)
{
	matrix world;
	matrix worldIT;
	float4 pos;
}

cbuffer MatrialConstant : register(b1)
{
	float4 baseColor;
}

Texture2D m_texture : register(t0);

struct VSInput
{
	float3 pos : POSITION;
	float2 texcoord : TEXCOORD;
	float3 normal : NORMAL;
};

struct VSOutput
{
	float4 posProj : SV_POSITION;
	float3 posWorld : POSITION;
	float2 texcoord : TEXCOORD;
	float3 normal : NORMAL;
};

struct PSOutput
{
	float4 posWorld : SV_Target0;
	float4 normal : SV_Target1;
	float4 color : SV_Target2;
};


PSOutput PS(VSOutput input)
{
	//float4 color = m_texture.Sample(g_sampler, input.texcoord);
	//if (color.r == 0)
	//	color = float4(1.0, 1.0, 1.0, 1.0);
	PSOutput output;
	output.posWorld = float4(input.posWorld, 1.f);
	output.normal = float4(input.normal, 1.0f);
	


	float4 TexColor = m_texture.Sample(g_sampler, input.texcoord);
	output.color = TexColor;

	if (baseColor.w == 1.f)
		output.color = baseColor;
	

	 

	return output;
}