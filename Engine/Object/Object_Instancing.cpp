#include "pch.h"
#include "Object_Instancing.h"
#include "Util.h"
#include "MeshBuffer.h"

Object_Instancing::Object_Instancing(MESH_TYPE meshType, const wstring& path,
	Vector3 position, uint32 size, const string& name, PSO_TYPE psoType,
	bool color, float scale, Vector3 rotation) : Object(meshType, path, position, name,
		psoType, color, scale, rotation)
{
	m_instancingData.resize(size);
	Vector3 startPosition = position;

	for (int i = 0; i < size; ++i)
	{
		startPosition.x += 1.f;
		m_instancingData[i].insWorld = Util::CreateMatrix(startPosition, Vector3(scale, scale, scale), rotation);
		m_instancingData[i].insWorldIT = m_instancingData[i].insWorld.Invert().Transpose();
	}

	GetMaterial()->GetMeshBuffer()->CreateInstancingBuffer(m_instancingData);
	
	
	//int32 srvIndex = OBJ_HEAP->GetSRVIndex();
	//m_instancingCpuSRVHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(OBJ_HEAP->GetSRVHeap()->GetCPUDescriptorHandleForHeapStart(), srvIndex, OBJ_HEAP->GetHeapSize());

	//Util::CreateSRV(m_instancingBuffer, m_instancingCpuSRVHandle,
	//	m_instancingData);
}

void Object_Instancing::BeginPlay()
{
	Object::BeginPlay();
}

void Object_Instancing::Update()
{
	Object::Update();

	// 인스턴싱 데이터 Update
}

void Object_Instancing::Render(ComPtr<ID3D12GraphicsCommandList>& cmdList)
{


	D3D12_VERTEX_BUFFER_VIEW vertexBuffers[] = { GetMaterial()->GetMeshBuffer()->GetVertexBufferView(),
		GetMaterial()->GetMeshBuffer()->GetInstancingBufferView()};

	cmdList->IASetVertexBuffers(0, _countof(vertexBuffers), vertexBuffers);
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	cmdList->SetGraphicsRootDescriptorTable(2, GetMaterial()->GetCBVHandle());

	if (GetMaterial()->m_baseTexture != nullptr)
		cmdList->SetGraphicsRootDescriptorTable(3, GetMaterial()->GetSRVHandle());

	cmdList->IASetIndexBuffer(&GetMaterial()->GetMeshBuffer()->GetIndexBufferView());
	cmdList->DrawIndexedInstanced(GetMaterial()->GetMeshBuffer()->GetIndexCount(), GetMaterial()->GetMeshBuffer()->GetInstancingCount(), 0, 0, 0);
}
