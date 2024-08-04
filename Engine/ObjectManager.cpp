#include "pch.h"
#include "ObjectManager.h"
#include "Object.h"
#include "Engine.h"
#include "PipeLine.h"
#include "Frustum.h"

void ObjectManager::Init()
{
	// Object 만들어서 push
	shared_ptr<Object> m_test = make_shared<Object>(MESH_TYPE::SPHERE, L"D:\\DirectX12\\DirectX12\\Resources\\Textures\\me.png"
		, Vector3(0.f, 0.f, 5.f), "Object", PSO_TYPE::DEFERRED, true, 2.f);
	m_test->GetMaterial()->b_dynamic = true;
	m_test->GetMaterial()->m_materialConstantData.baseColor = Vector4(1.0f, 0.f, 0.f, 1.f);

	m_objects[m_test->m_psoType].push_back(m_test);

	shared_ptr<Object> m_test2 = make_shared<Object>(MESH_TYPE::SPHERE, L"D:\\DirectX12\\DirectX12\\Resources\\Textures\\sky.png"
		, Vector3(0.f, 0.f, 5.f), "Skybox", PSO_TYPE::SKYBOX, false, 200.f);
	m_test2->GetMaterial()->b_dynamic = true;
	m_test2->b_haveBoundingShape = false;

	m_objects[m_test2->m_psoType].push_back(m_test2);

	// 모든 물체 정의 후
	BeginPlay();
}

void ObjectManager::BeginPlay()
{
	for (auto it = m_objects.begin(); it != m_objects.end(); ++it)
	{
		for (auto& Object : it->second)
		{
			Object->BeginPlay();
		}
	}
}

void ObjectManager::Update()
{
	for (auto it = m_objects.begin(); it != m_objects.end(); ++it)
	{
		for (auto& Object : it->second)
		{
			Object->Update();
		}
	}
}

void ObjectManager::Render()
{
	for (auto it = m_objects.begin(); it != m_objects.end(); ++it)
	{
		CMD_LIST->SetPipelineState(PIPELINE->GetPSO(it->first).Get());

		for (auto& Object : it->second)
		{
			if(g_engine->GetFrustum()->FrustumCulling(Object))
				Object->Render();
		}
	}
}
