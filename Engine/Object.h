#pragma once
#include "Material.h"
#include "Component.h"

class Object : public enable_shared_from_this<Object>
{
public:
	Object(MESH_TYPE meshType, const wstring& path, 
		ComPtr<ID3D12DescriptorHeap>& m_cbvHeap,  Vector3 position, 
		float scale = 1.f, Vector3 rotation = Vector3(0.f, 0.f, 0.f));

	virtual void Update();

	shared_ptr<Material> GetMaterial() { return m_material; }
	Vector3 GetPosition() { return m_position; }

	void SetPosition(Vector3 pos) { m_position = pos; }
	void AddComponent(COMPONENT_TYPE componentType, shared_ptr<Component<Object>> component);

private:
	Vector3 m_position;
	Vector3 m_rotation;
	Vector3 m_scale;
	shared_ptr<Material> m_material;

	unordered_map<COMPONENT_TYPE, shared_ptr<Component<Object>>> m_component;
};

