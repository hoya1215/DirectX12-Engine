#include "pch.h"
#include "Camera.h"
#include "Util.h"

void Camera::Update()
{
	Vector3 rotation = Vector3(m_dx, m_dy, 0.f);
	m_view = Util::CreateMatrix(m_position, rotation);
	m_proj = XMMatrixPerspectiveFovLH(m_fov, m_aspect, m_near, m_far);
}