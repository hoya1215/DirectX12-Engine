#pragma once
#include "Component.h"
#include "Object.h"


class BoundingShape : public Component<Object>
{
public:
	BoundingShape(Vector3 position, Vector3 extent, float radius);

	void CreateBoundingBox(Vector3 position, Vector3 extent);

	void CreateBoundingSphere(Vector3 position, float radius);



	virtual void Update() override;

public:
	BoundingBox m_bBox;
	BoundingSphere m_bSphere;

};

