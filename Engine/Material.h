#pragma once
#include "ConstantBuffer.h"

enum class MESH_TYPE
{
	RECTANGLE,
	CIRCLE,
	BOX
};


class MeshBuffer;
 
class Material
{
public:
	void Init(MESH_TYPE meshType, float scale, const wstring& path, ComPtr<ID3D12DescriptorHeap>& m_cbvHeap);
	void Update();

	void CreateCBV(ComPtr<ID3D12DescriptorHeap>& m_cbvHeap);
	void CreateSRV(ComPtr<ID3D12DescriptorHeap>& m_cbvHeap);

	shared_ptr<MeshBuffer> GetMeshBuffer() { return m_meshBuffer; }

private:
	shared_ptr<MeshBuffer> m_meshBuffer;
	shared_ptr<class Texture> m_texture;

	uint32 m_descriptorHeapSize;
	static uint32 m_cbIndex;



public:
	MeshConstant m_constantData;
	ComPtr<ID3D12Resource> m_constantBuffer;

	// 정적, 동적
	bool b_dynamic = false;

};

