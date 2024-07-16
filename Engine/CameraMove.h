#pragma once

#include "Behaviour.h"

class Camera;

class CameraMove : public Behaviour<Camera>
{
public:

	virtual void Update() override;

	void Move();
	void Rotate();

private:
	bool b_mouseRotate = false;
	float m_sensivity = 1.f;


};

