#include "pch.h"
#include "Filter.h"
#include "Util.h"
#include "PipeLine.h"

Filter::Filter()
{
}

void Filter::Init(COMPUTE_PSO_TYPE computePSOType)
{
	m_computePSOType = computePSOType;

	Util::CreateResource(m_buffer, HEAP_TYPE::DEFAULT, RESOURCE_STATE::SRV, 1);

	uint32 srvIndex = OBJ_HEAP->GetSRVIndex();
	m_srvCPUHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(OBJ_HEAP->GetSRVHeap()->GetCPUDescriptorHandleForHeapStart(), srvIndex, OBJ_HEAP->GetHeapSize());
	m_srvGPUHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(OBJ_HEAP->GetSRVHeap()->GetGPUDescriptorHandleForHeapStart(), srvIndex, OBJ_HEAP->GetHeapSize());
	Util::CreateSRV(m_buffer, m_srvCPUHandle);

	uint32 uavIndex = OBJ_HEAP->GetUAVIndex();
	m_uavCPUHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(OBJ_HEAP->GetUAVHeap()->GetCPUDescriptorHandleForHeapStart(), uavIndex, OBJ_HEAP->GetHeapSize());
	m_uavGPUHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(OBJ_HEAP->GetUAVHeap()->GetGPUDescriptorHandleForHeapStart(), uavIndex, OBJ_HEAP->GetHeapSize());
	Util::CreateUAV(m_buffer, m_uavCPUHandle);
}

void Filter::Render()
{
	Util::ResourceStateTransition(m_buffer, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	CMD_LIST->SetComputeRootSignature(PIPELINE->GetRootSignature().Get());
	CMD_LIST->SetPipelineState(PIPELINE->GetComputePSO(COMPUTE_PSO_TYPE::FILTER).Get());

	CMD_LIST->SetComputeRootDescriptorTable(4, m_uavGPUHandle);
	CMD_LIST->Dispatch(UINT(ceil(g_engine->m_width / 32.0f)), UINT(ceil(g_engine->m_height / 32.0f)), 1);

	Util::ResourceStateTransition(m_buffer, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}
