#include "pch.h"
#include "Material.h"
#include "MeshData.h"
#include "Mesh.h"
#include "MeshBuffer.h"
#include "Texture.h"
#include "Engine.h"
#include "DescriptorHeap.h"


void Material::Init(MESH_TYPE meshType, float scale, const wstring& path)
{
	VI_pair meshdata;

	switch (meshType)
	{
	case MESH_TYPE::RECTANGLE:
		meshdata = Mesh::CreateRectangle(scale);
		break;
	case MESH_TYPE::BOX:
		meshdata = Mesh::CreateBox(scale);
		break;
	case MESH_TYPE::SPHERE:
		meshdata = Mesh::CreateSphere(scale);
		break;
	}

	// vertex, index Buffer
	m_meshBuffer = make_shared<MeshBuffer>();
	m_meshBuffer->Create(meshdata);


	// Texture
	m_texture = make_shared<Texture>();
	m_texture->CreateTexture(path, DEVICE, RES_CMD_LIST);

	// CBV , SRV
	CreateCBV();
	CreateSRV();
}

void Material::Update()
{
	d3dUtil::UpdateConstBuffer(m_constantData, m_constantBuffer);
}

void Material::CreateCBV()
{
	d3dUtil::CreateConstBuffer(DEVICE, m_constantData, m_constantBuffer, 1);

	D3D12_GPU_VIRTUAL_ADDRESS cbAddress = m_constantBuffer->GetGPUVirtualAddress();
	//cbAddress += m_cbIndex * d3dUtil::CalcConstantBufferByteSize(sizeof(MeshConstant));

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
	cbvDesc.BufferLocation = cbAddress;
	cbvDesc.SizeInBytes = d3dUtil::CalcConstantBufferByteSize(sizeof(MeshConstant));

	//m_cbvHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(m_cbvHeap->GetCPUDescriptorHandleForHeapStart(), 0, m_descriptorHeapSize);
	uint32 index = OBJ_HEAP->GetCBVIndex();
	m_cbvHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(OBJ_HEAP->GetCBVHeap()->GetCPUDescriptorHandleForHeapStart(), index, OBJ_HEAP->GetHeapSize());
	DEVICE->CreateConstantBufferView(&cbvDesc, m_cbvHandle);
	m_gpuCBVHandle = OBJ_HEAP->GetCBVHeap()->GetGPUDescriptorHandleForHeapStart();
	m_gpuCBVHandle.ptr += index * OBJ_HEAP->GetHeapSize();
	//m_cbIndex++;
}

void Material::CreateSRV()
{
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = m_texture->m_image.GetMetadata().format;
	//srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Texture2D.MipLevels = 1;

	uint32 index = OBJ_HEAP->GetSRVIndex();
	m_srvHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(OBJ_HEAP->GetSRVHeap()->GetCPUDescriptorHandleForHeapStart(), index, OBJ_HEAP->GetHeapSize());
	DEVICE->CreateShaderResourceView(m_texture->GetTextureBuffer().Get(), &srvDesc, m_srvHandle);

	m_gpuSRVHandle = OBJ_HEAP->GetSRVHeap()->GetGPUDescriptorHandleForHeapStart();
	m_gpuSRVHandle.ptr += index * OBJ_HEAP->GetHeapSize();
}