#include "pch.h"
#include "Grid.h"
#include "Engine.h"
#include "Material.h"
#include "MeshBuffer.h"

Grid::Grid(MESH_TYPE meshType, const wstring& path,
	Vector3 position, const string& name, PSO_TYPE psoType, bool color,
	float scale, Vector3 rotation, int row, int column) : Object(meshType, path, position,
		name, psoType, color, scale, rotation, row, column)
{

}

void Grid::Render()
{
	CMD_LIST->IASetVertexBuffers(0, 1, &GetMaterial()->GetMeshBuffer()->GetVertexBufferView());
	//CMD_LIST->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
	CMD_LIST->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
	CMD_LIST->SetGraphicsRootDescriptorTable(2, GetMaterial()->GetCBVHandle());

	if (GetMaterial()->m_texture != nullptr)
		CMD_LIST->SetGraphicsRootDescriptorTable(3, GetMaterial()->GetSRVHandle());

	CMD_LIST->DrawInstanced(GetMaterial()->GetMeshBuffer()->GetVertexCount(), 1, 0, 0);

}
