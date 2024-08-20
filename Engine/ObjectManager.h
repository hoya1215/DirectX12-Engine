#pragma once

class Object;

class ObjectManager
{
public:
	void Init();
	void BeginPlay();
	void Update();
	void Render();
	void ShadowRender(ComPtr<ID3D12GraphicsCommandList>& cmdList);

private:
	unordered_map<PSO_TYPE, vector<shared_ptr<Object>>> m_objects;
	uint32 CurrentObjectCount = -1; // Skybox Á¦¿Ü
};

