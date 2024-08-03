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

	// 파이프라인
	void Init();
	void WorkSpace();
	void Render();


	// 생성
	void CreateShader(ComPtr<ID3DBlob>& blob, const wstring& filename, const D3D_SHADER_MACRO* defines,
		const string& entrypoint, const string& target);

	void CreateSampler();
	void CreateGlobalConstantData();

	ComPtr<ID3D12PipelineState> GetPSO(PSO_TYPE psoType) { return m_pso[psoType]; }

private:
	ComPtr<ID3D12Device> m_device;
	ComPtr<ID3D12GraphicsCommandList> m_cmdList;
	ComPtr<ID3D12GraphicsCommandList> m_resCmdList;
	ComPtr<ID3D12RootSignature> m_rootSignature;
	D3D12_STATIC_SAMPLER_DESC m_defaultSampler;

	// Mesh
	unique_ptr<MeshBuffer> m_meshBuffer;


	// shader
	ComPtr<ID3DBlob> m_vertexShader;
	ComPtr<ID3DBlob> m_pixelShader;
	ComPtr<ID3DBlob> m_skyboxVS;
	ComPtr<ID3DBlob> m_skyboxPS;
	ComPtr<ID3DBlob> m_deferredPS;
	ComPtr<ID3DBlob> m_postProcessVS;
	ComPtr<ID3DBlob> m_postProcessPS;


	// pipeline
	ComPtr<ID3D12PipelineState> m_defaultPSO;
	ComPtr<ID3D12PipelineState> m_skyboxPSO;
	ComPtr<ID3D12PipelineState> m_deferredPSO;
	ComPtr<ID3D12PipelineState> m_postProcessPSO;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC m_defaultPSODesc = {};
	D3D12_GRAPHICS_PIPELINE_STATE_DESC m_skyboxPSODesc = {};
	D3D12_GRAPHICS_PIPELINE_STATE_DESC m_deferredPSODesc = {};
	D3D12_GRAPHICS_PIPELINE_STATE_DESC m_postProcessPSODesc = {};



	// PSO Map
	unordered_map<PSO_TYPE, ComPtr<ID3D12PipelineState>> m_pso;
};

