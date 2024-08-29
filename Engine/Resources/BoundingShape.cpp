#include "pch.h"
#include "BoundingShape.h"

BoundingShape::BoundingShape(Vector3 position, Vector3 extent, float radius)
{
	CreateBoundingBox(position, extent);
	CreateBoundingSphere(position, radius);
}

void BoundingShape::CreateBoundingBox(Vector3 position, Vector3 extent)
{
	m_bBox.Center = position;
	m_bBox.Extents = extent;
}

void BoundingShape::CreateBoundingSphere(Vector3 position, float radius)
{
	m_bSphere.Center = position;
	m_bSphere.Radius = radius;
}

void BoundingShape::Update()
{
	Vector3 pos = GetObject()->GetPosition();
	m_bBox.Center = pos;
	m_bSphere.Center = pos;

	// 나중에 물체의 크기가 동적으로 변할때 크기도 수정
}
