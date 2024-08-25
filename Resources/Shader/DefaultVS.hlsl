#include "Common.hlsli"
#include "Function.hlsli"




Texture2D m_texture : register(t0);
Texture2D m_normalMap : register(t1);


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
	float3 posWorld : POSITION;
	float2 texcoord : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
};


VSOutput VS(VSInput input)
{
	VSOutput output;

	output.posProj = float4(input.pos, 1.0);
	output.posProj = mul(output.posProj, world);
	output.posWorld = output.posProj.xyz;
	//output.posProj = mul(output.posProj, view);
	output.posProj = mul(output.posProj, viewProj);
	//output.posProj += pos;
	output.normal = mul(float4(input.normal,1.0f), worldIT).xyz;
	output.tangent = mul(float4(input.tangent, 1.0f), world).xyz;
	output.texcoord = input.texcoord;



//#ifdef SHADOW
//
//	// 라이트 한개만
//	output.posWorld = input.pos;
//	output.posProj = mul(float4(input.pos, 1.0), viewProj_L);
//	output.texcoord = input.texcoord;
//	output.normal = input.normal;
//
//#endif

	return output;
}

float4 PS(VSOutput input) : SV_Target
{
	//float4 color = m_texture.Sample(g_sampler, input.texcoord);
	//if (color.r == 0)
	//	color = float4(1.0, 1.0, 1.0, 1.0);


	// light test
	//float3 color = float3(1.0f, 0.f, 0.f);
	//float3 lightColor = CalculateLight(0, input.normal, input.posWorld);
	//color = color * lightColor;




	return baseColor;
}