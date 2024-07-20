#include "pch.h"
#include "Mesh.h"

VI_pair Mesh::m_box;
VI_pair Mesh::m_rectangle;
VI_pair Mesh::m_sphere;

pair<vector<Vertex>, vector<uint16>> Mesh::CreateBox(float scale)
{
	if (!m_box.first.empty())
		return m_box;

	float ds = 1.f * scale;

    vector<Vertex> vertices =
    {
        // À­¸é
        {Vector3(-ds, ds, -ds), Vector2(0.f, 0.f), Vector4(1.0f, 1.0f, 1.0f, 1.0f)},
        {Vector3(-ds, ds, ds), Vector2(1.0f, 0.f), Vector4(1.0f, 1.0f, 1.0f, 1.0f)},
        {Vector3(ds, ds , ds), Vector2(1.f, 1.f), Vector4(1.f, 1.f, 1.f, 1.f)},
        {Vector3(ds, ds, -ds), Vector2(0.f, 1.f), Vector4(1.0f, 1.0f, 1.0f, 1.0f)},

        // ¾Æ·§¸é
        {Vector3(-ds, -ds, -ds), Vector2(0.f, 0.f), Vector4(1.0f, 1.0f, 1.0f, 1.0f)},
        {Vector3(ds, -ds, -ds), Vector2(1.f, 0.f), Vector4(1.0f, 1.0f, 1.0f, 1.0f)},
        {Vector3(ds, -ds, ds), Vector2(1.f, 1.f), Vector4(1.0f, 1.0f, 1.0f, 1.0f)},
        {Vector3(-ds, -ds, ds), Vector2(0.f, 1.f), Vector4(1.0f, 1.0f, 1.0f, 1.0f)},

        // ¾Õ¸é
        {Vector3(-ds, -ds, -ds), Vector2(0.f, 0.f), Vector4(1.0f, 1.0f, 1.0f, 1.0f)},
        {Vector3(-ds, ds, -ds), Vector2(1.f, 0.f), Vector4(1.0f, 1.0f, 1.0f, 1.0f)},
        {Vector3(ds, ds, -ds), Vector2(1.f, 1.f), Vector4(1.0f, 1.0f, 1.0f, 1.0f)},
        {Vector3(ds, -ds, -ds), Vector2(0.f, 1.f), Vector4(1.0f, 1.0f, 1.0f, 1.0f)},

        // µÞ¸é
        {Vector3(-ds, -ds, ds), Vector2(0.f, 0.f), Vector4(1.0f, 1.0f, 1.0f, 1.0f)},
        {Vector3(ds, -ds, ds), Vector2(1.f , 0.f), Vector4(1.0f, 1.0f, 1.0f, 1.0f)},
        {Vector3(ds, ds, ds), Vector2(1.f, 1.f), Vector4(1.0f, 1.0f, 1.0f, 1.0f)},
        {Vector3(-ds, ds, ds), Vector2(0.f, 1.f), Vector4(1.0f, 1.0f, 1.0f, 1.0f)},

        // ¿ÞÂÊ
        {Vector3(-ds, -ds, ds), Vector2(0.f, 0.f), Vector4(1.0f, 1.0f, 1.0f, 1.0f)},
        {Vector3(-ds, ds, ds), Vector2(1.f, 0.f), Vector4(1.0f, 1.0f, 1.0f, 1.0f)},
        {Vector3(-ds, ds, -ds), Vector2(1.f, 1.f), Vector4(1.0f, 1.0f, 1.0f, 1.0f)},
        {Vector3(-ds, -ds, -ds), Vector2(0.f, 1.f), Vector4(1.0f, 1.0f, 1.0f, 1.0f)},

        //¿À¸¥ÂÊ
        {Vector3(ds, -ds, ds), Vector2(0.f, 0.f), Vector4(1.0f, 1.0f, 1.0f, 1.0f)},
        {Vector3(ds, -ds, -ds), Vector2(1.f, 0.f), Vector4(1.0f, 1.0f, 1.0f, 1.0f)},
        {Vector3(ds, ds, -ds), Vector2(1.f, 1.f), Vector4(1.0f, 1.0f, 1.0f, 1.0f)},
        {Vector3(ds, ds, ds), Vector2(1.f, 1.f), Vector4(1.0f, 1.0f, 1.0f, 1.0f)}
    };





	vector<uint16> indices =
	{
        0,  1,  2,  0,  2,  3,  // À­¸é
        4,  5,  6,  4,  6,  7,  // ¾Æ·§¸é
        8,  9,  10, 8,  10, 11, // ¾Õ¸é
        12, 13, 14, 12, 14, 15, // µÞ¸é
        16, 17, 18, 16, 18, 19, // ¿ÞÂÊ
        20, 21, 22, 20, 22, 23  // ¿À¸¥ÂÊ
	};
	
	m_box = { vertices, indices };

	return m_box;
}

