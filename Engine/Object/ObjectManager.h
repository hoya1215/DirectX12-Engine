#pragma once

class Object;
class ObjectPool;

class ObjectManager
{
public:
	void Init();
	void BeginPlay();
	void Update();
	void Render(ComPtr<ID3D12GraphicsCommandList>& cmdList);
	void ShadowRender(ComPtr<ID3D12GraphicsCommandList>& cmdList);

	void Render1(ComPtr<ID3D12GraphicsCommandList>& cmdList);
	void Render2(ComPtr<ID3D12GraphicsCommandList>& cmdList);
	void ShadowRender1(ComPtr<ID3D12GraphicsCommandList>& cmdList);
	void ShadowRender2(ComPtr<ID3D12GraphicsCommandList>& cmdList);

	bool AddObject(PSO_TYPE psoType);
	void DeleteObject(int objectIndex);

private:
	unordered_map<PSO_TYPE, list<shared_ptr<Object>>> m_objects;
	unordered_map<PSO_TYPE, vector<shared_ptr<Object>>> m_objectlist1;
	unordered_map<PSO_TYPE, vector<shared_ptr<Object>>> m_objectlist2;

	shared_ptr<ObjectPool> m_objectPool;

	uint32 CurrentObjectCount = -1; // Skybox Á¦¿Ü

	int m_objectCount = -1;
	int m_objectIndex = 0;

	int32 m_maxObjectPoolCount = 3;
};

