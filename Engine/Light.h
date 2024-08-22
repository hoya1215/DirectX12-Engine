#pragma once
#include "Headers.h"

enum
{
	MAX_LIGHT_COUNT = 3
};

struct LightInfo
{
	float ambient;
	float diffuse;
	float specular;
	int lightType; // 0 : directional  /  1 : point  /  2 : spot

	Vector4 radiance;
	Vector4 position;
	Vector4 direction;

	Matrix view_L;
	Matrix proj_L;
	Matrix viewProj_L;
};

struct LightConstant
{
	LightInfo lightInfo[MAX_LIGHT_COUNT];
	int lightCount;
	Vector3 padding1;
};


class Light
{
public:
	Light();

	void Init();
	void Update();
	void Render(ComPtr<ID3D12GraphicsCommandList>& cmdList);

	bool AddLight(LIGHT_TYPE lightType, Vector4 radiance, Vector4 pos, Vector4 dir);
	void CreateCBV();

public:
	LightConstant m_constantData;

private:
	ComPtr<ID3D12Resource> m_constantBuffer;
	D3D12_GPU_VIRTUAL_ADDRESS m_lightCBAddress;
	static int m_lightIndex;
};

