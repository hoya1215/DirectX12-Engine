#pragma once

enum class RENDERTARGET_TYPE
{
	SWAPCHAIN,
	DEFERRED,
	COMBINE
};

enum
{
	SWAPCHAIN_COUNT = 2,
	DEFERRED_COUNT = 5,
	COMBINE_COUNT = 1,
	TOTAL_HEAP_COUNT = DEFERRED_COUNT + COMBINE_COUNT
};

class RenderTargets
{
public:
	void Create();
	void CreateDeferred();
	void CreateCombine();

	void ClearRenderTarget(ComPtr<ID3D12GraphicsCommandList>& cmdList);

	void OMSetRenderTarget(ComPtr<ID3D12GraphicsCommandList>& cmdList, const D3D12_CPU_DESCRIPTOR_HANDLE& dsvHandle);

	void OMSetCombineRenderTarget(ComPtr<ID3D12GraphicsCommandList>& cmdList);

public:
	uint32 m_RTVHeapIndex = 0;
	D3D12_CPU_DESCRIPTOR_HANDLE m_dsvHandle;

	// Deferred
	ComPtr<ID3D12Resource> m_deferredRTVBuffer[DEFERRED_COUNT];
	ComPtr<ID3D12DescriptorHeap> m_RTVHeap;
	ComPtr<ID3D12DescriptorHeap> m_SRVHeap;
	uint32 m_RTVHeapSize;
	D3D12_CPU_DESCRIPTOR_HANDLE m_deferredRTVHeapHandle;

	D3D12_GPU_DESCRIPTOR_HANDLE m_deferredSRVHeapStartHandle;

	// Combine
	ComPtr<ID3D12Resource> m_combineRTVBuffer;
	D3D12_CPU_DESCRIPTOR_HANDLE m_combineRTVHeapHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE m_combineSRVCPUHeapHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE m_combineSRVGPUHeapHandle;

};

