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

void Grid::Render(ComPtr<ID3D12GraphicsCommandList>& cmdList)
{
	cmdList->IASetVertexBuffers(0, 1, &GetMaterial()->GetMeshBuffer()->GetVertexBufferView());
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
	cmdList->SetGraphicsRootDescriptorTable(2, GetMaterial()->GetCBVHandle());

	if (GetMaterial()->m_baseTexture != nullptr)
		cmdList->SetGraphicsRootDescriptorTable(3, GetMaterial()->GetSRVHandle());

	cmdList->DrawInstanced(GetMaterial()->GetMeshBuffer()->GetVertexCount(), 1, 0, 0);
}
