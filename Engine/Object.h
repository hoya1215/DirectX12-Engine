#pragma once
#include "Material.h"
#include "Component.h"

class Object : public enable_shared_from_this<Object>
{
public:
	Object(MESH_TYPE meshType, const wstring& path = L"",
		 Vector3 position = Vector3(0.f, 0.f, 0.f), const string& name = "Object", PSO_TYPE psoType = PSO_TYPE::DEFAULT,
		float scale = 1.f, Vector3 rotation = Vector3(0.f, 0.f, 0.f));


	virtual void Update();

	virtual void Render();

	void AddComponent(COMPONENT_TYPE componentType, shared_ptr<Component<Object>> component);




	shared_ptr<Material> GetMaterial() { return m_material; }
	Vector3 GetPosition() { return m_position; }
	string GetName() { return m_name; }

	void SetPosition(Vector3 pos) { m_position = pos; }
	void SetName(string name) { m_name = name; }


public:
	PSO_TYPE m_psoType;

private:

	// Transform
	Vector3 m_position;
	Vector3 m_rotation;
	Vector3 m_scale;

	// Material
	shared_ptr<Material> m_material;


	// Component
	unordered_map<COMPONENT_TYPE, shared_ptr<Component<Object>>> m_component;

	string m_name;

};

