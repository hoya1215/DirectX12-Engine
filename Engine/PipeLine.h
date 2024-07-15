#pragma once


#include "pch.h"
#include "MeshData.h"
#include "MeshBuffer.h"
#include "Texture.h"
#include "ConstantBuffer.h"

enum
{
	CBV_REGISTER_INDEX = 0,
	SRV_REGISTER_INDEX = 1,
	CBV_REGISTER_COUNT = 1,
	SRV_REGISTER_COUNT = 1,
	DESCRIPTOR_COUNT = CBV_REGISTER_COUNT + SRV_REGISTER_COUNT

};

class Object;
class Camera;

class PipeLine
{

public:
	PipeLine(ComPtr<ID3D12Device>& device, ComPtr<ID3D12GraphicsCommandList>& cmdList, ComPtr<ID3D12GraphicsCommandList>& resourceCmdList);

	// ����������
	void Init();
	void Update();
	void Render();

	// ���
	void Move();
	void Rotate();

	// ����
	void CreateShader(ComPtr<ID3DBlob>& blob, const wstring& filename, const D3D_SHADER_MACRO* defines,
		const string& entrypoint, const string& target);

	void CreateSampler();
	void CreateGlobalConstantData();

	ComPtr<ID3D12PipelineState> GetPSO() { return m_defaultPSO; }

private:
	ComPtr<ID3D12Device> m_device;
	ComPtr<ID3D12GraphicsCommandList> m_cmdList;
	ComPtr<ID3D12GraphicsCommandList> m_resCmdList;
	ComPtr<ID3D12RootSignature> m_rootSignature;
	D3D12_STATIC_SAMPLER_DESC m_defaultSampler;

	// Camera
	shared_ptr<Camera> m_mainCamera;

	// Mesh
	unique_ptr<MeshBuffer> m_meshBuffer;


	// shader
	ComPtr<ID3DBlob> m_vertexShader;
	ComPtr<ID3DBlob> m_pixelShader;


	// pipeline
	ComPtr<ID3D12PipelineState> m_defaultPSO;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC m_defaultPSODesc = {};

	// ��� ����
	ComPtr<ID3D12Resource> m_constantBuffer;
	ComPtr<ID3D12DescriptorHeap> m_cbvHeap;
	uint32 m_descriptorSize = 0;
	uint32 m_cbIndex = 0;

	// �۷ι� ��� ����
	ComPtr<ID3D12Resource> m_globalConstantBuffer;
	GlobalConstant m_globalConstantData;
	D3D12_GPU_VIRTUAL_ADDRESS m_globalCBAddress;

	// Texture
	unique_ptr<Texture> m_texture;

	// Material �׽�Ʈ
	shared_ptr<Object> m_test;
};

