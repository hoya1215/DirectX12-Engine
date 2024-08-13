#pragma once

#include "Component.h"

class Camera : public enable_shared_from_this<Camera>
{
public:
	Camera(string name);

	void Update();
	void UpdateDirVector();

	void SetPosition(Vector3 pos) { m_position = pos; }
	void Setdx(float dx) { m_dx = dx; }
	void Setdy(float dy) { m_dy = dy; }

	float Getdx() { return m_dx; }
	float Getdy() { return m_dy; }
	Vector3 GetPosition() { return m_position; }
	Vector3 GetForward() { return m_forward; }
	Vector3 GetRight() { return m_right; }


	void AddComponent(COMPONENT_TYPE componentType, shared_ptr<Component<Camera>> component);
public:
	Matrix m_view;
	Matrix m_proj;

private:
	string m_name;

	// 위치
	Vector3 m_position{ 0.f, 0.f, 0.f };
	Vector3 m_right{ 1.f, 0.f, 0.f };
	Vector3 m_forward{ 0.f, 0.f, 1.f };
	Vector3 m_up{ 0.f, 1.f, 0.f };

	// 회전
	float m_dx = 0.f;
	float m_dy = 0.f;

	// 기타
	float m_near = 1.f;
	float m_far = 400.f;
	float m_aspect = 1280.f / 720.f;
	float m_fov = XM_PI / 2.f;


	unordered_map<COMPONENT_TYPE, shared_ptr<Component<Camera>>> m_component;

};

