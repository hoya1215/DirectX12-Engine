#pragma once
#include "MeshData.h"

class Mesh
{
public:
	void Init();
	static pair<vector<Vertex>, vector<uint16>> CreateBox(float scale = 1.f);
	static pair<vector<Vertex>, vector<uint16>> CreateRectangle(float scale = 1.f);
	
};

