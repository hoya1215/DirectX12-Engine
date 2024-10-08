#pragma once
#include "Object.h"

	struct ObjectInstancingData
	{
		Matrix insWorld;
		Matrix insWorldIT;
	};

class Object_Instancing : public Object
{
public:
	Object_Instancing(MESH_TYPE meshType, const wstring& path = L"",
		Vector3 position = Vector3(0.f, 0.f, 0.f), uint32 size = 10 , const string& name = "Object", PSO_TYPE psoType = PSO_TYPE::INSTANCING,
		bool color = true, float scale = 1.f, Vector3 rotation = Vector3(0.f, 0.f, 0.f));

	virtual void BeginPlay() override;
	virtual void Update() override;
	virtual void Render(ComPtr<ID3D12GraphicsCommandList>& cmdList) override;

public:
	vector<ObjectInstancingData> m_instancingData;

};

