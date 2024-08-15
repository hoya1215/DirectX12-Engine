#include "pch.h"
#include "PostProcess.h"
#include "Object.h"
#include "Engine.h"
#include "PipeLine.h"
#include "DescriptorHeap.h"
#include "RenderTargets.h"

PostProcess::PostProcess()
{
	m_combineRectangle = make_shared<Object>(MESH_TYPE::RECTANGLE);
	m_postProcessRectangle = make_shared<Object>(MESH_TYPE::RECTANGLE);
}

void PostProcess::CombineRender(const D3D12_GPU_DESCRIPTOR_HANDLE& rtvGPUHandle)
{
	RT->OMSetCombineRenderTarget();


	CMD_LIST->SetPipelineState(PIPELINE->GetPSO(PSO_TYPE::COMBINE).Get());
	CMD_LIST->SetGraphicsRootDescriptorTable(3, rtvGPUHandle);
	CMD_LIST->SetGraphicsRootDescriptorTable(4, OBJ_HEAP->GetPostSRVHeap()->GetGPUDescriptorHandleForHeapStart());
	m_combineRectangle->Render();
}

void PostProcess::FXAA_Render()
{
	// Compute Shader 연결 , 필터 이어서 
}

void PostProcess::PostRender()
{
	// SRV combine , filter
	CMD_LIST->SetPipelineState(PIPELINE->GetPSO(PSO_TYPE::POST_PROCESS).Get());
	CMD_LIST->SetGraphicsRootDescriptorTable(4, OBJ_HEAP->GetPostSRVHeap()->GetGPUDescriptorHandleForHeapStart());
	m_postProcessRectangle->Render();
}