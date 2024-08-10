#include "pch.h"
#include "DescriptorHeap.h"
#include "Engine.h"
#include "Util.h"

void DescriptorHeap::Init()
{
	m_descriptorHeapSize = DEVICE->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	CreateObjHeap();
	CreatePostHeap();
	
}

void DescriptorHeap::CreateObjHeap()
{
	Util::CreateCBVHeap(m_cbvHeap, MAX_CBV_SIZE);
	Util::CreateSRVHeap(m_srvHeap, MAX_SRV_SIZE);
	Util::CreateUAVHeap(m_uavHeap, MAX_UAV_SIZE);
}

void DescriptorHeap::CreatePostHeap()
{
	Util::CreateSRVHeap(m_postSRVHeap, MAX_SRV_SIZE);
}

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::NewSRVHandle()
{
	uint32 Index = GetSRVIndex();
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(OBJ_HEAP->GetSRVHeap()->GetCPUDescriptorHandleForHeapStart(), Index, OBJ_HEAP->GetHeapSize());
}

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::NewUAVHandle()
{
	uint32 Index = GetUAVIndex();
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(OBJ_HEAP->GetUAVHeap()->GetCPUDescriptorHandleForHeapStart(), Index, OBJ_HEAP->GetHeapSize());
}
