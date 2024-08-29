#include "pch.h"
#include "Frustum.h"
#include "Object.h"
#include "BoundingShape.h"

void Frustum::Update(const Matrix invViewProj)
{

    vector<Vector3> worldPos = {

    XMVector3TransformCoord(Vector3(-1.f, 1.f, 0.f), invViewProj),
    XMVector3TransformCoord(Vector3(1.f, 1.f, 0.f), invViewProj),
    XMVector3TransformCoord(Vector3(1.f, -1.f, 0.f), invViewProj),
    XMVector3TransformCoord(Vector3(-1.f, -1.f, 0.f), invViewProj),
    XMVector3TransformCoord(Vector3(-1.f, 1.f, 1.f), invViewProj),
    XMVector3TransformCoord(Vector3(1.f, 1.f, 1.f), invViewProj),
    XMVector3TransformCoord(Vector3(1.f, -1.f, 1.f), invViewProj),
    XMVector3TransformCoord(Vector3(-1.f, -1.f, 1.f), invViewProj) };

    m_plane[PLANE_TYPE::FRONT] =
        XMPlaneFromPoints(worldPos[0], worldPos[1], worldPos[2]); // CW
    m_plane[PLANE_TYPE::BACK] =
        XMPlaneFromPoints(worldPos[4], worldPos[7], worldPos[5]); // CCW
    m_plane[UP] =
        XMPlaneFromPoints(worldPos[4], worldPos[5], worldPos[1]); // CW
    m_plane[DOWN] =
        XMPlaneFromPoints(worldPos[7], worldPos[3], worldPos[6]); // CCW
    m_plane[LEFT] =
        XMPlaneFromPoints(worldPos[4], worldPos[0], worldPos[7]); // CW
    m_plane[RIGHT] =
        XMPlaneFromPoints(worldPos[5], worldPos[6], worldPos[1]); // CCW

}

bool Frustum::FrustumCulling(shared_ptr<Object> obj)
{
    for (auto& i : m_plane)
    {
        Vector3 normal = Vector3(i.x, i.y, i.z);

        shared_ptr<BoundingShape> bs = dynamic_pointer_cast<BoundingShape>(obj->GetComponent(COMPONENT_TYPE::BOUNDING_SHAPE));
        if (bs == nullptr)
            return true;

        // 캐스팅 잘됐는지 확인
        //cout << bs->m_bSphere.Radius << endl;

        if (normal.Dot(obj->GetPosition()) + i.w > bs->m_bSphere.Radius)
            return false;
    }

    return true;
}
