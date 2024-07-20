#include "pch.h"
#include "Object.h"
#include "Material.h"
#include "Util.h"
#include "Engine.h"
#include "MeshBuffer.h"
#include "PipeLine.h"

Object::Object(MESH_TYPE meshType, const wstring& path, 
	Vector3 position, 
	float scale, Vector3 rotation)
{

	m_material = make_shared<Material>();
	m_material->Init(meshType, scale, path);
	m_position = position;
	m_rotation = rotation;
	m_scale = Vector3(scale, scale, scale);
	m_material->m_constantData.world = Util::CreateMatrix(m_position, m_scale, m_rotation);
	m_material->m_constantData.pos = Vector4(0.2f, 0.2f, 0.f, 0.f);
	m_material->Update();
}

void Object::Update()
{
	if (m_material->b_dynamic)
	{
		m_material->m_constantData.world = Util::CreateMatrix(m_position, m_scale, m_rotation);
		m_material->Update();
	}

	for (auto it = m_component.begin(); it != m_component.end(); ++it)
	{
		it->second->Update();
	}

}

void Object::Render()
{

	////ID3D12DescriptorHeap* descriptorHeaps[] = { m_cbvHeap.Get() };
	////CMD_LIST->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
	////CMD_LIST->SetGraphicsRootSignature(m_rootSignature.Get());

	////CMD_LIST->IASetVertexBuffers(0, 1, m_meshBuffer->GetVertexBufferView());
	//CMD_LIST->IASetVertexBuffers(0, 1, m_material->GetMeshBuffer()->GetVertexBufferView());
	////CMD_LIST->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
	//CMD_LIST->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//CMD_LIST->SetGraphicsRootConstantBufferView(0, m_globalCBAddress);
	//CMD_LIST->SetGraphicsRootDescriptorTable(1, m_cbvHeap->GetGPUDescriptorHandleForHeapStart());
	////CMD_LIST->IASetIndexBuffer(m_meshBuffer->GetIndexBufferView());
	//CMD_LIST->IASetIndexBuffer(m_test->GetMaterial()->GetMeshBuffer()->GetIndexBufferView());
	//CMD_LIST->DrawIndexedInstanced(m_test->GetMaterial()->GetMeshBuffer()->GetIndexCount(), 1, 0, 0, 0);
	////CMD_LIST->DrawIndexedInstanced(m_meshBuffer->GetIndexCount(), 1, 0, 0, 0);
}

void Object::AddComponent(COMPONENT_TYPE componentType, shared_ptr<Component<Object>> component)
{
	auto it = m_component.find(componentType);
	if (it != m_component.end())
		return;

	m_component.insert({ componentType, component });
	component->Init(shared_from_this());
}