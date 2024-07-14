#pragma once


class Camera
{
public:
	void Update();

private:

	// ��ġ
	Vector3 m_position{ 0.f, 0.f, 0.f };
	Vector3 m_right{ 1.f, 0.f, 0.f };
	Vector3 m_forward{ 0.f, 0.f, 1.f };

	// ȸ��
	float m_dx;
	float m_dy;

	// ��Ÿ
	float m_near = 0.f;
	float m_far = 1.f;
	float m_aspect = XM_PI / 4;
	float m_fov;

	Matrix m_view;
	Matrix m_proj;
	

};

