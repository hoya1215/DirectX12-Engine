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

Texture2D t_position : register(t0);
Texture2D t_normal : register(t1);
Texture2D t_color : register(t2);
Texture2D t_shadowMap : register(t3);
Texture2D t_filter : register(t4);

struct VSInput
{
	float3 pos : POSITION;
	float2 texcoord : TEXCOORD;
	float3 normal : NORMAL;
};

struct VSOutput
{
	float4 posProj : SV_POSITION;
	float2 texcoord : TEXCOORD;
	float3 normal : NORMAL;
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
	float4 position = t_position.Sample(g_sampler, input.texcoord);
	float3 normal = t_normal.Sample(g_sampler, input.texcoord).xyz;
	float3 color = t_color.Sample(g_sampler, input.texcoord).xyz;
	//
	float3 posWorld = position.xyz;


	// light test
	// position 의 w 값으로 빛 계산 해줄지 말지 결정 
	if (position.w == 1.0f)
	{
		float3 lightColor = CalculateLight(0, normal, posWorld, t_shadowMap);
		color = color * lightColor;
	}

	// filter test
	//color = t_filter.Sample(g_sampler, input.texcoord).xyz;
	 
	// Shadow Map
	//float s = t_shadowMap.Sample(g_sampler, input.texcoord).r;
	//color = float3(s, s, s);
	//color = CalculateLight(0, normal, posWorld, t_shadowMap);

	return float4(color, 1.f);
}