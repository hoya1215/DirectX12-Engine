#include "pch.h"
#include "RenderTargets.h"
#include "Util.h"
#include "DescriptorHeap.h"

void RenderTargets::Create()
{
	m_deferredRTVHeapSize = DEVICE->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	Util::CreateResource(m_deferredRTVBuffer, DEFERRED_COUNT);

	m_deferredSRVHeapStartHandle = OBJ_HEAP->GetSRVHeap()->GetGPUDescriptorHandleForHeapStart();

	UINT srvDescriptorSize = DEVICE->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	for (int i = 0; i < DEFERRED_COUNT; ++i)
	{
		int32 index = OBJ_HEAP->GetSRVIndex();
		D3D12_CPU_DESCRIPTOR_HANDLE srvHandle = OBJ_HEAP->GetSRVHeap()->GetCPUDescriptorHandleForHeapStart();
		srvHandle.ptr += index * srvDescriptorSize;

		Util::CreateSRV(m_deferredRTVBuffer[i], srvHandle);

		if (i == 0)
			m_deferredSRVHeapStartHandle.ptr += index * srvDescriptorSize;
	}
	Util::CreateRTV(m_deferredRTVBuffer, m_deferredRTVHeap, DEFERRED_COUNT);

	//Util::CreateRTVAndSRV(m_deferredRTVBuffer, m_deferredRTVHeap, m_deferredSRVHeap, DEFERRED_COUNT);
}

void RenderTargets::ClearRenderTarget()
{
	float color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	for (int i = 0; i < DEFERRED_COUNT; ++i)
	{
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(m_deferredRTVHeap->GetCPUDescriptorHandleForHeapStart(), i * m_deferredRTVHeapSize);
		CMD_LIST->ClearRenderTargetView(rtvHandle, color, 0, nullptr);
	}
}

void RenderTargets::OMSetRenderTarget(const D3D12_CPU_DESCRIPTOR_HANDLE& dsvHandle)
{

	D3D12_CPU_DESCRIPTOR_HANDLE handle = m_deferredRTVHeap->GetCPUDescriptorHandleForHeapStart();
	CMD_LIST->OMSetRenderTargets(DEFERRED_COUNT, &handle, TRUE, &dsvHandle);
}

