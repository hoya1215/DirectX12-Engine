#include "pch.h"
#include "Object.h"
#include "Material.h"
#include "Util.h"
#include "Engine.h"
#include "MeshBuffer.h"
#include "PipeLine.h"

Object::Object(MESH_TYPE meshType, const wstring& path, 
	Vector3 position, const string& name, PSO_TYPE psoType,
	float scale, Vector3 rotation)
{
	m_name = name;
	m_psoType = psoType;
	m_material = make_shared<Material>();
	m_material->Init(meshType, scale, path);
	m_position = position;
	m_rotation = rotation;
	m_scale = Vector3(scale, scale, scale);
	m_material->m_constantData.world = Util::CreateMatrix(m_position, m_scale, m_rotation);
	m_material->m_constantData.worldIT = m_material->m_constantData.world;
	m_material->m_constantData.worldIT = m_material->m_constantData.worldIT.Transpose().Invert();
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
	CMD_LIST->IASetVertexBuffers(0, 1, m_material->GetMeshBuffer()->GetVertexBufferView());
	//CMD_LIST->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
	CMD_LIST->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	CMD_LIST->SetGraphicsRootDescriptorTable(2, m_material->GetCBVHandle());

	if(m_material->m_texture != nullptr)
		CMD_LIST->SetGraphicsRootDescriptorTable(3, m_material->GetSRVHandle());

	CMD_LIST->IASetIndexBuffer(m_material->GetMeshBuffer()->GetIndexBufferView());
	CMD_LIST->DrawIndexedInstanced(m_material->GetMeshBuffer()->GetIndexCount(), 1, 0, 0, 0);
}

void Object::AddComponent(COMPONENT_TYPE componentType, shared_ptr<Component<Object>> component)
{
	auto it = m_component.find(componentType);
	if (it != m_component.end())
		return;

	m_component.insert({ componentType, component });
	component->Init(shared_from_this());
}