#include "Common.hlsli"
#include "Function.hlsli"


cbuffer MeshConstant : register(b0)
{
	matrix world;
	matrix worldIT;
	float4 pos;

	int useNormalMap;
	float3 padding;
}

cbuffer MatrialConstant : register(b1)
{
	float4 baseColor;
}

Texture2D t_combine : register(t6);
Texture2D t_filter : register(t7);

struct VSInput
{
	float3 pos : POSITION;
	float2 texcoord : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
};

struct VSOutput
{
	float4 posProj : SV_POSITION;
	float2 texcoord : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
};

VSOutput VS(VSInput input)
{
	VSOutput output;
	output.posProj = float4(input.pos, 1.f);
	output.texcoord = input.texcoord;
	output.normal = input.normal;

	return output;
}


float4 PS(VSOutput input) : SV_Target
{
	float3 color = t_combine.Sample(g_sampler, input.texcoord).xyz;
	// filter test
	//color = t_filter.Sample(g_sampler, input.texcoord).xyz;



	return float4(color, 1.f);
}