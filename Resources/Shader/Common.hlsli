#ifndef __COMMON_HLSLI__
#define __COMMON_HLSLI__

#define MAX_LIGHT_COUNT 3

// Object Constant
cbuffer MeshConstant : register(b0)
{
	matrix world;
	matrix worldIT;
	float4 pos;
}

cbuffer MatrialConstant : register(b1)
{

	float4 baseColor;
	float metallicFactor;
	float3 emissionFactor;
	float roughnessFactor;

	// state
	int useNormalMap;
	int useAoMap;
	int useMetallicMap;
	int useRoughnessMap;
	int useEmissionMap;
	float ambient;
	float diffuse;
	float specular;

	int usePBR;
	float3 padding;
}

// Light Constant
struct LightInfo
{

	int lightType; // 0 : directional  /  1 : point  /  2 : spot
	float3 padding;

	float4 radiance;
	float4 position;
	float4 direction;

	matrix view_L;
	matrix proj_L;
	matrix viewProj_L;
};

// Global Constant
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