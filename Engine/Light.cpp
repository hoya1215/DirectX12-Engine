#include "pch.h"
#include "Light.h"
#include "Engine.h"

int Light::m_lightIndex = 0;

Light::Light()
{
	m_constantData.lightCount = MAX_LIGHT_COUNT;

	CreateCBV();
}

void Light::Init()
{

}

void Light::Update()
{
	d3dUtil::UpdateConstBuffer(m_constantData, m_constantBuffer);
}

void Light::Render()
{
	CMD_LIST->SetGraphicsRootConstantBufferView(1, m_lightCBAddress);
}

bool Light::AddLight(LIGHT_TYPE lightType, Vector4 radiance, Vector4 pos, Vector4 dir)
{
	if (m_lightIndex >= MAX_LIGHT_COUNT)
		return false;

	LightInfo info;
	switch (lightType)
	{
	case LIGHT_TYPE::DIRECTIONAL:
		info.lightType = 0;
		info.ambient = 0.2f;
		info.diffuse = 0.8f;
		info.specular = 0.5f;
		break;
	case LIGHT_TYPE::POINT:
		info.lightType = 1;
		info.ambient = 0.1f;
		info.diffuse = 0.7f;
		info.specular = 0.5f;
		break;
	case LIGHT_TYPE::SPOT:
		info.lightType = 2;
		info.ambient = 0.1f;
		info.diffuse = 0.9f;
		info.specular = 0.6f;
		break;
	}

	info.radiance = radiance;
	info.position = pos;
	info.direction = dir;

	m_constantData.lightInfo[m_lightIndex] = info;
	m_lightIndex++;
	
	return true;
}

void Light::CreateCBV()
{

	d3dUtil::CreateConstBuffer(DEVICE, m_constantData, m_constantBuffer, 1);
	m_lightCBAddress = m_constantBuffer->GetGPUVirtualAddress();

}