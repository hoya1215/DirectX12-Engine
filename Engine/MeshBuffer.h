#pragma once
#include "MeshData.h"
#include "Engine.h"


class MeshBuffer
{
public:
	void Create(VI_pair meshdata);

	void CreateVertexBuffer(const vector<Vertex>& data, ComPtr<ID3D12Device>& device);
	void CreateIndexBuffer(const vector<uint16>& data, ComPtr<ID3D12Device>& device);

	template <typename T>
	void CreateInstancingBuffer(const vector<T>& data)
	{
		m_instanceCount = data.size();

        uint32 instancingBufferSize = m_instanceCount * sizeof(T);
        D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(instancingBufferSize);


        D3D12_CLEAR_VALUE optimizedClearValue = {};
        //D3D12_RESOURCE_STATES resourceStates = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COMMON;
        D3D12_RESOURCE_STATES resourceStates = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ;
        float arrFloat[4] = { 0.f, 0.f, 0.f, 0.f };
        optimizedClearValue = CD3DX12_CLEAR_VALUE(DXGI_FORMAT_R16G16B16A16_FLOAT, arrFloat);

        D3D12_HEAP_PROPERTIES heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        D3D12_HEAP_FLAGS heapFlags = D3D12_HEAP_FLAG_NONE;


        // Create Texture2D

        HRESULT hr = DEVICE->CreateCommittedResource(
            &heapProperty,
            heapFlags,
            &desc,
            resourceStates,
            nullptr,
            IID_PPV_ARGS(&m_instancingBuffer));

        assert(SUCCEEDED(hr));

        void* pData = nullptr;
        D3D12_RANGE readRange = { 0, 0 };
        m_instancingBuffer->Map(0, &readRange, &pData);

        // 인스턴스 데이터 복사
        memcpy(pData, data.data(), sizeof(T) * data.size());

        // 맵핑 해제
        m_instancingBuffer->Unmap(0, nullptr);

        m_instancingBufferView.BufferLocation = m_instancingBuffer->GetGPUVirtualAddress();
        m_instancingBufferView.StrideInBytes = sizeof(T);
        m_instancingBufferView.SizeInBytes = sizeof(T) * m_instanceCount;
	}

	const D3D12_VERTEX_BUFFER_VIEW& GetVertexBufferView() { return m_vertexBufferView; }
	const D3D12_INDEX_BUFFER_VIEW& GetIndexBufferView() { return m_indexBufferView; }
    const D3D12_VERTEX_BUFFER_VIEW& GetInstancingBufferView() { return m_instancingBufferView; }
	uint32 GetVertexCount() { return m_vertexCount; }
	uint32 GetIndexCount() { return m_indexCount; }
    uint32 GetInstancingCount() { return m_instanceCount; }

private:
	ComPtr<ID3D12Resource> m_vertexBufferDefault = nullptr;
	ComPtr<ID3D12Resource> m_vertexBufferUpload = nullptr;
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;
	uint32 m_vertexCount = 0;

	ComPtr<ID3D12Resource> m_indexBufferDefault;
	ComPtr<ID3D12Resource> m_indexBufferUpload;
	D3D12_INDEX_BUFFER_VIEW m_indexBufferView;
	uint32 m_indexCount = 0;

	ComPtr<ID3D12Resource> m_instancingBuffer;
	D3D12_VERTEX_BUFFER_VIEW m_instancingBufferView;
	uint32 m_instanceCount = 0;

	
};

