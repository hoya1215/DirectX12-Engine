#pragma once

struct GlobalConstant
{
	Matrix view;
	Matrix proj;
	Matrix viewProj;
	Matrix invViewProj;
	Vector3 eyePos;// World
	
};

struct MeshConstant
{
	Matrix world;
	Matrix worldIT;
	Matrix invTranspose;
	Vector4 pos; 


};

struct MaterialConstant
{

	Vector4 baseColor; // baseColor
	float metallicFactor = 0.1f;
	Vector3 emissionFactor = Vector3(0.f, 0.f, 0.f);
	float roughnessFactor = 0.5f;

	// state
	int useNormalMap = 0;
	int useAoMap = 0; 
	int useMetallicMap = 0;
	int useRoughnessMap = 0;
	int useEmissionMap = 0;
	float ambient = 0.2f;
	float diffuse = 0.8f;
	float specular = 0.5f;
	
	int usePBR = 0;
	Vector3 padding;
};