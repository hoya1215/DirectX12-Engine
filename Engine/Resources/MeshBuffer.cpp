#include "pch.h"
#include "MeshBuffer.h"
#include "MeshData.h"
#include "Engine.h"

void MeshBuffer::Create(VI_pair meshdata)
{
	CreateVertexBuffer(meshdata.first, DEVICE);
	CreateIndexBuffer(meshdata.second, DEVICE);
}

void MeshBuffer::CreateVertexBuffer(const vector<Vertex>& data, ComPtr<ID3D12Device>& device)
{
	// vertexBuffer

	m_vertexCount = static_cast<uint32>(data.size());
	uint32 vertexBufferSize = m_vertexCount * sizeof(Vertex);

	// 되는 코드
	D3D12_HEAP_PROPERTIES heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC vbdesc = CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize);

	device->CreateCommittedResource(
		&heapProperty,
		D3D12_HEAP_FLAG_NONE,
		&vbdesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&m_vertexBufferDefault));

	void* vertexData = nullptr;
	CD3DX12_RANGE readRange(0, 0);
	m_vertexBufferDefault->Map(0, &readRange, &vertexData);
	memcpy(vertexData, data.data(), vertexBufferSize);
	m_vertexBufferDefault->Unmap(0, nullptr);


	// 나중에 보완 디폴트 -> 임시 업로드 
	//m_vertexBufferDefault = d3dUtil::CreateDefaultBuffer(m_device,
	//	m_cmdList, meshdata.first.data(), vertexBufferSize, m_vertexBufferUpload);

	m_vertexBufferView.BufferLocation = m_vertexBufferDefault->GetGPUVirtualAddress();
	m_vertexBufferView.StrideInBytes = sizeof(Vertex);
	m_vertexBufferView.SizeInBytes = m_vertexCount * sizeof(Vertex);
}

void MeshBuffer::CreateIndexBuffer(const vector<uint16>& data, ComPtr<ID3D12Device>& device)
{
	// indexBuffer
// 	D3D12_HEAP_PROPERTIES heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	m_indexCount = static_cast<uint32>(data.size());
	uint32 indexBufferSize = m_indexCount * sizeof(uint16);
	D3D12_HEAP_PROPERTIES heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC iddesc = CD3DX12_RESOURCE_DESC::Buffer(indexBufferSize);

	device->CreateCommittedResource(
		&heapProperty,
		D3D12_HEAP_FLAG_NONE,
		&iddesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&m_indexBufferDefault));

	void* indexData = nullptr;
	//CD3DX12_RANGE readRange(0, 0);
	m_indexBufferDefault->Map(0, nullptr, &indexData);
	memcpy(indexData, data.data(), indexBufferSize);
	m_indexBufferDefault->Unmap(0, nullptr);
	// 

	////void* indexData = nullptr;
	////m_indexBufferDefault->Map(0, &readRange, &indexData);
	////memcpy(indexData, &meshdata.second[0], indexBufferSize);
	////m_indexBufferDefault->Unmap(0, nullptr);
	//m_indexBufferDefault = d3dUtil::CreateDefaultBuffer(m_device.Get(),
	//	m_cmdList.Get(), meshdata.second.data(), indexBufferSize, m_indexBufferUpload);

	m_indexBufferView.BufferLocation = m_indexBufferDefault->GetGPUVirtualAddress();
	m_indexBufferView.Format = DXGI_FORMAT_R16_UINT;
	m_indexBufferView.SizeInBytes = indexBufferSize;
}
