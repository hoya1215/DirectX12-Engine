#include "Common.hlsli"
#include "Function.hlsli"



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

struct VSOutput
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
	float4 color : SV_Target2; // albedo
	float4 PBR_Info1 : SV_Target3; // ao , metallic, roughness
	float4 PBR_Info2 : SV_Target4; // emission 
};


PSOutput PS(VSOutput input)
{
	//float4 color = m_texture.Sample(g_sampler, input.texcoord);
	//if (color.r == 0)
	//	color = float4(1.0, 1.0, 1.0, 1.0);
	PSOutput output;
	output.posWorld = float4(input.posWorld, 1.f);
	output.normal = float4(input.normal, 1.0f);

	if (useNormalMap)
	{
		float3 normalTex = t_normalMap.Sample(g_sampler, input.texcoord).xyz;
		normalTex = 2.0 * normalTex - 1.0;

		float3 N = input.normal;
		float3 T = normalize(input.tangent - dot(input.tangent, N) * N);
		float3 B = cross(N, T);

		float3x3 TBN = float3x3(T, B, N);
		output.normal = float4(normalize(mul(normalTex, TBN)), 1.0);
	}
	
	if (usePBR)
	{
		float ao = useAoMap ? t_ao.Sample(g_sampler, input.texcoord).r : 1.0;
		float metallic = useMetallicMap ? t_metallic.Sample(g_sampler, input.texcoord).r : metallicFactor;
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