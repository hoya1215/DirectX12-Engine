#pragma once
#include "ConstantBuffer.h"

enum class MESH_TYPE
{
	RECTANGLE,
	CIRCLE,
	BOX,
	SPHERE
};


class MeshBuffer;
 
class Material
{
public:
	void Init(MESH_TYPE meshType, float scale, const wstring& path);
	void Update();

	void CreateCBV();
	void CreateSRV();

	shared_ptr<MeshBuffer> GetMeshBuffer() { return m_meshBuffer; }
	D3D12_GPU_DESCRIPTOR_HANDLE GetCBVHandle() { return m_gpuCBVHandle; }
	D3D12_GPU_DESCRIPTOR_HANDLE GetSRVHandle() { return m_gpuSRVHandle; }

private:
	shared_ptr<MeshBuffer> m_meshBuffer;
	

	CD3DX12_CPU_DESCRIPTOR_HANDLE m_cbvHandle;
	CD3DX12_CPU_DESCRIPTOR_HANDLE m_srvHandle;
	
	D3D12_GPU_DESCRIPTOR_HANDLE m_gpuCBVHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE m_gpuSRVHandle;



public:
	MeshConstant m_meshConstantData;
	ComPtr<ID3D12Resource> m_meshConstantBuffer;

	MaterialConstant m_materialConstantData;
	ComPtr<ID3D12Resource> m_materialConstantBuffer;


	shared_ptr<class Texture> m_texture;
	// 정적, 동적
	bool b_dynamic = false;

};

