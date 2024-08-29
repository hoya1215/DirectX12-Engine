#pragma once
#include "Headers.h"

enum PLANE_TYPE
{
	FRONT,
	BACK,
	UP,
	DOWN,
	LEFT,
	RIGHT,

	END

};

class Frustum {

public:

	void Update(const Matrix invViewProj);

	bool FrustumCulling(shared_ptr<class Object> obj);

protected:

	array<Vector4, END> m_plane;

};

