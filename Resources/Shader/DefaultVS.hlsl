
cbuffer GlobalConstant : register(b0)
{
	matrix view;
	matrix proj;
	matrix viewProj;
	float4 p;
}

cbuffer MeshConstant : register(b1)
{
	matrix world;
	float4 pos;
}

SamplerState g_sampler : register(s0);
Texture2D m_texture : register(t0);

struct VSInput
{
	float3 pos : POSITION;
	float2 texcoord : TEXCOORD;
	float4 color : COLOR;
};

struct VSOutput
{
	float4 posProj : SV_POSITION;
	float2 texcoord : TEXCOORD;
	float4 color : COLOR;
};


VSOutput VS(VSInput input)
{
	VSOutput output;

	output.posProj = float4(input.pos, 1.0);
	output.posProj = mul(output.posProj, world);
	//output.posProj = mul(output.posProj, view);
	output.posProj = mul(output.posProj, viewProj);
	//output.posProj += pos;
	output.color = input.color;
	output.texcoord = input.texcoord;

	return output;
}

float4 PS(VSOutput input) : SV_Target
{
	float4 color = m_texture.Sample(g_sampler, input.texcoord);
	if (color.r == 0)
		color = float4(1.0, 1.0, 1.0, 1.0);

	return color;
}