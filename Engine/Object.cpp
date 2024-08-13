#include "pch.h"
#include "Object.h"
#include "Material.h"
#include "Util.h"
#include "Engine.h"
#include "MeshBuffer.h"
#include "PipeLine.h"
#include "BoundingShape.h"


Object::Object(MESH_TYPE meshType , const wstring& path,
	Vector3 position, const string& name, PSO_TYPE psoType, bool color,
	float scale, Vector3 rotation, int row, int column)
{
	m_name = name;
	m_psoType = psoType;
	m_material = make_shared<Material>();
	m_material->Init(meshType, scale, path, row, column);
	m_position = position;
	m_rotation = rotation;
	m_scale = Vector3(scale, scale, scale);
	m_material->m_meshConstantData.world = Util::CreateMatrix(m_position, m_scale, m_rotation);
	m_material->m_meshConstantData.worldIT = m_material->m_meshConstantData.world;
	m_material->m_meshConstantData.worldIT = m_material->m_meshConstantData.worldIT.Transpose().Invert();
	m_material->m_meshConstantData.pos = Vector4(0.2f, 0.2f, 0.f, 0.f);

	if (color) // w 값으로 사용할지 말지 결정
		m_material->m_materialConstantData.baseColor = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	else
		m_material->m_materialConstantData.baseColor = Vector4(0.f, 0.f, 0.f, 0.f);

	m_material->Update();

	// 나중에 물체의 x y z 길이 달라질때 scale 매트릭스 추출해서 수정

}

void Object::BeginPlay()
{
	// BoundingShape
	if (b_haveBoundingShape)
	{
		shared_ptr<BoundingShape> m_bShape = make_shared<BoundingShape>(m_position, m_scale, max(max(m_scale.x, m_scale.y), m_scale.z));
		AddComponent(COMPONENT_TYPE::BOUNDING_SHAPE, m_bShape);
	}

}

void Object::Update()
{
	if (m_material->b_dynamic)
	{
		m_material->m_meshConstantData.world = Util::CreateMatrix(m_position, m_scale, m_rotation);
		m_material->Update();
	}

	for (auto it = m_component.begin(); it != m_component.end(); ++it)
	{
		it->second->Update();
	}

}

void Object::Render()
{
	CMD_LIST->IASetVertexBuffers(0, 1, &m_material->GetMeshBuffer()->GetVertexBufferView());
	//CMD_LIST->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
	CMD_LIST->IASetPrimitiveTopology(g_engine->GetPrimitiveType(m_material->m_primitiveType));
	CMD_LIST->SetGraphicsRootDescriptorTable(2, m_material->GetCBVHandle());

	if(m_material->m_texture != nullptr)
		CMD_LIST->SetGraphicsRootDescriptorTable(3, m_material->GetSRVHandle());

	if (m_material->m_primitiveType == PRIMITIVE_TYPE::POINT)
		CMD_LIST->DrawInstanced(m_material->GetMeshBuffer()->GetVertexCount(), 1, 0, 0);
	else
	{
		CMD_LIST->IASetIndexBuffer(&m_material->GetMeshBuffer()->GetIndexBufferView());
		CMD_LIST->DrawIndexedInstanced(m_material->GetMeshBuffer()->GetIndexCount(), 1, 0, 0, 0);
	}

}

void Object::AddComponent(COMPONENT_TYPE componentType, shared_ptr<Component<Object>> component)
{
	auto it = m_component.find(componentType);
	if (it != m_component.end())
		return;

	m_component.insert({ componentType, component });
	component->Init(shared_from_this());
}

shared_ptr<Component<Object>> Object::GetComponent(COMPONENT_TYPE componentType)
{
	auto it = m_component.find(componentType);
	if (it != m_component.end())
		return m_component[componentType];
	else
		return nullptr;
}
