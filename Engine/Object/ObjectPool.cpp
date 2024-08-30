#include "pch.h"
#include "ObjectPool.h"
#include "Object.h"

void ObjectPool::PushObject(PSO_TYPE psoType,   shared_ptr<Object>&& object)
{
	m_objectUsePool[psoType].push(move(object));
}

shared_ptr<Object> ObjectPool::GetUseObject(PSO_TYPE psoType)
{
	if (m_objectUsePool[psoType].empty())
	{
		SwapPool(psoType);
	}

	// 아직 반환된 것이 하나도 없다면
	if (m_objectUsePool[psoType].empty())
	{
		ReAllocate(psoType);
	}

	shared_ptr<Object> object = move(m_objectUsePool[psoType].front());
	m_objectUsePool[psoType].pop();
	return object;
}

void ObjectPool::ReturnObject(PSO_TYPE psoType, shared_ptr<Object>&& returnedObject)
{
	m_objectReturnPool[psoType].push(move(returnedObject));
}

void ObjectPool::ReturnObject(PSO_TYPE psoType, const shared_ptr<Object>& returnedObject)
{
	m_objectReturnPool[psoType].push(returnedObject);
}

void ObjectPool::ReAllocate(PSO_TYPE psoType)
{
	for (int i = 0; i < 5; ++i)
	{
		shared_ptr<Object> m_test = make_shared<Object>(MESH_TYPE::SPHERE);
		m_test->SetPipelineType(PSO_TYPE::DEFERRED);
		m_test->SetPosition(Vector3(-1.f + 2.f * i, 0.f, 5.f));
		m_test->GetMaterial()->b_dynamic = true;
		m_test->GetMaterial()->m_materialConstantData.baseColor = Vector4(1.0f, 0.f, 0.f, 1.f);

		m_objectUsePool[psoType].push(move(m_test));
	}

	
}

void ObjectPool::SwapPool(PSO_TYPE psoType)
{
	m_objectUsePool[psoType].swap(m_objectReturnPool[psoType]);
}
