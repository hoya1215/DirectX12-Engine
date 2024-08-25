#pragma once
#include "ConstantBuffer.h"

enum class MESH_TYPE
{
	RECTANGLE,
	CIRCLE,
	BOX,
	SPHERE,
	GRID,
	POINT
};

enum class TEXTURE_TYPE
{
	NORMAL,
	AO,
	METALLIC,
	ROUGHNESS,
	EMISSION
};


class MeshBuffer;
class Texture;
 
class Material
{
public:
	void Init(MESH_TYPE meshType, float scale, const wstring& path, int row = 10, int column = 10);
	
	void Update();

	void CreateCBV();
	void CreateSRV(shared_ptr<Texture> texture);

	void AddTexture(TEXTURE_TYPE textureType, const wstring& path);


	shared_ptr<MeshBuffer> GetMeshBuffer() { return m_meshBuffer; }
	D3D12_GPU_DESCRIPTOR_HANDLE GetCBVHandle() { return m_gpuCBVHandle; }
	D3D12_GPU_DESCRIPTOR_HANDLE GetSRVHandle() { return m_gpuSRVHandle; }

	void SetPrimitiveType() {}

private:
	shared_ptr<MeshBuffer> m_meshBuffer;
	

	CD3DX12_CPU_DESCRIPTOR_HANDLE m_cbvHandle;
	CD3DX12_CPU_DESCRIPTOR_HANDLE m_srvHandle;
	
	D3D12_GPU_DESCRIPTOR_HANDLE m_gpuCBVHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE m_gpuSRVHandle;

	int m_textureCount = 0;

public:
	MeshConstant m_meshConstantData;
	ComPtr<ID3D12Resource> m_meshConstantBuffer;

	MaterialConstant m_materialConstantData;
	ComPtr<ID3D12Resource> m_materialConstantBuffer;


	shared_ptr<Texture> m_baseTexture;
	shared_ptr<Texture> m_normalTexture;

	// 정적, 동적
	bool b_dynamic = false;

	PRIMITIVE_TYPE m_primitiveType = PRIMITIVE_TYPE::TRIANGLE;
};

