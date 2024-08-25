#include "pch.h"
#include "RenderTargets.h"
#include "Util.h"
#include "DescriptorHeap.h"
#include "CommandManager.h"

void RenderTargets::Create()
{
	m_RTVHeapSize = DEVICE->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	Util::CreateRTVHeap(m_RTVHeap, TOTAL_HEAP_COUNT);

	CreateDeferred();
	CreateCombine();

}

void RenderTargets::CreateDeferred()
{
	// Deferred
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
	m_deferredRTVHeapHandle = m_RTVHeap->GetCPUDescriptorHandleForHeapStart();

	Util::CreateRTV(m_deferredRTVBuffer,m_deferredRTVHeapHandle , DEFERRED_COUNT);
	m_RTVHeapIndex += DEFERRED_COUNT;
}

void RenderTargets::CreateCombine()
{
	// Combine
	Util::CreateResource(m_combineRTVBuffer);
	m_combineRTVHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(m_RTVHeap->GetCPUDescriptorHandleForHeapStart(), m_RTVHeapIndex, m_RTVHeapSize);
	DEVICE->CreateRenderTargetView(m_combineRTVBuffer.Get(), nullptr, m_combineRTVHeapHandle);
	m_RTVHeapIndex++;

	uint32 combineIndex = OBJ_HEAP->GetPostSRVIndex();
	m_combineSRVCPUHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(OBJ_HEAP->GetPostSRVHeap()->GetCPUDescriptorHandleForHeapStart(), combineIndex, OBJ_HEAP->GetHeapSize());
	Util::CreateSRV(m_combineRTVBuffer, m_combineSRVCPUHeapHandle);

	m_combineSRVGPUHeapHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(OBJ_HEAP->GetPostSRVHeap()->GetGPUDescriptorHandleForHeapStart(), combineIndex, OBJ_HEAP->GetHeapSize());
}

void RenderTargets::ClearRenderTarget(ComPtr<ID3D12GraphicsCommandList>& cmdList)
{
	float color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	for (int i = 0; i < DEFERRED_COUNT; ++i)
	{
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(m_RTVHeap->GetCPUDescriptorHandleForHeapStart(), i * m_RTVHeapSize);
		cmdList->ClearRenderTargetView(rtvHandle, color, 0, nullptr);
		//CMD_MANAGER->GetCmdList(COMMANDLIST_TYPE::MAIN)->ClearRenderTargetView(rtvHandle, color, 0, nullptr);
	}

	cmdList->ClearRenderTargetView(m_combineRTVHeapHandle, color, 0, nullptr);
	//CMD_MANAGER->GetCmdList(COMMANDLIST_TYPE::MAIN)->ClearRenderTargetView(m_combineRTVHeapHandle, color, 0, nullptr);
}

void RenderTargets::OMSetRenderTarget(ComPtr<ID3D12GraphicsCommandList>& cmdList, const D3D12_CPU_DESCRIPTOR_HANDLE& dsvHandle)
{
	m_dsvHandle = dsvHandle;

	D3D12_CPU_DESCRIPTOR_HANDLE handle = m_RTVHeap->GetCPUDescriptorHandleForHeapStart();
	cmdList->OMSetRenderTargets(DEFERRED_COUNT, &handle, TRUE, &dsvHandle);

	//CMD_MANAGER->GetCmdList(COMMANDLIST_TYPE::MAIN)->OMSetRenderTargets(DEFERRED_COUNT, &handle, TRUE, &dsvHandle);
}

void RenderTargets::OMSetCombineRenderTarget(ComPtr<ID3D12GraphicsCommandList>& cmdList)
{
	cmdList->OMSetRenderTargets(COMBINE_COUNT, &m_combineRTVHeapHandle, FALSE, nullptr);
	//CMD_MANAGER->GetCmdList(COMMANDLIST_TYPE::MAIN)->OMSetRenderTargets(COMBINE_COUNT, &m_combineRTVHeapHandle, FALSE, nullptr);
	//CMD_LIST->OMSetRenderTargets(COMBINE_COUNT, &m_combineRTVHeapHandle, FALSE, nullptr);
}

