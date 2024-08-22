#include "pch.h"
#include "ObjectManager.h"
#include "Object.h"
#include "Engine.h"
#include "PipeLine.h"
#include "Frustum.h"
#include "Object_Instancing.h"
#include "Grid.h"
#include "CommandManager.h"

void ObjectManager::Init()
{
	// Object 만들어서 push
	for (int i = 0; i < 700; ++i)
	{
		shared_ptr<Object> m_test = make_shared<Object>(MESH_TYPE::SPHERE, L"D:\\DirectX12\\DirectX12\\Resources\\Textures\\me.png"
			, Vector3(-10.f + 0.1 * i, 0.f, 5.f), "Object", PSO_TYPE::DEFERRED, true, 2.f);
		m_test->GetMaterial()->b_dynamic = true;
		m_test->GetMaterial()->m_materialConstantData.baseColor = Vector4(1.0f, 0.f, 0.f, 1.f);

		if (m_test->m_index % 2 == 0)
			m_objectPool1[m_test->m_psoType].push_back(m_test);
		else
			m_objectPool2[m_test->m_psoType].push_back(m_test);

		m_objects[m_test->m_psoType].push_back(m_test);
	}

	shared_ptr<Object> m_test = make_shared<Object>(MESH_TYPE::SPHERE, L"D:\\DirectX12\\DirectX12\\Resources\\Textures\\me.png"
		, Vector3(0.f, 0.f, 5.f), "Object", PSO_TYPE::DEFERRED, true, 2.f);
	m_test->GetMaterial()->b_dynamic = true;
	m_test->GetMaterial()->m_materialConstantData.baseColor = Vector4(1.0f, 0.f, 0.f, 1.f);

	if (m_test->m_index % 2 == 0)
		m_objectPool1[m_test->m_psoType].push_back(m_test);
	else
		m_objectPool2[m_test->m_psoType].push_back(m_test);

	m_objects[m_test->m_psoType].push_back(m_test);



	shared_ptr<Object> m_test3 = make_shared<Object>(MESH_TYPE::BOX, L"D:\\DirectX12\\DirectX12\\Resources\\Textures\\me.png"
		, Vector3(0.f, -6.f, 5.f), "Object", PSO_TYPE::DEFERRED, true, 2.f);
	m_test3->GetMaterial()->b_dynamic = true;
	m_test3->GetMaterial()->m_materialConstantData.baseColor = Vector4(1.0f, 1.f, 1.f, 1.f);

	if (m_test3->m_index % 2 == 0)
		m_objectPool1[m_test3->m_psoType].push_back(m_test3);
	else
		m_objectPool2[m_test3->m_psoType].push_back(m_test3);
	m_objects[m_test3->m_psoType].push_back(m_test3);

	shared_ptr<Object> m_test2 = make_shared<Object>(MESH_TYPE::SPHERE, L"D:\\DirectX12\\DirectX12\\Resources\\Textures\\sky.png"
		, Vector3(0.f, 0.f, 5.f), "Skybox", PSO_TYPE::SKYBOX, false, 200.f);
	m_test2->GetMaterial()->b_dynamic = true;
	m_test2->b_haveBoundingShape = false;
	m_test2->b_shadow = false;


	if (m_test2->m_index % 2 == 0)
		m_objectPool1[m_test2->m_psoType].push_back(m_test2);
	else
		m_objectPool2[m_test2->m_psoType].push_back(m_test2);
	m_objects[m_test2->m_psoType].push_back(m_test2);

	// shadow test
	/*shared_ptr<Object> m_shadowTest = make_shared<Object>(MESH_TYPE::RECTANGLE, L"D:\\DirectX12\\DirectX12\\Resources\\Textures\\me.png"
		, Vector3(0.f, -10.f, 5.f), "Object", PSO_TYPE::DEFERRED, true, 3.f);
	m_shadowTest->GetMaterial()->b_dynamic = true;
	m_shadowTest->GetMaterial()->m_materialConstantData.baseColor = Vector4(1.0f, 1.f, 1.f, 1.f);*/
	//m_shadowTest->m_rotation = Vector3(90.f, 0.f, 0.f);

	//m_objects[m_shadowTest->m_psoType].push_back(m_shadowTest);


	//shared_ptr<Object> m_gs = make_shared<Object>(MESH_TYPE::POINT);
	//m_gs->GetMaterial()->b_dynamic = true;
	//m_gs->GetMaterial()->m_materialConstantData.baseColor = Vector4(0.f, 0.f, 1.f, 1.f);
	//m_gs->SetPipelineType(PSO_TYPE::DEFERRED_GS);
	//m_gs->SetPrimitiveType(PRIMITIVE_TYPE::POINT);

	
	//m_objects[m_gs->m_psoType].push_back(m_gs);


	// HS , DS Test
	//shared_ptr<Grid> m_grid = make_shared<Grid>(MESH_TYPE::GRID);
	//m_grid->GetMaterial()->b_dynamic = true;
	//m_grid->GetMaterial()->m_materialConstantData.baseColor = Vector4(0.f, 0.f, 1.f, 1.f);
	//m_grid->SetPipelineType(PSO_TYPE::DEFERRED_TS);

	//m_objects[m_grid->m_psoType].push_back(m_grid);

	// Instance Object
	//shared_ptr<Object_Instancing> m_instancingTest = make_shared<Object_Instancing>(MESH_TYPE::SPHERE,
	//	L"D:\\DirectX12\\DirectX12\\Resources\\Textures\\me.png", Vector3(2.f, 0.f, 3.f), 10, "Object",
	//	PSO_TYPE::INSTANCING, true, 2.f);
	//m_instancingTest->GetMaterial()->b_dynamic = true;
	//m_instancingTest->GetMaterial()->m_materialConstantData.baseColor = Vector4(0.f, 1.f, 0.f, 1.f);
	//
	//m_objects[m_instancingTest->m_psoType].push_back(m_instancingTest);

	for (auto it = m_objects.begin(); it != m_objects.end(); ++it)
	{
		m_objectCount += it->second.size();
	}


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

void ObjectManager::Render(ComPtr<ID3D12GraphicsCommandList>& cmdList)
{

	for (auto it = m_objects.begin(); it != m_objects.end(); ++it)
	{
		cmdList->SetPipelineState(PIPELINE->GetPSO(it->first).Get());
		//CMD_MANAGER->GetCmdList(COMMANDLIST_TYPE::MAIN)->SetPipelineState(PIPELINE->GetPSO(it->first).Get());

		for (auto& Object : it->second)
		{

			if (g_engine->GetFrustum()->FrustumCulling(Object))
			{
				Object->Render(cmdList);
				//Object->Render(CMD_MANAGER->GetCmdList(COMMANDLIST_TYPE::MAIN));
				CurrentObjectCount++;
			}
		}
	}


}

void ObjectManager::ShadowRender(ComPtr<ID3D12GraphicsCommandList>& cmdList)
{
	// 기본 파이프라인 세팅
	cmdList->SetPipelineState(PIPELINE->GetPSO(PSO_TYPE::SHADOW).Get());
	
	for (auto it = m_objects.begin(); it != m_objects.end(); ++it)
	{
		for (auto& Object : it->second)
		{
			if (!Object->b_shadow)
				continue;
			if (g_engine->GetFrustum()->FrustumCulling(Object))
				Object->Render(cmdList);
		}
	}
}

void ObjectManager::Render1(ComPtr<ID3D12GraphicsCommandList>& cmdList)
{
	for (auto it = m_objectPool1.begin(); it != m_objectPool1.end(); ++it)
	{
		cmdList->SetPipelineState(PIPELINE->GetPSO(it->first).Get());
		//CMD_MANAGER->GetCmdList(COMMANDLIST_TYPE::MAIN)->SetPipelineState(PIPELINE->GetPSO(it->first).Get());

		for (auto& Object : it->second)
		{

			if (g_engine->GetFrustum()->FrustumCulling(Object))
			{
				Object->Render(cmdList);
				//Object->Render(CMD_MANAGER->GetCmdList(COMMANDLIST_TYPE::MAIN));
				CurrentObjectCount++;
			}
		}
	}

}

void ObjectManager::Render2(ComPtr<ID3D12GraphicsCommandList>& cmdList)
{
	for (auto it = m_objectPool2.begin(); it != m_objectPool2.end(); ++it)
	{
		cmdList->SetPipelineState(PIPELINE->GetPSO(it->first).Get());
		//CMD_MANAGER->GetCmdList(COMMANDLIST_TYPE::MAIN)->SetPipelineState(PIPELINE->GetPSO(it->first).Get());

		for (auto& Object : it->second)
		{

			if (g_engine->GetFrustum()->FrustumCulling(Object))
			{
				Object->Render(cmdList);
				//Object->Render(CMD_MANAGER->GetCmdList(COMMANDLIST_TYPE::MAIN));
				CurrentObjectCount++;
			}
		}
	}
}

void ObjectManager::ShadowRender1(ComPtr<ID3D12GraphicsCommandList>& cmdList)
{
	// 기본 파이프라인 세팅
	cmdList->SetPipelineState(PIPELINE->GetPSO(PSO_TYPE::SHADOW).Get());

	for (auto it = m_objectPool1.begin(); it != m_objectPool1.end(); ++it)
	{
		for (auto& Object : it->second)
		{
			if (!Object->b_shadow)
				continue;
			if (g_engine->GetFrustum()->FrustumCulling(Object))
				Object->Render(cmdList);
		}
	}
}

void ObjectManager::ShadowRender2(ComPtr<ID3D12GraphicsCommandList>& cmdList)
{
	// 기본 파이프라인 세팅
	cmdList->SetPipelineState(PIPELINE->GetPSO(PSO_TYPE::SHADOW).Get());

	for (auto it = m_objectPool2.begin(); it != m_objectPool2.end(); ++it)
	{
		for (auto& Object : it->second)
		{
			if (!Object->b_shadow)
				continue;
			if (g_engine->GetFrustum()->FrustumCulling(Object))
				Object->Render(cmdList);
		}
	}
}
