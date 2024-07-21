#pragma once

class Object;

class ObjectManager
{
public:
	void Init();
	void Update();
	void Render();

private:
	unordered_map<PSO_TYPE, vector<shared_ptr<Object>>> m_objects;
};

