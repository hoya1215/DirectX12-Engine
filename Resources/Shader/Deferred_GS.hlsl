#include "Common.hlsli"


Texture2D t_albedo : register(t0);
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

struct GSInput
{
	float3 pos : POSITION;
	float2 texcoord : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
};

struct PSInput
{
	float4 posProj : SV_POSITION;
	float3 posWorld : POSITION;
	float2 texcoord : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
};

struct PSOutput
{
	float4 posWorld : SV_Target0;
	float4 normal : SV_Target1;
	float4 color : SV_Target2;
	float4 PBR_Info1 : SV_Target3; // ao , metallic, roughness
	float4 PBR_Info2 : SV_Target4; // emission 
};


GSInput VS(VSInput input)
{
	GSInput output;
	output.pos = input.pos;
	output.texcoord = input.texcoord;
	output.normal = input.normal;
	output.tangent = input.tangent;

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
	output.tangent = input[0].tangent;
	outputStream.Append(output);

	output.posWorld = float3(input[0].pos.x - d,
		input[0].pos.y + d, input[0].pos.z);
	output.posWorld = mul(float4(output.posWorld, 1.0), world).xyz;
	output.posProj = mul(float4(output.posWorld, 1.0), viewProj);
	output.texcoord = float2(0.0, 0.0);
	output.normal = input[0].normal;
	output.tangent = input[0].tangent;
	outputStream.Append(output);

	output.posWorld = float3(input[0].pos.x + d,
	input[0].pos.y - d, input[0].pos.z);
	output.posWorld = mul(float4(output.posWorld, 1.0), world).xyz;
	output.posProj = mul(float4(output.posWorld, 1.0), viewProj);
	output.texcoord = float2(1.0, 1.0);
	output.normal = input[0].normal;
	output.tangent = input[0].tangent;
	outputStream.Append(output);

	output.posWorld = float3(input[0].pos.x + d,
	input[0].pos.y + d, input[0].pos.z);
	output.posWorld = mul(float4(output.posWorld, 1.0), world).xyz;
	output.posProj = mul(float4(output.posWorld, 1.0), viewProj);
	output.texcoord = float2(1.0, 0.0);
	output.normal = input[0].normal;
	output.tangent = input[0].tangent;
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

	if (usePBR)
	{
		float ao = useAoMap ? t_ao.Sample(g_sampler, input.texcoord).r : 1.0;
		float metallic = useMetallicMap ? t_metallic.Sample(g_sampler, input.texcoord).a : metallicFactor;
		float roughness = useRoughnessMap ? t_roughness.Sample(g_sampler, input.texcoord).r : roughnessFactor;
		float3 emission = useEmissionMap ? t_emission.Sample(g_sampler, input.texcoord).rgb : emissionFactor;

		output.PBR_Info1 = float4(ao, metallic, roughness, 1.0);
		output.PBR_Info2 = float4(emission, 1.0);
	}
	else
	{
		output.PBR_Info1 = float4(ambient, diffuse, specular, 0.0);
		output.PBR_Info2 = float4(0.0, 0.0, 0.0, 0.0);
	}


	float4 TexColor = t_albedo.Sample(g_sampler, input.texcoord);
	output.color = TexColor;

	if (baseColor.w == 1.f)
		output.color = baseColor;




	return output;
}