pair<vector<Vertex>, vector<uint16>> Mesh::CreateRectangle(float scale)
{
	if (!m_rectangle.first.empty())
		return m_rectangle;

	vector<Vertex> vec(4);
	float defaultScale = 1.f * scale;
	vec[0].pos = Vector3(-defaultScale, -defaultScale, 0.0f);
	vec[0].texcoord = Vector2(0.0f, 1.0f);
	vec[0].color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

	vec[1].pos = Vector3(-defaultScale, defaultScale, 0.0f);
	vec[1].texcoord = Vector2(0.0f, 0.0f);
	vec[1].color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

	vec[2].pos = Vector3(defaultScale, defaultScale, 0.0f);
	vec[2].texcoord = Vector2(1.0f, 0.0f);
	vec[2].color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

	vec[3].pos = Vector3(defaultScale, -defaultScale, 0.0f);
	vec[3].texcoord = Vector2(1.0f, 1.0f);
	vec[3].color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

	vector<uint16> indices =
	{
		0, 1, 2,
		0, 2, 3
	};

	m_rectangle = { vec, indices };
	return m_rectangle;
}

pair<vector<Vertex>, vector<uint16>> Mesh::CreateSphere(float scale)
{
	if (!m_sphere.first.empty())
		return m_sphere;

	float radius = 0.5f; // ±¸ÀÇ ¹ÝÁö¸§
	uint32 stackCount = 20; // °¡·Î ºÐÇÒ
	uint32 sliceCount = 20; // ¼¼·Î ºÐÇÒ

	vector<Vertex> vertices;

	Vertex v;

	// ºÏ±Ø
	v.pos = Vector3(0.0f, radius, 0.0f);
	v.texcoord = Vector2(0.5f, 0.0f);

	vertices.push_back(v);

	float stackAngle = XM_PI / stackCount;
	float sliceAngle = XM_2PI / sliceCount;

	float deltaU = 1.f / static_cast<float>(sliceCount);
	float deltaV = 1.f / static_cast<float>(stackCount);

	// °í¸®¸¶´Ù µ¹¸é¼­ Á¤Á¡À» °è»êÇÑ´Ù (ºÏ±Ø/³²±Ø ´ÜÀÏÁ¡Àº °í¸®°¡ X)
	for (uint32 y = 1; y <= stackCount - 1; ++y)
	{
		float phi = y * stackAngle;

		// °í¸®¿¡ À§Ä¡ÇÑ Á¤Á¡
		for (uint32 x = 0; x <= sliceCount; ++x)
		{
			float theta = x * sliceAngle;

			v.pos.x = radius * sinf(phi) * cosf(theta);
			v.pos.y = radius * cosf(phi);
			v.pos.z = radius * sinf(phi) * sinf(theta);

			v.texcoord = Vector2(deltaU * x, deltaV * y);


			vertices.push_back(v);
		}
	}

	// ³²±Ø
	v.pos = Vector3(0.0f, -radius, 0.0f);
	v.texcoord = Vector2(0.5f, 1.0f);

	vertices.push_back(v);

	vector<uint16> idx(36);

	// ºÏ±Ø ÀÎµ¦½º
	for (uint32 i = 0; i <= sliceCount; ++i)
	{
		//  [0]
		//   |  \
		//  [i+1]-[i+2]
		idx.push_back(0);
		idx.push_back(i + 2);
		idx.push_back(i + 1);
	}

	// ¸öÅë ÀÎµ¦½º
	uint32 ringVertexCount = sliceCount + 1;
	for (uint32 y = 0; y < stackCount - 2; ++y)
	{
		for (uint32 x = 0; x < sliceCount; ++x)
		{
			//  [y, x]-[y, x+1]
			//  |		/
			//  [y+1, x]
			idx.push_back(1 + (y)*ringVertexCount + (x));
			idx.push_back(1 + (y)*ringVertexCount + (x + 1));
			idx.push_back(1 + (y + 1) * ringVertexCount + (x));
			//		 [y, x+1]
			//		 /	  |
			//  [y+1, x]-[y+1, x+1]
			idx.push_back(1 + (y + 1) * ringVertexCount + (x));
			idx.push_back(1 + (y)*ringVertexCount + (x + 1));
			idx.push_back(1 + (y + 1) * ringVertexCount + (x + 1));
		}
	}

	// ³²±Ø ÀÎµ¦½º
	uint32 bottomIndex = static_cast<uint32>(vertices.size()) - 1;
	uint32 lastRingStartIndex = bottomIndex - ringVertexCount;
	for (uint32 i = 0; i < sliceCount; ++i)
	{
		//  [last+i]-[last+i+1]
		//  |      /
		//  [bottom]
		idx.push_back(bottomIndex);
		idx.push_back(lastRingStartIndex + i);
		idx.push_back(lastRingStartIndex + i + 1);
	}

	m_sphere = { vertices, idx };
	return m_sphere;
}
