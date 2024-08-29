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

	void CreateObjHeap();
	void CreatePostHeap(); 
	void CreateImGuiHeap();



	ComPtr<ID3D12DescriptorHeap> GetCBVHeap() { return m_cbvHeap; }
	ComPtr<ID3D12DescriptorHeap> GetSRVHeap() { return m_srvHeap; }
	ComPtr<ID3D12DescriptorHeap> GetUAVHeap() { return m_uavHeap; }
	ComPtr<ID3D12DescriptorHeap> GetPostSRVHeap() { return m_postSRVHeap; }
	ComPtr<ID3D12DescriptorHeap> GetImGuiSRVHeap() { return m_imguiSRVHeap; }

	uint32 GetCBVIndex() { return m_cbvIndex++; }
	uint32 GetSRVIndex() { return m_srvIndex++; }
	uint32 GetUAVIndex() { return m_uavIndex++; }

	uint32 GetPostSRVIndex() { return m_postSRVIndex++; }

	uint32 GetHeapSize() { return m_descriptorHeapSize; }
	D3D12_CPU_DESCRIPTOR_HANDLE NewSRVHandle();
	D3D12_CPU_DESCRIPTOR_HANDLE NewUAVHandle();



private:
	// 물체들의 CBV SRV Heap
	ComPtr<ID3D12DescriptorHeap> m_cbvHeap;
	ComPtr<ID3D12DescriptorHeap> m_srvHeap;
	ComPtr<ID3D12DescriptorHeap> m_uavHeap;

	// 분리 test
	ComPtr<ID3D12DescriptorHeap> m_postSRVHeap;

	// ImGui Heap
	ComPtr<ID3D12DescriptorHeap> m_imguiSRVHeap;

	uint32 m_cbvIndex = 0;
	uint32 m_srvIndex = 0;
	uint32 m_uavIndex = 0;
	uint32 m_descriptorHeapSize;

	uint32 m_postSRVIndex = 0;
	
};

