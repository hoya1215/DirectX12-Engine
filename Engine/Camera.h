#pragma once


class Camera
{
public:
	Camera();

	void Update();

	void SetPosition(Vector3 pos) { m_position = pos; }
	void Setdx(float dx) { m_dx = dx; }

	float Getdx() { return m_dx; }
	Vector3 GetPosition() { return m_position; }
public:
	Matrix m_view;
	Matrix m_proj;

private:

	// 위치
	Vector3 m_position{ 0.f, 0.f, 0.f };
	Vector3 m_right{ 1.f, 0.f, 0.f };
	Vector3 m_forward{ 0.f, 0.f, 1.f };

	// 회전
	float m_dx = 0.f;
	float m_dy = 0.f;

	// 기타
	float m_near = 1.f;
	float m_far = 400.f;
	float m_aspect = 1280.f / 720.f;
	float m_fov = XM_PI / 2.f;


	

};

