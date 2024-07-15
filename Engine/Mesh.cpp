#include "pch.h"
#include "Mesh.h"

pair<vector<Vertex>, vector<uint16>> Mesh::CreateBox(float scale)
{
	float defaultScale = 1.f * scale;

	vector<Vertex> vertices =
	{ 
		{ Vector3(-defaultScale, -defaultScale, -defaultScale), Vector2(1.f, 0.f),Vector4(1.0f, 1.0f, 1.0f, 1.0f) },
		{ Vector3(-defaultScale, defaultScale, -defaultScale), Vector2(0.f, 0.f),Vector4(1.0f, 1.0f, 1.0f, 1.0f) },
		{ Vector3(defaultScale, defaultScale, -defaultScale), Vector2(0.f, 1.f),Vector4(1.0f, 1.0f, 1.0f, 1.0f) },
		{ Vector3(defaultScale, -defaultScale, -defaultScale), Vector2(1.f, 1.f),Vector4(1.0f, 1.0f, 1.0f, 1.0f) },
		{ Vector3(-defaultScale, -defaultScale, defaultScale), Vector2(1.f, 0.f),Vector4(1.0f, 1.0f, 1.0f, 1.0f) },
		{ Vector3(-defaultScale, defaultScale, defaultScale), Vector2(0.f, 0.f),Vector4(1.0f, 1.0f, 1.0f, 1.0f) },
		{ Vector3(defaultScale, defaultScale, defaultScale), Vector2(0.f, 1.f),Vector4(1.0f, 1.0f, 1.0f, 1.0f) },
		{ Vector3(defaultScale, -defaultScale, defaultScale), Vector2(1.f, 1.f),Vector4(1.0f, 1.0f, 1.0f, 1.0f) }
	
	};





	vector<uint16> indices =
	{
		0, 1, 2,
		0, 2, 3,

		4, 6, 5,
		4, 7, 6,

		4, 5, 1,
		4, 1, 0,

		3, 2, 6,
		3, 6, 7,

		1, 5, 6,
		1, 6, 2,

		4, 0, 3,
		4, 3, 7
	};

	return { vertices, indices };
}

pair<vector<Vertex>, vector<uint16>> Mesh::CreateRectangle(float scale)
{
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

	return { vec, indices };
}
