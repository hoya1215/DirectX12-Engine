#pragma once
#include "MeshData.h"

class Mesh
{
public:
	static pair<vector<Vertex>, vector<uint16>> CreateBox(float scale = 1.f);
	static pair<vector<Vertex>, vector<uint16>> CreateRectangle(float scale = 1.f);
	static pair<vector<Vertex>, vector<uint16>> CreateSphere(float scale = 1.f);
	static pair<vector<Vertex>, vector<uint16>> CreateGrid(int row = 10, int column = 10, float scale = 1.f);

private:
	static VI_pair m_box;
	static VI_pair m_rectangle;
	static VI_pair m_sphere;
	static VI_pair m_grid;
	
};

