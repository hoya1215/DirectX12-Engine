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


Texture2D m_texture : register(t0);

struct VSInput
{
	float3 pos : POSITION;
	float2 texcoord : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	matrix insWorld : WORLD;
	matrix insWorldIT : WORLDIT;
};

struct VSOutput
{
	float4 posProj : SV_POSITION;
	float3 posWorld : POSITION;
	float2 texcoord : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
};



VSOutput VS(VSInput input)
{
	VSOutput output;
	output.posWorld = mul(float4(input.pos, 1.f), input.insWorld).xyz;
	output.posProj = mul(float4(output.posWorld, 1.0f), viewProj);

	output.texcoord = input.texcoord;
	output.normal = input.normal;
	
	output.normal = mul(float4(output.normal, 1.0f), input.insWorldIT).xyz;

	output.tangent = mul(float4(input.tangent, 1.0f), input.insWorld).xyz;



	return output;
}