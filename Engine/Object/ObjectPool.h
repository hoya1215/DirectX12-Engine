#pragma once


class Object;

class ObjectPool
{
public:
	void PushObject(PSO_TYPE psoType,  shared_ptr<Object>&& object);

	shared_ptr<Object> GetUseObject(PSO_TYPE psoType);

	void ReturnObject(PSO_TYPE psoType, shared_ptr<Object>&& returnedObject);
	void ReturnObject(PSO_TYPE psoType, const shared_ptr<Object>& returnedObject);

	void ReAllocate(PSO_TYPE psoType);

	void SwapPool(PSO_TYPE psoType);

private:
	unordered_map<PSO_TYPE, queue<shared_ptr<Object>>> m_objectUsePool;
	unordered_map<PSO_TYPE, queue<shared_ptr<Object>>> m_objectReturnPool;
};

