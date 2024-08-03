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
	m_globalConstantData.view = MAIN_CAMERA->m_view.Transpose();
	m_globalConstantData.proj = MAIN_CAMERA->m_proj.Transpose();
	m_globalConstantData.viewProj = (MAIN_CAMERA->m_view * MAIN_CAMERA->m_proj).Transpose();
	m_globalConstantData.invViewProj = (MAIN_CAMERA->m_view * MAIN_CAMERA->m_proj).Invert().Transpose();
	m_globalConstantData.eyePos = MAIN_CAMERA->GetPosition();

	d3dUtil::UpdateConstBuffer(m_globalConstantData, m_globalConstantBuffer);
}

void GlobalData::Render()
{
	CMD_LIST->SetGraphicsRootConstantBufferView(0, m_globalCBAddress);
}

void GlobalData::CreateGlobalConstantData()
{
	d3dUtil::CreateConstBuffer(DEVICE, m_globalConstantData, m_globalConstantBuffer, 1);
	m_globalCBAddress = m_globalConstantBuffer->GetGPUVirtualAddress();
}
