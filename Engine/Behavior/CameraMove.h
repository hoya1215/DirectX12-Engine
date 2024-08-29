#pragma once

#include "Behavior.h"

class Camera;

class CameraMove : public Behavior<Camera>
{
public:

	virtual void Update() override;

	void Move();
	void Rotate();

private:
	bool b_mouseRotate = false;
	float m_sensivity = 1.f;


};

