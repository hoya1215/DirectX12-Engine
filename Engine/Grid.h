#pragma once
#include "Object.h"



class Grid : public Object
{
public:
	Grid(MESH_TYPE meshType, const wstring& path = L"",
		Vector3 position = Vector3(0.f, 0.f, 0.f), const string& name = "Object", PSO_TYPE psoType = PSO_TYPE::DEFAULT,
		bool color = true, float scale = 1.f, Vector3 rotation = Vector3(0.f, 0.f, 0.f),
		int row = 10, int column = 10);

	virtual void Render(ComPtr<ID3D12GraphicsCommandList>& cmdList) override;
};

