#include "pch.h"
#include "Material.h"
#include "MeshData.h"
#include "Mesh.h"
#include "MeshBuffer.h"
#include "Texture.h"
#include "Engine.h"


uint32 Material::m_cbIndex = 0;

void Material::Init(MESH_TYPE meshType, float scale, const wstring& path, ComPtr<ID3D12DescriptorHeap>& m_cbvHeap)
{
	VI_pair meshdata;

	switch (meshType)
	{
	case MESH_TYPE::RECTANGLE:
		meshdata = Mesh::CreateRectangle(scale);
		break;
	}

	// vertex, index Buffer
	m_meshBuffer = make_shared<MeshBuffer>();
	m_meshBuffer->Create(meshdata);


	// Texture
	m_texture = make_shared<Texture>();
	m_texture->CreateTexture(path, DEVICE, RES_CMD_LIST);

	// CBV , SRV
	m_descriptorHeapSize = DEVICE->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	CreateCBV(m_cbvHeap);
	CreateSRV(m_cbvHeap);
}

void Material::Update()
{
	d3dUtil::UpdateConstBuffer(m_constantData, m_constantBuffer);
}

void Material::CreateCBV(ComPtr<ID3D12DescriptorHeap>& m_cbvHeap)
{
	d3dUtil::CreateConstBuffer(DEVICE, m_constantData, m_constantBuffer, 1);

	D3D12_GPU_VIRTUAL_ADDRESS cbAddress = m_constantBuffer->GetGPUVirtualAddress();
	cbAddress += m_cbIndex * d3dUtil::CalcConstantBufferByteSize(sizeof(MeshConstant));

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
	cbvDesc.BufferLocation = cbAddress;
	cbvDesc.SizeInBytes = d3dUtil::CalcConstantBufferByteSize(sizeof(MeshConstant));

	CD3DX12_CPU_DESCRIPTOR_HANDLE cbvHandle(m_cbvHeap->GetCPUDescriptorHandleForHeapStart(), 0, m_descriptorHeapSize);
	DEVICE->CreateConstantBufferView(&cbvDesc, cbvHandle);
	m_cbIndex++;
}

void Material::CreateSRV(ComPtr<ID3D12DescriptorHeap>& m_cbvHeap)
{
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = m_texture->m_image.GetMetadata().format;
	//srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Texture2D.MipLevels = 1;

	CD3DX12_CPU_DESCRIPTOR_HANDLE srvHandle(m_cbvHeap->GetCPUDescriptorHandleForHeapStart(), 1, m_descriptorHeapSize);
	DEVICE->CreateShaderResourceView(m_texture->GetTextureBuffer().Get(), &srvDesc, srvHandle);
}