#include "Common.hlsli"



Texture2D t_texture : register(t0);
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

struct HSInput
{
	float3 pos : POSITION;
	float2 texcoord : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
};

struct DSInput
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

struct PatchOutput
{
	float edges[4]	: SV_TessFactor;
	float inside[2]	: SV_InsideTessFactor;
};



struct PSOutput
{
	float4 posWorld : SV_Target0;
	float4 normal : SV_Target1;
	float4 color : SV_Target2;
	float4 PBR_Info1 : SV_Target3; // ao , metallic, roughness
	float4 PBR_Info2 : SV_Target4; // emission 
};


// VS
HSInput VS(VSInput input)
{
	HSInput output;

    output.pos = input.pos;
    output.normal = input.normal;
	output.texcoord = input.texcoord;
	output.tangent = input.tangent;

	return output;
}

// HS
PatchOutput PatchPoint(InputPatch<HSInput, 4> input, uint PatchID : SV_PrimitiveID)
{
    float4 center =
        float4((input[0].pos + input[1].pos + input[2].pos + input[3].pos) * 0.25, 1.0);


    center = mul(center, world);

    float dis = length(center - eyePos);
    float disMin = 0.5f;
    float disMax = 2.0f;
    float tessLevel = 32.0;


	float tess = tessLevel * smoothstep(disMin, disMax, disMax - dis) + 1.0;


    PatchOutput output;

    output.edges[0] = tess;
    output.edges[1] = tess;
    output.edges[2] = tess;
    output.edges[3] = tess;

    output.inside[0] = tess;
    output.inside[1] = tess;


    return output;
}

[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("PatchPoint")] DSInput
HS(
	InputPatch<HSInput, 4> input,
	uint i : SV_OutputControlPointID,
	uint PatchID : SV_PrimitiveID)
{
	DSInput output;

	output.pos = input[i].pos;
	output.normal = input[i].normal;
	output.texcoord = input[i].texcoord;

	return output;
}


// DS
[domain("quad")] PSInput DS(PatchOutput input,
    float2 uv : SV_DomainLocation,
    const OutputPatch<DSInput, 4> quad)
{
    PSInput output;

    float3 n1 = lerp(quad[0].normal, quad[3].normal, uv.x);
    float3 n2 = lerp(quad[1].normal, quad[2].normal, uv.x);
    float3 n = lerp(n1, n2, uv.y);

    output.normal = n;
    output.normal = mul(float4(output.normal, 1.f), worldIT).xyz;

	float3 tan1 = lerp(quad[0].tangent, quad[3].tangent, uv.x);
	float3 tan2 = lerp(quad[1].tangent, quad[2].tangent, uv.x);
	float3 tan = lerp(n1, n2, uv.y);

	output.tangent = tan;
	output.tangent = mul(float4(output.tangent, 1.f), world).xyz;




    float2 t1 = lerp(quad[0].texcoord.xy, quad[3].texcoord.xy, uv.x);
    float2 t2 = lerp(quad[1].texcoord.xy, quad[2].texcoord.xy, uv.x);
    float2 t = lerp(t1, t2, uv.y);

    output.texcoord = t;


	float3 p1 = lerp(quad[0].pos, quad[3].pos, uv.x);
	float3 p2 = lerp(quad[1].pos, quad[2].pos, uv.x);
	float3 p = lerp(p1, p2, uv.y);

    output.posWorld = mul(float4(p, 1.f), world).xyz;
    output.posProj = mul(float4(output.posWorld, 1.0), viewProj);

    return output;
}

// PS
PSOutput PS(PSInput input)
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



	float4 TexColor = t_texture.Sample(g_sampler, input.texcoord);
	output.color = TexColor;

	if (baseColor.w == 1.f)
		output.color = baseColor;

	return output;
}