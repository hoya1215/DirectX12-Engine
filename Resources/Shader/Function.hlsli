#ifndef __FUNCTION_HLSLI__
#define __FUNCTION_HLSLI__

#include "Common.hlsli"

float3 CalculateLight(int lightIndex, float3 normalWorld, float3 posWorld)
{
	LightInfo light = lightInfo[lightIndex];

	float3 lightDir = -light.direction;
	float3 lightReflect = reflect(-lightDir, normalWorld);
	float3 viewDir = eyePos - posWorld;

	float ambient = light.ambient;
	float diffuse = dot(normalWorld, lightDir) * light.diffuse;
	float specular = pow(max(dot(lightReflect, viewDir), 0.0), 32) * light.specular;

	float3 color = (ambient + specular) * light.radiance + diffuse * light.radiance;

	return color;
}

#endif