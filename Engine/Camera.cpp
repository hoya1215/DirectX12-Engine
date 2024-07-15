#include "pch.h"
#include "Camera.h"
#include "Util.h"

Camera::Camera()
{
	Vector3 rotation = Vector3(m_dx, m_dy, 0.f);
	m_view = Util::CreateMatrix(-m_position, Vector3(1.f, 1.f, 1.f), rotation);
	//m_view = Matrix::CreateTranslation(m_position) * Matrix::CreateRotationX(m_dx)
	//	* Matrix::CreateRotationY(m_dy);
	m_proj = XMMatrixPerspectiveFovLH(m_fov, m_aspect, m_near, m_far);
	m_proj = m_proj.Transpose();
}

void Camera::Update()
{
	Vector3 rotation = Vector3(m_dx, m_dy, 0.f);
	m_view = Util::CreateMatrix(-m_position, Vector3(1.f, 1.f, 1.f), rotation);
	//m_view = Matrix::CreateTranslation(m_position) * Matrix::CreateRotationX(m_dx)
	//	* Matrix::CreateRotationY(m_dy);
	m_proj = XMMatrixPerspectiveFovLH(m_fov, m_aspect, m_near, m_far);
	m_proj = m_proj.Transpose();
}