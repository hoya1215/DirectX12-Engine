#pragma once
#include "Material.h"


class Object
{
public:
	Object(MESH_TYPE meshType, const wstring& path, 
		ComPtr<ID3D12DescriptorHeap>& m_cbvHeap,  Vector3 position, 
		float scale = 1.f, Vector3 rotation = Vector3(0.f, 0.f, 0.f));

	virtual void Update();

	shared_ptr<Material> GetMaterial() { return m_material; }

private:
	Vector3 m_position;
	Vector3 m_rotation;
	Vector3 m_scale;
	shared_ptr<Material> m_material;

};

