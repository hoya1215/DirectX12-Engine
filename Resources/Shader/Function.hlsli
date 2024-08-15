#ifndef __FUNCTION_HLSLI__
#define __FUNCTION_HLSLI__

#include "Common.hlsli"

float3 CalculateLight(int lightIndex, float3 normalWorld, float3 posWorld, Texture2D shadowMap)
{
	LightInfo light = lightInfo[lightIndex];

	float3 lightDir = -light.direction;
	float3 lightReflect = reflect(-lightDir, normalWorld);
	float3 viewDir = eyePos - posWorld;

	float ambient = light.ambient;
	float diffuse = dot(normalWorld, lightDir) * light.diffuse;
	float specular = pow(max(dot(lightReflect, viewDir), 0.0), 32) * light.specular;

    // ±×¸²ÀÚ
    float shadowFactor = 1.0;

    float4 posDepth = mul(float4(posWorld, 1.0), light.viewProj_L); 
    posDepth.xyz /= posDepth.w;
    float2 shadowTex = posDepth.xy;
    shadowTex.x = shadowTex.x * 0.5 + 0.5;
    shadowTex.y = -shadowTex.y * 0.5 + 0.5;

    int width, height, numMips;
    shadowMap.GetDimensions(0, width, height, numMips);
    float dx = 1.0 / (float)width;
    float percent = 0.0;
    //const float2 offsets[9] = { float2(-1, -1), float2(0, -1), float2(1, -1),
    //                           float2(-1, 0),  float2(0, 0),  float2(1, 0),
    //                           float2(-1, 1),  float2(0, 1),  float2(1, 1) };

    //[unroll] for (int i = 0; i < 9; i++) {
    //    percent += shadowMap
    //        .SampleCmpLevelZero(g_sampler,
    //            shadowTex.xy + offsets[i] * dx,
    //            posDepth.z - 0.00003)
    //        .r;
    //}
    //shadowFactor = percent / 9.0;

    float shadowDepth = shadowMap.Sample(g_sampler, shadowTex).r;
    if (shadowDepth + 0.02 < posDepth.z)
        shadowFactor = 0;
  
    //return float3(shadowDepth, shadowDepth, shadowDepth);

    float3 color = ambient * light.radiance + (diffuse + specular) * light.radiance * shadowFactor;
	//float3 color = (ambient + specular) * light.radiance + diffuse * light.radiance;
    //float3 color = float3(shadowFactor, shadowFactor, shadowFactor);

	return color;
}

#endif