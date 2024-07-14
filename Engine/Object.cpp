#include "pch.h"
#include "Object.h"
#include "Material.h"
#include "Util.h"

Object::Object(MESH_TYPE meshType, const wstring& path, 
	ComPtr<ID3D12DescriptorHeap>& m_cbvHeap,  Vector3 position, 
	float scale, Vector3 rotation)
{
	m_material = make_shared<Material>();
	m_material->Init(meshType, scale, path, m_cbvHeap);
	m_position = position;
	m_rotation = rotation;
	m_scale = Vector3(scale, scale, scale);
	m_material->m_constantData.world = Util::CreateMatrix(m_position, m_rotation, m_scale) ;
	m_material->m_constantData.pos = Vector4(0.2f, 0.2f, 0.f, 0.f);
	m_material->Update();
}

void Object::Update()
{
	if (m_material->b_dynamic)
	{
		m_material->m_constantData.world = Util::CreateMatrix(m_position, m_rotation, m_scale);
		m_material->Update();
	}

}