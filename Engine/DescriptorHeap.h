#pragma once

enum
{
	MAX_CBV_SIZE = 256,
	MAX_SRV_SIZE = 256
};

class DescriptorHeap
{
public:
	void Init();

	ComPtr<ID3D12DescriptorHeap> GetCBVHeap() { return m_cbvHeap; }
	ComPtr<ID3D12DescriptorHeap> GetSRVHeap() { return m_srvHeap; }
	uint32 GetCBVIndex() { return m_cbvIndex++; }
	uint32 GetSRVIndex() { return m_srvIndex++; }
	uint32 GetHeapSize() { return m_descriptorHeapSize; }



private:
	// 물체들의 CBV SRV Heap
	ComPtr<ID3D12DescriptorHeap> m_cbvHeap;
	ComPtr<ID3D12DescriptorHeap> m_srvHeap;

	uint32 m_cbvIndex = 0;
	uint32 m_srvIndex = 0;
	uint32 m_descriptorHeapSize;
	
};

