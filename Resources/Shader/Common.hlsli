#ifndef __COMMON_HLSLI__
#define __COMMON_HLSLI__

#define MAX_LIGHT_COUNT 3


struct LightInfo
{
	float ambient;
	float diffuse;
	float specular;
	int lightType; // 0 : directional  /  1 : point  /  2 : spot

	float4 radiance;
	float4 position;
	float4 direction;
};


cbuffer GlobalConstant : register(b10)
{
	matrix view;
	matrix proj;
	matrix viewProj;
	matrix invViewProj;
	float3 eyePos;
}

cbuffer LightConstant : register(b11)
{
	LightInfo lightInfo[MAX_LIGHT_COUNT];
	int lightCount;
	float3 padding1;
}


SamplerState g_sampler : register(s0);

#endif