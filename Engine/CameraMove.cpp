#include "pch.h"
#include "CameraMove.h"
#include "Camera.h"
#include "Engine.h"
#include "KeyInput.h"



void CameraMove::Update()
{
	if (INPUT->GetKeyState(Key::M) == "P")
	{
		b_mouseRotate = !b_mouseRotate;
	}

	Move();

	if(b_mouseRotate)
		Rotate();
}

void CameraMove::Move()
{
	Vector3 pos = GetObject()->GetPosition();

	if (INPUT->GetKeyState(Key::W) == "P" || INPUT->GetKeyState(Key::W) == "KP")
	{

		pos.y += 1.f * DT;
		GetObject()->SetPosition(pos);

	}

	if (INPUT->GetKeyState(Key::S) == "P" || INPUT->GetKeyState(Key::S) == "KP")
	{
		pos.y -= 1.f * DT;
		GetObject()->SetPosition(pos);
	}

	if (INPUT->GetKeyState(Key::D) == "P" || INPUT->GetKeyState(Key::D) == "KP")
	{
		pos.x += 1.f * DT;
		GetObject()->SetPosition(pos);
	}

	if (INPUT->GetKeyState(Key::A) == "P" || INPUT->GetKeyState(Key::A) == "KP")
	{
		pos.x -= 1.f * DT;
		GetObject()->SetPosition(pos);
	}
}

void CameraMove::Rotate()
{
	POINT mousePos;
	GetCursorPos(&mousePos);
	ScreenToClient(g_engine->GetHWND(), &mousePos);

	float mx = (2.f * mousePos.x / g_engine->m_width) - 1.f;
	float my = 1.f - (2.f * mousePos.y / g_engine->m_height);


	GetObject()->Setdx(my * m_sensivity);
	GetObject()->Setdy(-mx * m_sensivity);

}