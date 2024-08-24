#include "pch.h"
#include "Material.h"
#include "MeshData.h"
#include "Mesh.h"
#include "MeshBuffer.h"
#include "Texture.h"
#include "Engine.h"
#include "DescriptorHeap.h"
#include "CommandManager.h"


void Material::Init(MESH_TYPE meshType, float scale, const wstring& path, int row, int column)
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
	case MESH_TYPE::GRID:
		meshdata = Mesh::CreateGrid(row, column, scale);
		break;
	case MESH_TYPE::POINT:
		meshdata = Mesh::CreatePoint();
		break;
	}

	// vertex, index Buffer
	m_meshBuffer = make_shared<MeshBuffer>();
	m_meshBuffer->Create(meshdata);


	// Texture
	if (path != L"")
	{
		m_baseTexture = make_shared<Texture>();
		m_baseTexture->CreateTexture(path, DEVICE, RES_CMD_LIST);
	}

	// CBV , SRV
	CreateCBV();
	if (m_baseTexture)
	{
		CreateSRV(m_baseTexture);
	}
}

void Material::Update()
{
	d3dUtil::UpdateConstBuffer(m_meshConstantData, m_meshConstantBuffer);
	d3dUtil::UpdateConstBuffer(m_materialConstantData, m_materialConstantBuffer);
}

void Material::CreateCBV()
{
	// Mesh
	d3dUtil::CreateConstBuffer(DEVICE, m_meshConstantData, m_meshConstantBuffer, 1);

	D3D12_GPU_VIRTUAL_ADDRESS cbAddress_mesh = m_meshConstantBuffer->GetGPUVirtualAddress();
	//cbAddress += m_cbIndex * d3dUtil::CalcConstantBufferByteSize(sizeof(MeshConstant));

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc_mesh;
	cbvDesc_mesh.BufferLocation = cbAddress_mesh;
	cbvDesc_mesh.SizeInBytes = d3dUtil::CalcConstantBufferByteSize(sizeof(MeshConstant));

	//m_cbvHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(m_cbvHeap->GetCPUDescriptorHandleForHeapStart(), 0, m_descriptorHeapSize);
	uint32 index_mesh = OBJ_HEAP->GetCBVIndex();
	m_cbvHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(OBJ_HEAP->GetCBVHeap()->GetCPUDescriptorHandleForHeapStart(), index_mesh, OBJ_HEAP->GetHeapSize());
	DEVICE->CreateConstantBufferView(&cbvDesc_mesh, m_cbvHandle);
	m_gpuCBVHandle = OBJ_HEAP->GetCBVHeap()->GetGPUDescriptorHandleForHeapStart();
	m_gpuCBVHandle.ptr += index_mesh * OBJ_HEAP->GetHeapSize();
	//m_cbIndex++;

	// Material
	d3dUtil::CreateConstBuffer(DEVICE, m_materialConstantData, m_materialConstantBuffer, 1);

	D3D12_GPU_VIRTUAL_ADDRESS cbAddress_material = m_materialConstantBuffer->GetGPUVirtualAddress();
	//cbAddress += m_cbIndex * d3dUtil::CalcConstantBufferByteSize(sizeof(MeshConstant));

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc_material;
	cbvDesc_material.BufferLocation = cbAddress_material;
	cbvDesc_material.SizeInBytes = d3dUtil::CalcConstantBufferByteSize(sizeof(MaterialConstant));

	//m_cbvHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(m_cbvHeap->GetCPUDescriptorHandleForHeapStart(), 0, m_descriptorHeapSize);
	uint32 index_material = OBJ_HEAP->GetCBVIndex();
	m_cbvHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(OBJ_HEAP->GetCBVHeap()->GetCPUDescriptorHandleForHeapStart(), index_material, OBJ_HEAP->GetHeapSize());
	DEVICE->CreateConstantBufferView(&cbvDesc_material, m_cbvHandle);
}

void Material::CreateSRV(shared_ptr<Texture> texture)
{
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = texture->m_image.GetMetadata().format;
	//srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Texture2D.MipLevels = 1;

	uint32 index = OBJ_HEAP->GetSRVIndex();
	
	CD3DX12_CPU_DESCRIPTOR_HANDLE Handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(OBJ_HEAP->GetSRVHeap()->GetCPUDescriptorHandleForHeapStart(), index, OBJ_HEAP->GetHeapSize());
	DEVICE->CreateShaderResourceView(texture->GetTextureBuffer().Get(), &srvDesc, Handle);

	if (m_textureCount == 0)
	{
		m_srvHandle = Handle;
		m_gpuSRVHandle = OBJ_HEAP->GetSRVHeap()->GetGPUDescriptorHandleForHeapStart();
		m_gpuSRVHandle.ptr += index * OBJ_HEAP->GetHeapSize();
	}

	m_textureCount++;


}

void Material::AddTexture(TEXTURE_TYPE textureType, const wstring& path)
{
	switch (textureType)
	{
	case TEXTURE_TYPE::NORMAL:
		m_normalTexture = make_shared<Texture>();
		m_normalTexture->CreateTexture(path, DEVICE, RES_CMD_LIST);
		CreateSRV(m_normalTexture);
		m_meshConstantData.m_useNormalMap = 1;
	}
}

