#ifndef __FUNCTION_HLSLI__
#define __FUNCTION_HLSLI__

#include "Common.hlsli"



float3 CalculateLight(int lightIndex, float3 normalWorld, float3 posWorld, 
    float ambientC, float diffuseC, float specularC, Texture2D shadowMap)
{
	LightInfo light = lightInfo[lightIndex];

	//float3 lightDir = -light.direction;
    float3 lightDir = normalize(light.position.xyz - posWorld);
	float3 lightReflect = reflect(-lightDir, normalWorld);
	float3 viewDir = normalize(eyePos - posWorld);

	float ambient = ambientC;
	float diffuse = dot(normalWorld, lightDir) * diffuseC;
	float specular = pow(max(dot(lightReflect, viewDir), 0.0), 32) * specularC;


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

float3 SchlickFresnel(float3 F0, float NdotH)
{
    //return F0 + (1.0 - F0) * pow(2.0, (-5.55473 * NdotH - 6.98316) * NdotH);
    return F0 + (1.0 - F0) * pow(clamp(1.0 - NdotH, 0.0, 1.0), 5.0);
}

float NdfGGX(float NdotH, float roughness)
{
    float alpha = roughness * roughness;
    float alphaSq = alpha * alpha;
    float denom = (NdotH * NdotH) * (alphaSq - 1.0) + 1.0;

    return alphaSq / (3.141592 * denom * denom);
}


float SchlickG1(float NdotV, float k)
{
    return NdotV / (NdotV * (1.0 - k) + k);
}

float SchlickGGX(float NdotL, float NdotV, float roughness)
{
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;
    return SchlickG1(NdotL, k) * SchlickG1(NdotV, k);

}




float3 CalculatePBR(
    int lightIndex,
    float3 normalWorld,
    float3 posWorld,
    float3 albedo,
    float ao,
    float metallic,
    float roughness,
    float3 emission,
    Texture2D shadowMap)
{
    LightInfo light = lightInfo[lightIndex];

    float3 lightDir = -light.direction;
    //float3 lightDir = normalize(light.position.xyz - posWorld);
    float3 lightReflect = normalize(reflect(-lightDir, normalWorld));
    float3 viewDir = normalize(eyePos - posWorld);

    // PBR
    float3 halfDir = normalize(viewDir + lightDir);

    float NdotL = max(0.0, dot(normalWorld, lightDir));
    //NdotL = 1.0;
    float NdotH = max(0.0, dot(normalWorld, halfDir));
    float NdotV = max(0.0, dot(normalWorld, viewDir));

    // Diffuse BRDF
    float3 Fdielectric = float3(0.04, 0.04, 0.04);
    float3 F0 = lerp(Fdielectric, albedo, metallic);
    float3 F = SchlickFresnel(F0, max(0.0, dot(halfDir, viewDir)));
    float3 kd = lerp(float3(1, 1, 1) - F, float3(0, 0, 0), metallic);
    kd = float3(1, 1, 1) - F;
    kd = kd * float3(1 - metallic, 1 - metallic, 1 - metallic);
    float3 diffuseBRDF = kd * albedo / 3.14;

    // Specular BRDF
    float D = NdfGGX(NdotH, roughness);
    float3 G = SchlickGGX(NdotL, NdotV, roughness);
    float3 specularBRDF = (F * D * G) / max(0.0001, 4.0 * NdotL * NdotV);

    float3 color = (diffuseBRDF + specularBRDF) * NdotL * light.radiance;
    color = kd * albedo + color;

    color = color / (color + float3(1.0f, 1.0f, 1.0f));
    color = pow(color, float3(1.0f / 2.2f, 1.0f / 2.2f, 1.0f / 2.2f));
    //float ambient = albedo * 0.2;
    //color += ambient;

    //color = diffuseBRDF  * light.radiance;
   // color = specularBRDF  * light.radiance;

    return color;
}

#endif