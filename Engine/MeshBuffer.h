#pragma once
#include "MeshData.h"


class MeshBuffer
{
public:
	void Create(VI_pair meshdata);

	void CreateVertexBuffer(const vector<Vertex>& data, ComPtr<ID3D12Device>& device);
	void CreateIndexBuffer(const vector<uint16>& data, ComPtr<ID3D12Device>& device);

	D3D12_VERTEX_BUFFER_VIEW* GetVertexBufferView() { return &m_vertexBufferView; }
	D3D12_INDEX_BUFFER_VIEW* GetIndexBufferView() { return &m_indexBufferView; }
	uint32 GetVertexCount() { return m_vertexCount; }
	uint32 GetIndexCount() { return m_indexCount; }

private:
	ComPtr<ID3D12Resource> m_vertexBufferDefault = nullptr;
	ComPtr<ID3D12Resource> m_vertexBufferUpload = nullptr;
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;
	uint32 m_vertexCount = 0;

	ComPtr<ID3D12Resource> m_indexBufferDefault;
	ComPtr<ID3D12Resource> m_indexBufferUpload;
	D3D12_INDEX_BUFFER_VIEW m_indexBufferView;
	uint32 m_indexCount = 0;


	
};

