#pragma once

enum class COMPONENT_TYPE
{
	MATERIAL,
	BEHAVIOUR
};

class Object;

template<typename T = Object>
class Component
{
public:
	void Init(const shared_ptr<T>& obj)
	{
		//m_object = make_shared<T>(); : 생성자 문제
		m_object = obj;
	}

	virtual void Update() {}

	shared_ptr<T> GetObject() { return m_object; }

private:
	shared_ptr<T> m_object;
};

