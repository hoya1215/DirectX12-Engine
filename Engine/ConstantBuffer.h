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
	Vector4 pos; 

	// state
	int m_useNormalMap;
	Vector3 padding;
};

struct MaterialConstant
{
	Vector4 baseColor;
};