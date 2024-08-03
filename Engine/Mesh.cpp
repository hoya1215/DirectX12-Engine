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

	vector<Vertex> vertices(24);

	// À­¸é
	vertices[0].pos = Vector3(-ds, ds, -ds);
	vertices[0].texcoord = Vector2(0.f, 0.f);
	vertices[0].normal = vertices[0].pos;
	vertices[0].normal.Normalize();

	vertices[1].pos = Vector3(-ds, ds, ds);
	vertices[1].texcoord = Vector2(1.f, 0.f);
	vertices[1].normal = vertices[1].pos;
	vertices[1].normal.Normalize();

	vertices[2].pos = Vector3(ds, ds, ds);
	vertices[2].texcoord = Vector2(1.f, 1.f);
	vertices[2].normal = vertices[2].pos;
	vertices[2].normal.Normalize();

	vertices[3].pos = Vector3(ds, ds, -ds);
	vertices[3].texcoord = Vector2(0.f, 1.f);
	vertices[3].normal = vertices[3].pos;
	vertices[3].normal.Normalize();

	// ¾Æ·§¸é
	vertices[4].pos = Vector3(-ds, -ds, -ds);
	vertices[4].texcoord = Vector2(0.f, 0.f);
	vertices[4].normal = vertices[4].pos;
	vertices[4].normal.Normalize();

	vertices[5].pos = Vector3(-ds, ds, -ds);
	vertices[5].texcoord = Vector2(1.f, 0.f);
	vertices[5].normal = vertices[5].pos;
	vertices[5].normal.Normalize();

	vertices[6].pos = Vector3(ds, -ds, ds);
	vertices[6].texcoord = Vector2(1.f, 1.f);
	vertices[6].normal = vertices[6].pos;
	vertices[6].normal.Normalize();

	vertices[7].pos = Vector3(-ds, -ds, ds);
	vertices[7].texcoord = Vector2(0.f, 1.f);
	vertices[7].normal = vertices[7].pos;
	vertices[7].normal.Normalize();


	// ¾Õ¸é
	vertices[8].pos = Vector3(-ds, -ds, -ds);
	vertices[8].texcoord = Vector2(0.f, 0.f);
	vertices[8].normal = vertices[8].pos;
	vertices[8].normal.Normalize();

	vertices[9].pos = Vector3(-ds, ds, -ds);
	vertices[9].texcoord = Vector2(1.f, 0.f);
	vertices[9].normal = vertices[9].pos;
	vertices[9].normal.Normalize();

	vertices[10].pos = Vector3(ds, ds, -ds);
	vertices[10].texcoord = Vector2(1.f, 1.f);
	vertices[10].normal = vertices[10].pos;
	vertices[10].normal.Normalize();

	vertices[11].pos = Vector3(ds, -ds, -ds);
	vertices[11].texcoord = Vector2(0.f, 1.f);
	vertices[11].normal = vertices[11].pos;
	vertices[11].normal.Normalize();

	// µÞ¸é
	vertices[12].pos = Vector3(-ds, -ds, ds);
	vertices[12].texcoord = Vector2(0.f, 0.f);
	vertices[12].normal = vertices[12].pos;
	vertices[12].normal.Normalize();

	vertices[13].pos = Vector3(ds, -ds, ds);
	vertices[13].texcoord = Vector2(1.f, 0.f);
	vertices[13].normal = vertices[13].pos;
	vertices[13].normal.Normalize();

	vertices[14].pos = Vector3(-ds, ds, -ds);
	vertices[14].texcoord = Vector2(1.f, 0.f);
	vertices[14].normal = vertices[14].pos;
	vertices[14].normal.Normalize();

	vertices[15].pos = Vector3(-ds, ds, ds);
	vertices[15].texcoord = Vector2(0.f, 1.f);
	vertices[15].normal = vertices[15].pos;
	vertices[15].normal.Normalize();

	// ¿ÞÂÊ
	vertices[16].pos = Vector3(-ds, -ds, ds);
	vertices[16].texcoord = Vector2(0.f, 0.f);
	vertices[16].normal = vertices[16].pos;
	vertices[16].normal.Normalize();

	vertices[17].pos = Vector3(-ds, ds, ds);
	vertices[17].texcoord = Vector2(1.f, 0.f);
	vertices[17].normal = vertices[17].pos;
	vertices[17].normal.Normalize();

	vertices[18].pos = Vector3(-ds, ds, -ds);
	vertices[18].texcoord = Vector2(1.f, 1.f);
	vertices[18].normal = vertices[18].pos;
	vertices[18].normal.Normalize();

	vertices[19].pos = Vector3(-ds, -ds, -ds);
	vertices[19].texcoord = Vector2(0.f, 1.f);
	vertices[19].normal = vertices[19].pos;
	vertices[19].normal.Normalize();

	// ¿À¸¥ÂÊ
	vertices[20].pos = Vector3(ds, -ds, ds);
	vertices[20].texcoord = Vector2(0.f, 0.f);
	vertices[20].normal = vertices[20].pos;
	vertices[20].normal.Normalize();

	vertices[21].pos = Vector3(ds, -ds, -ds);
	vertices[21].texcoord = Vector2(1.f, 0.f);
	vertices[21].normal = vertices[21].pos;
	vertices[21].normal.Normalize();

	vertices[22].pos = Vector3(ds, ds, -ds);
	vertices[22].texcoord = Vector2(1.f, 1.f);
	vertices[22].normal = vertices[22].pos;
	vertices[22].normal.Normalize();

	vertices[23].pos = Vector3(ds, ds, ds);
	vertices[23].texcoord = Vector2(1.f, 1.f);
	vertices[23].normal = vertices[23].pos;
	vertices[23].normal.Normalize();

    //vector<Vertex> vertices =
    //{
    //    // À­¸é
    //    {Vector3(-ds, ds, -ds), Vector2(0.f, 0.f), Vector4(1.0f, 1.0f, 1.0f, 1.0f)},
    //    {Vector3(-ds, ds, ds), Vector2(1.0f, 0.f), Vector4(1.0f, 1.0f, 1.0f, 1.0f)},
    //    {Vector3(ds, ds , ds), Vector2(1.f, 1.f), Vector4(1.f, 1.f, 1.f, 1.f)},
    //    {Vector3(ds, ds, -ds), Vector2(0.f, 1.f), Vector4(1.0f, 1.0f, 1.0f, 1.0f)},

    //    // ¾Æ·§¸é
    //    {Vector3(-ds, -ds, -ds), Vector2(0.f, 0.f), Vector4(1.0f, 1.0f, 1.0f, 1.0f)},
    //    {Vector3(ds, -ds, -ds), Vector2(1.f, 0.f), Vector4(1.0f, 1.0f, 1.0f, 1.0f)},
    //    {Vector3(ds, -ds, ds), Vector2(1.f, 1.f), Vector4(1.0f, 1.0f, 1.0f, 1.0f)},
    //    {Vector3(-ds, -ds, ds), Vector2(0.f, 1.f), Vector4(1.0f, 1.0f, 1.0f, 1.0f)},

    //    // ¾Õ¸é
    //    {Vector3(-ds, -ds, -ds), Vector2(0.f, 0.f), Vector4(1.0f, 1.0f, 1.0f, 1.0f)},
    //    {Vector3(-ds, ds, -ds), Vector2(1.f, 0.f), Vector4(1.0f, 1.0f, 1.0f, 1.0f)},
    //    {Vector3(ds, ds, -ds), Vector2(1.f, 1.f), Vector4(1.0f, 1.0f, 1.0f, 1.0f)},
    //    {Vector3(ds, -ds, -ds), Vector2(0.f, 1.f), Vector4(1.0f, 1.0f, 1.0f, 1.0f)},

    //    // µÞ¸é
    //    {Vector3(-ds, -ds, ds), Vector2(0.f, 0.f), Vector4(1.0f, 1.0f, 1.0f, 1.0f)},
    //    {Vector3(ds, -ds, ds), Vector2(1.f , 0.f), Vector4(1.0f, 1.0f, 1.0f, 1.0f)},
    //    {Vector3(ds, ds, ds), Vector2(1.f, 1.f), Vector4(1.0f, 1.0f, 1.0f, 1.0f)},
    //    {Vector3(-ds, ds, ds), Vector2(0.f, 1.f), Vector4(1.0f, 1.0f, 1.0f, 1.0f)},

    //    // ¿ÞÂÊ
    //    {Vector3(-ds, -ds, ds), Vector2(0.f, 0.f), Vector4(1.0f, 1.0f, 1.0f, 1.0f)},
    //    {Vector3(-ds, ds, ds), Vector2(1.f, 0.f), Vector4(1.0f, 1.0f, 1.0f, 1.0f)},
    //    {Vector3(-ds, ds, -ds), Vector2(1.f, 1.f), Vector4(1.0f, 1.0f, 1.0f, 1.0f)},
    //    {Vector3(-ds, -ds, -ds), Vector2(0.f, 1.f), Vector4(1.0f, 1.0f, 1.0f, 1.0f)},

    //    //¿À¸¥ÂÊ
    //    {Vector3(ds, -ds, ds), Vector2(0.f, 0.f), Vector4(1.0f, 1.0f, 1.0f, 1.0f)},
    //    {Vector3(ds, -ds, -ds), Vector2(1.f, 0.f), Vector4(1.0f, 1.0f, 1.0f, 1.0f)},
    //    {Vector3(ds, ds, -ds), Vector2(1.f, 1.f), Vector4(1.0f, 1.0f, 1.0f, 1.0f)},
    //    {Vector3(ds, ds, ds), Vector2(1.f, 1.f), Vector4(1.0f, 1.0f, 1.0f, 1.0f)}
    //};





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
	float defaultScale = 2.f * scale;
	vec[0].pos = Vector3(-defaultScale, -defaultScale, 0.0f);
	vec[0].texcoord = Vector2(0.0f, 1.0f);
	vec[0].normal = vec[0].pos;
	vec[0].normal.Normalize();

	vec[1].pos = Vector3(-defaultScale, defaultScale, 0.0f);
	vec[1].texcoord = Vector2(0.0f, 0.0f);
	vec[1].normal = vec[1].pos;
	vec[1].normal.Normalize();


	vec[2].pos = Vector3(defaultScale, defaultScale, 0.0f);
	vec[2].texcoord = Vector2(1.0f, 0.0f);
	vec[2].normal = vec[2].pos;
	vec[2].normal.Normalize();


	vec[3].pos = Vector3(defaultScale, -defaultScale, 0.0f);
	vec[3].texcoord = Vector2(1.0f, 1.0f);
	vec[3].normal = vec[3].pos;
	vec[3].normal.Normalize();


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
	v.normal = v.pos;
	v.normal.Normalize();

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

			v.normal = v.pos;
			v.normal.Normalize();

			v.texcoord = Vector2(deltaU * x, deltaV * y);


			vertices.push_back(v);
		}
	}

	// ³²±Ø
	v.pos = Vector3(0.0f, -radius, 0.0f);
	v.texcoord = Vector2(0.5f, 1.0f);
	v.normal = v.pos;
	v.normal.Normalize();

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
