#pragma once

enum
{
	MAX_CBV_SIZE = 256,
	MAX_SRV_SIZE = 256,
	MAX_UAV_SIZE = 256
};

class DescriptorHeap
{
public:
	void Init();

	ComPtr<ID3D12DescriptorHeap> GetCBVHeap() { return m_cbvHeap; }
	ComPtr<ID3D12DescriptorHeap> GetSRVHeap() { return m_srvHeap; }
	ComPtr<ID3D12DescriptorHeap> GetUAVHeap() { return m_uavHeap; }
	uint32 GetCBVIndex() { return m_cbvIndex++; }
	uint32 GetSRVIndex() { return m_srvIndex++; }
	uint32 GetUAVIndex() { return m_uavIndex++; }
	uint32 GetHeapSize() { return m_descriptorHeapSize; }
	D3D12_CPU_DESCRIPTOR_HANDLE NewSRVHandle();
	D3D12_CPU_DESCRIPTOR_HANDLE NewUAVHandle();



private:
	// 물체들의 CBV SRV Heap
	ComPtr<ID3D12DescriptorHeap> m_cbvHeap;
	ComPtr<ID3D12DescriptorHeap> m_srvHeap;
	ComPtr<ID3D12DescriptorHeap> m_uavHeap;

	uint32 m_cbvIndex = 0;
	uint32 m_srvIndex = 0;
	uint32 m_uavIndex = 0;
	uint32 m_descriptorHeapSize;
	
};

