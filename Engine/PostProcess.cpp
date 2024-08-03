#include "pch.h"
#include "PostProcess.h"
#include "Object.h"
#include "Engine.h"
#include "PipeLine.h"
#include "DescriptorHeap.h"
#include "RenderTargets.h"

PostProcess::PostProcess()
{
	m_rectangle = make_shared<Object>(MESH_TYPE::RECTANGLE);
	m_rectangle->m_psoType = PSO_TYPE::POST_PROCESS;
}

void PostProcess::Render(const D3D12_GPU_DESCRIPTOR_HANDLE& rtvGPUHandle)
{
	//ID3D12DescriptorHeap* descriptorHeaps[] = { OBJ_HEAP->GetCBVHeap().Get(), 
	//g_engine->m_deferred->m_deferredSRVHeap.Get()};
	//CMD_LIST->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	CMD_LIST->SetPipelineState(PIPELINE->GetPSO(m_rectangle->m_psoType).Get());
	CMD_LIST->SetGraphicsRootDescriptorTable(3, rtvGPUHandle);
	//CMD_LIST->SetGraphicsRootShaderResourceView(4, rtvGPUHandle.ptr);
	m_rectangle->Render();
}