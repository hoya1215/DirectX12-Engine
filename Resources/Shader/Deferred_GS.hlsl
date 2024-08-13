#include "Common.hlsli"

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

struct GSInput
{
	float3 pos : POSITION;
	float2 texcoord : TEXCOORD;
	float3 normal : NORMAL;
};

struct PSInput
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


GSInput VS(VSInput input)
{
	GSInput output;
	output.pos = input.pos;
	output.texcoord = input.texcoord;
	output.normal = input.normal;

	return output;
}

[maxvertexcount(4)]
void GS(point GSInput input[1] : SV_POSITION, inout TriangleStream< PSInput > outputStream)
{
	PSInput output;

	float3 center = input[0].pos;
	float d = 0.5f;



	output.posWorld = float3(input[0].pos.x - d,
		input[0].pos.y - d, input[0].pos.z);
	output.posWorld = mul(float4(output.posWorld, 1.0), world).xyz;
	output.posProj = mul(float4(output.posWorld, 1.0), viewProj);
	output.texcoord = float2(0.0, 1.0);
	output.normal = input[0].normal;
	outputStream.Append(output);

	output.posWorld = float3(input[0].pos.x - d,
		input[0].pos.y + d, input[0].pos.z);
	output.posWorld = mul(float4(output.posWorld, 1.0), world).xyz;
	output.posProj = mul(float4(output.posWorld, 1.0), viewProj);
	output.texcoord = float2(0.0, 0.0);
	output.normal = input[0].normal;
	outputStream.Append(output);

	output.posWorld = float3(input[0].pos.x + d,
	input[0].pos.y - d, input[0].pos.z);
	output.posWorld = mul(float4(output.posWorld, 1.0), world).xyz;
	output.posProj = mul(float4(output.posWorld, 1.0), viewProj);
	output.texcoord = float2(1.0, 1.0);
	output.normal = input[0].normal;
	outputStream.Append(output);

	output.posWorld = float3(input[0].pos.x + d,
	input[0].pos.y + d, input[0].pos.z);
	output.posWorld = mul(float4(output.posWorld, 1.0), world).xyz;
	output.posProj = mul(float4(output.posWorld, 1.0), viewProj);
	output.texcoord = float2(1.0, 0.0);
	output.normal = input[0].normal;
	outputStream.Append(output);


	//outputStream.RestartStrip();
	//
	//output.posWorld = float3(input[0].pos.x - d,
	//	input[0].pos.y - d, input[0].pos.z);
	//output.posWorld = mul(float4(output.posWorld, 1.0), world).xyz;
	//output.posProj = mul(float4(output.posWorld, 1.0), viewProj);
	//output.texcoord = float2(0.0, 1.0);
	//output.normal = input[0].normal;
	//outputStream.Append(output);
	//
	//output.posWorld = float3(input[0].pos.x + d,
	//	input[0].pos.y + d, input[0].pos.z);
	//output.posWorld = mul(float4(output.posWorld, 1.0), world).xyz;
	//output.posProj = mul(float4(output.posWorld, 1.0), viewProj);
	//output.texcoord = float2(1.0, 0.0);
	//output.normal = input[0].normal;
	//outputStream.Append(output);
	//
	//output.posWorld = float3(input[0].pos.x + d,
	//	input[0].pos.y - d, input[0].pos.z);
	//output.posWorld = mul(float4(output.posWorld, 1.0), world).xyz;
	//output.posProj = mul(float4(output.posWorld, 1.0), viewProj);
	//output.texcoord = float2(1.0, 1.0);
	//output.normal = input[0].normal;
	//outputStream.Append(output);
	//
	//outputStream.RestartStrip();
}

PSOutput PS(PSInput input)
{

	PSOutput output;
	output.posWorld = float4(input.posWorld, 1.f);
	output.normal = float4(input.normal, 1.0f);



	float4 TexColor = m_texture.Sample(g_sampler, input.texcoord);
	output.color = TexColor;

	if (baseColor.w == 1.f)
		output.color = baseColor;




	return output;
}