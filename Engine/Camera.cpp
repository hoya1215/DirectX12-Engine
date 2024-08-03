#include "pch.h"
#include "Camera.h"
#include "Util.h"
#include "CameraMove.h"

Camera::Camera(string name)
{
	m_name = name;
	Vector3 rotation = Vector3(m_dx, m_dy, 0.f);
	m_view = Matrix::CreateTranslation(-m_position) * Matrix::CreateRotationY(-m_dy)
		* Matrix::CreateRotationX(m_dx);
	//m_view = m_view.Transpose();
	//m_view = Util::CreateMatrix(m_position, Vector3(1.f, 1.f, 1.f), rotation);
	m_proj = XMMatrixPerspectiveFovLH(m_fov, m_aspect, m_near, m_far);
	//m_proj = m_proj.Transpose();

	//if (m_name == "MainCamera")
	//{
	//	shared_ptr<CameraMove> cameraMove = make_shared<CameraMove>();
	//	this->AddComponent(COMPONENT_TYPE::BEHAVIOUR, cameraMove);
	//}
}

void Camera::Update()
{


	for (auto it = m_component.begin(); it != m_component.end(); ++it)
	{
		it->second->Update();
	}

	Vector3 rotation = Vector3(m_dx, m_dy, 0.f);
	m_view = Matrix::CreateTranslation(-m_position) * Matrix::CreateRotationY(-m_dy)
		* Matrix::CreateRotationX(m_dx);
	//m_view = m_view.Transpose();
	//m_view = Util::CreateMatrix(m_position, Vector3(1.f, 1.f, 1.f), rotation);
	m_proj = XMMatrixPerspectiveFovLH(m_fov, m_aspect, m_near, m_far);
	//m_proj = m_proj.Transpose();

}

void Camera::AddComponent(COMPONENT_TYPE componentType, shared_ptr<Component<Camera>> component)
{
	auto it = m_component.find(componentType);
	if (it != m_component.end())
		return;

	m_component.insert({ componentType, component });
	component->Init(shared_from_this());
}