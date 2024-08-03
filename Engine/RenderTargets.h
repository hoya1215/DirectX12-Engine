#pragma once

enum class RENDERTARGET_TYPE
{
	SWAPCHAIN,
	DEFERRED
};

enum
{
	SWAPCHAIN_COUNT = 2,
	DEFERRED_COUNT = 3
};

class RenderTargets
{
public:
	void Create();

	void ClearRenderTarget();

	void OMSetRenderTarget(const D3D12_CPU_DESCRIPTOR_HANDLE& dsvHandle);

public:
	ComPtr<ID3D12Resource> m_deferredRTVBuffer[DEFERRED_COUNT];
	ComPtr<ID3D12DescriptorHeap> m_deferredRTVHeap;
	ComPtr<ID3D12DescriptorHeap> m_deferredSRVHeap;
	uint32 m_deferredRTVHeapSize;
	D3D12_CPU_DESCRIPTOR_HANDLE m_deferredRTVHeapHandle;

	D3D12_GPU_DESCRIPTOR_HANDLE m_deferredSRVHeapStartHandle;
};

