#include "pch.h"
#include "Light.h"
#include "Engine.h"
#include "Util.h"
#include "CommandManager.h"

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
	for (int i = 0; i < m_lightIndex; ++i)
	{
		LightInfo CurrentLight = m_constantData.lightInfo[i];
		Vector3 position = Vector3(CurrentLight.position.x,
			CurrentLight.position.y, CurrentLight.position.z);

		Matrix view = XMMatrixLookAtLH(position, Vector3(0.f, 0.f, 0.f),
			Vector3(0.f, 1.f, 0.f));
		Matrix proj = XMMatrixPerspectiveFovLH(XMConvertToRadians(90.f), 1.f, 1.f, 50.0f);
		m_constantData.lightInfo[i].view_L = view.Transpose();
		m_constantData.lightInfo[i].proj_L = proj.Transpose();
		m_constantData.lightInfo[i].viewProj_L = (view * proj).Transpose();
	
	}

	d3dUtil::UpdateConstBuffer(m_constantData, m_constantBuffer);
}

void Light::Render(ComPtr<ID3D12GraphicsCommandList>& cmdList)
{
	cmdList->SetGraphicsRootConstantBufferView(1, m_lightCBAddress);

	//CMD_MANAGER->GetCmdList(COMMANDLIST_TYPE::MAIN)->SetGraphicsRootConstantBufferView(1, m_lightCBAddress);
	//CMD_MANAGER->GetCmdList(COMMANDLIST_TYPE::SHADOW)->SetGraphicsRootConstantBufferView(1, m_lightCBAddress);

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
		break;
	case LIGHT_TYPE::POINT:
		info.lightType = 1;
		break;
	case LIGHT_TYPE::SPOT:
		info.lightType = 2;
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