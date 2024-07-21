cbuffer GlobalConstant : register(b0)
{
	matrix view;
	matrix proj;
	matrix viewProj;
	float4 p;
}


SamplerState g_sampler : register(s0);