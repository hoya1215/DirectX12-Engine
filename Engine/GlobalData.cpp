#include "pch.h"
#include "GlobalData.h"
#include "Engine.h"
#include "Camera.h"

void GlobalData::Init()
{
	CreateGlobalConstantData();
}

void GlobalData::Update()
{
	m_globalConstantData.view = MAIN_CAMERA->m_view;
	m_globalConstantData.proj = MAIN_CAMERA->m_proj;
	m_globalConstantData.viewProj = MAIN_CAMERA->m_view * MAIN_CAMERA->m_proj;

	d3dUtil::UpdateConstBuffer(m_globalConstantData, m_globalConstantBuffer);
}

void GlobalData::Render()
{
	CMD_LIST->SetGraphicsRootConstantBufferView(0, m_globalCBAddress);
}

void GlobalData::CreateGlobalConstantData()
{

	m_globalConstantData.pos = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
	d3dUtil::CreateConstBuffer(DEVICE, m_globalConstantData, m_globalConstantBuffer, 1);
	m_globalCBAddress = m_globalConstantBuffer->GetGPUVirtualAddress();

}
